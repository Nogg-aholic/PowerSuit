


#include "SubModules/EMC_PowerModule.h"
#include "SubModules/EMC_FuelModule.h"
#include "SubModules/EMC_ShieldModule.h"
#include "SubModules/EMC_AttachmentModule.h"
#include "SubModules/EMC_InventoryModule.h"

#include "EquipmentModuleComponent.h"
#include "FGCharacterPlayer.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "Attachment/PowerSuitModuleAttachment.h"
#include "Equipment/Equipment_PowerSuit.h"


void UEMC_PowerModule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEMC_PowerModule, PowerConsumption);
	DOREPLIFETIME(UEMC_PowerModule, Producing);
	DOREPLIFETIME(UEMC_PowerModule, Active);
	DOREPLIFETIME(UEMC_PowerModule, ActiveTimer);
	DOREPLIFETIME(UEMC_PowerModule, FuseBreak);
	DOREPLIFETIME(UEMC_PowerModule, FuseBreakOverDraw);
}

// Calculate current power production/consumption factoring in AddedDeltaPower
float UEMC_PowerModule::GetPowerDraw()
{
	if (!Parent->EquipmentParent)
		return 0;

	return PowerConsumption;
}

float UEMC_PowerModule::GetActiveTimerDuration()
{
	return  FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nActiveTime).value(), 0.f, 10000.f);
}
float UEMC_PowerModule::GetFuseTimerDuration()
{
	return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nFuseTime).value(), 0.f, 10000.f);
}
float UEMC_PowerModule::GetOverDrawDuration()
{
	return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value(), 0.f, 10000.f);
};


bool UEMC_PowerModule::IsFuseIntact()
{
	// ... and the suit has been overdrawn for more than the suit's max overdraw time ...
	if (FTimespan(FDateTime::Now() - FuseBreakOverDraw).GetTotalSeconds() > Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value())
	{
		// ... then suit should stop producing because it has exceeded the overdraw timeframe
		return false;
	}
	else
	{
		// ... then the suit still should be producing because it's within the overdraw timeframe
		return true;
	}
}

// Run checks to update and return if suit is producing power
void UEMC_PowerModule::UpdateProductionState() {


	// Ff the suit is out of power...
	if (CurrentPower <= 0.f)
	{
		// ... and the suit has fuel ...
		if (Parent->FuelModule->FuelAmountBuffer > 0)
		{
			// ... and the suit/modules does NOT prevent fuse breaking when there is fuel ...
			if (!Parent->Stats.HasFlag(ESuitFlag::SuitFlag_FuseNeverBreaksWhenFueled))
			{
				Producing = IsFuseIntact();
			}
			else 
			{
				// ... then the suit is fuel-fuse-break-protected and should be producing
				Producing = true;
			}
		}
		else 
		{
			Producing = IsFuseIntact();
		}
	}
}

// Handle power consumption/generation and fuse break timer setting
void UEMC_PowerModule::RegenPower() {

	const float PowerBalance = PowerConsumption;
	const float DeltaPower = PowerBalance* Parent->Delta;

#ifdef MODDING_SHIPPING
	float & mCurrentPowerLevel_ = Parent->EquipmentParent->mCurrentPowerLevel;
	float & mPowerCapacity_ = Parent->EquipmentParent->mPowerCapacity;
#else
	float & mCurrentPowerLevel_ = Parent->EquipmentParent->*get(steal_mCurrentPowerLevel());
	float & mPowerCapacity_ = Parent->EquipmentParent->*get(steal_mPowerCapacity());
#endif
	// reading value from hover pack since it modified power last frame after we did 
	CurrentPower = FMath::Clamp(mCurrentPowerLevel_ - DeltaPower, 0.f, mPowerCapacity_);
	// now set value to it so it uses our  modifications as well
	mCurrentPowerLevel_ = CurrentPower;
	// If out of power and the suit has been overdrawn for more than the suit's max overdraw time, set the fusebreak overdraw time to now
	if (CurrentPower == 0.f)
		if (FTimespan(FDateTime::Now() - FuseBreakOverDraw).GetTotalSeconds() > Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value() + 1)
			FuseBreakOverDraw = FDateTime::Now();

}



void UEMC_PowerModule::TryRestart()
{
	const FTimespan timeSinceFuseBreak = FDateTime::Now() - FuseBreak;
	// ... and the FuseTimer has ran out ...
	if (timeSinceFuseBreak.GetTotalSeconds() > GetFuseTimerDuration())
	{
#ifdef MODDING_SHIPPING
		float & mCurrentPowerLevel_ = Parent->EquipmentParent->mCurrentPowerLevel;
#else
		float & mCurrentPowerLevel_ = Parent->EquipmentParent->*get(steal_mCurrentPowerLevel());
#endif
		mCurrentPowerLevel_ = 0.01f;
		CurrentPower = 0.01f;

		Producing = true; // restart power production

		if (Active)
		{
			// we do this to prevent to triggering the Fuse instantly again 
			// this will reactivate itself on next frame if we dont offset the Timer
			// needs better solution
			ActiveTimer = ActiveTimer - FTimespan(0, 0, GetActiveTimerDuration());
			Active = false;
		}
		Parent->InventoryModule->BulkUpdateStats(Parent->nInventory);
		
	}
}

void UEMC_PowerModule::BreakFuse()
{
	// we already failed to Produce but arent Rebooting yet 
		// sanity check .. we havnt just done this right?
	if (FTimespan(FDateTime::Now() - FuseBreak).GetTotalSeconds() > GetFuseTimerDuration())
	{
		// Restart the suit since the fuse has reset
		Parent->SuitState = EPowerSuitState::PS_REBOOTING;
		Parent->ResetStats(); // Reset movement stats back to player default
		FuseBreak = FDateTime::Now();
		Parent->OnFuseTriggered.Broadcast(Producing, FuseBreak);
		Parent->ShieldModule->ShieldDmg = FDateTime::Now();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" IF you see this, Dubug me. Something Toggled Suit State while Fuse was broken"));
	}
}

// Handle power consumption/generation and fuse break timer setting
// Server Only
void UEMC_PowerModule::PreTick() {
	// if the suit is not producing power (was off because of FuseTimer) ...
	if (Parent->SuitState == EPowerSuitState::PS_REBOOTING && !Producing)
	{
		TryRestart();
	}
	else if (!Producing)
	{
		BreakFuse();
	}
	

}

// Server Only
void UEMC_PowerModule::GoActive()
{
	if (!Parent->EquipmentParent)
		return;

	if (!Parent->EquipmentParent->GetInstigator()->HasAuthority())
		return;

	if (Producing)
	{
		ActiveTimer = FDateTime::Now();
		Active = true;
	}
}

// Server Only
void UEMC_PowerModule::Tick()
{
	if (Producing)
	{
		const FTimespan timeSinceActiveTimer = FDateTime::Now() - ActiveTimer;

		// Active timer checking
		if (timeSinceActiveTimer.GetTotalSeconds() > GetActiveTimerDuration())
			Active = false;

		float powerCost = 0.f;

		powerCost += Parent->GetSuitPropertySafe(ESuitProperty::nPowerConsumption).clamp();
		powerCost -= Parent->GetSuitPropertySafe(ESuitProperty::nPowerProduction).clamp();

		if (Active)
		{
			powerCost += Parent->GetSuitPropertySafe(ESuitProperty::nPowerConsumptionActive).clamp();
			powerCost -= Parent->GetSuitPropertySafe(ESuitProperty::nPowerProductionActive).clamp();
		}

		for (auto * i : Parent->AttachmentModule->Attachments)
			if (i)
				powerCost += i->GetDeltaPowerConsumption(Parent->Delta);

		if (PowerConsumption != powerCost)
		{
			PowerConsumption = powerCost;
		}
	}

}

// Handle power consumption/generation and fuse break timer setting
// Server Only
void UEMC_PowerModule::PostTick() {
	// Check if producing power now (suit is on) *after* seeing if it should be re-enabled (now that FuseTimer reboot checking has been done)
	UpdateProductionState();
	if (Producing)
	{
		// adjust this frame's power state
		RegenPower();
	}
}