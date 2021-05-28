


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
}

// Calculate current power production/consumption factoring in AddedDeltaPower
float UEMC_PowerModule::GetPowerDraw() const
{
	if (!Parent->EquipmentParent)
		return 0;
#ifdef MODDING_SHIPPING
	float & mCurrentPowerLevel_ = Parent->EquipmentParent->mCurrentPowerLevel;
	float & mPowerCapacity_ = Parent->EquipmentParent->mPowerCapacity;
#else
	float & mCurrentPowerLevel_ = Parent->EquipmentParent->*get(steal_mCurrentPowerLevel());
	float & mPowerCapacity_ = Parent->EquipmentParent->*get(steal_mPowerCapacity());
#endif

	const float pw = FMath::Abs(Parent->nCurrentPower - mCurrentPowerLevel_) * (1/Parent->Delta);

	return Parent->PowerConsumption + pw;
}

// Calculate current power production/consumption factoring in AddedDeltaPower
float UEMC_PowerModule::GetPowerCapacity() const
{
	if (!Parent->EquipmentParent)
		return 0;
#ifdef MODDING_SHIPPING
	float& mPowerCapacity_ = Parent->EquipmentParent->mPowerCapacity;
#else
	float& mPowerCapacity_ = Parent->EquipmentParent->*get(steal_mPowerCapacity());
#endif
	return mPowerCapacity_;
}
float UEMC_PowerModule::GetFuseTimerDuration() const
{
	return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nFuseTime).value(), 0.f, 10000.f);
}
float UEMC_PowerModule::GetOverDrawDuration() const
{
	return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value(), 0.f, 10000.f);
};


bool UEMC_PowerModule::IsFuseIntact() const
{
	// ... and the suit has been overdrawn for more than the suit's max overdraw time ...
	if (FTimespan(FDateTime::Now() - Parent->nFuseBreakOverDraw).GetTotalSeconds() > Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value())
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
void UEMC_PowerModule::UpdateProductionState() const
{


	// Ff the suit is out of power...
	if (Parent->nCurrentPower <= 0.f)
	{
		// ... and the suit has fuel ...
		if (Parent->nFuelAmount > 0)
		{
			// ... and the suit/modules does NOT prevent fuse breaking when there is fuel ...
			if (!Parent->Stats.HasFlag(ESuitFlag::SuitFlag_FuseNeverBreaksWhenFueled))
			{
				Parent->nProducing = IsFuseIntact();
			}
			else 
			{
				// ... then the suit is fuel-fuse-break-protected and should be producing
				Parent->nProducing = true;
			}
		}
		else 
		{
			Parent->nProducing = IsFuseIntact();
		}
	}
}

// Handle power consumption/generation and fuse break timer setting
void UEMC_PowerModule::RegenPower() const
{

	const float PowerBalance = GetPowerDraw();
	const float DeltaPower = PowerBalance* Parent->Delta;

#ifdef MODDING_SHIPPING
	float & mCurrentPowerLevel_ = Parent->EquipmentParent->mCurrentPowerLevel;
	float & mPowerCapacity_ = Parent->EquipmentParent->mPowerCapacity;
#else
	float & mCurrentPowerLevel_ = Parent->EquipmentParent->*get(steal_mCurrentPowerLevel());
	float & mPowerCapacity_ = Parent->EquipmentParent->*get(steal_mPowerCapacity());
#endif
	// reading value from hover pack since it modified power last frame after we did 
	Parent->nCurrentPower = FMath::Clamp(Parent->nCurrentPower - DeltaPower, 0.f, mPowerCapacity_);
	// now set value to it so it uses our  modifications as well
	mCurrentPowerLevel_ = Parent->nCurrentPower;
	// If out of power and the suit has been overdrawn for more than the suit's max overdraw time, set the fusebreak overdraw time to now
	if (Parent->nCurrentPower == 0.f)
		if (FTimespan(FDateTime::Now() - Parent->nFuseBreakOverDraw).GetTotalSeconds() > Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value() + 1)
			Parent->nFuseBreakOverDraw = FDateTime::Now();

}



void UEMC_PowerModule::TryRestart() const
{
	const FTimespan timeSinceFuseBreak = FDateTime::Now() - Parent->nFuseBreak;
	// ... and the FuseTimer has ran out ...
	if (timeSinceFuseBreak.GetTotalSeconds() > GetFuseTimerDuration())
	{
#ifdef MODDING_SHIPPING
		float & mCurrentPowerLevel_ = Parent->EquipmentParent->mCurrentPowerLevel;
#else
		float & mCurrentPowerLevel_ = Parent->EquipmentParent->*get(steal_mCurrentPowerLevel());
#endif
		mCurrentPowerLevel_ = 0.01f;
		Parent->nCurrentPower = 0.01f;

		Parent->nProducing = true; // restart power production

		Parent->InventoryModule->BulkUpdateStats(Parent->nInventory);
		
	}
}

void UEMC_PowerModule::BreakFuse()
{
	// we already failed to Produce but arent Rebooting yet 
		// sanity check .. we havnt just done this right?
	if (FTimespan(FDateTime::Now() - Parent->nFuseBreak).GetTotalSeconds() > GetFuseTimerDuration())
	{
		// Restart the suit since the fuse has reset
		Parent->SuitState = EPowerSuitState::PS_REBOOTING;
		Parent->ResetStats(); // Reset movement stats back to player default
		Parent->nFuseBreak = FDateTime::Now();
		Parent->OnFuseTriggered.Broadcast(Parent->nProducing, Parent->nFuseBreak);
		Parent->nShieldDmg = FDateTime::Now();
	}
	else
	{
		UE_LOG(PowerSuit_Log, Error, TEXT(" IF you see this, Dubug me. Something Toggled Suit State while Fuse was broken"));
	}
}

// Handle power consumption/generation and fuse break timer setting
// Server Only
void UEMC_PowerModule::PreTick() {
	// if the suit is not producing power (was off because of FuseTimer) ...
	if (Parent->SuitState == EPowerSuitState::PS_REBOOTING && !Parent->nProducing)
	{
		TryRestart();
	}
	else if (!Parent->nProducing)
	{
		BreakFuse();
	}
	

}


// Server Only
void UEMC_PowerModule::Tick() const
{
	if (Parent->nProducing)
	{
		float powerCost = 0.f;

		powerCost += Parent->GetSuitPropertySafe(ESuitProperty::nPowerConsumption).Clamp();
		powerCost -= Parent->GetSuitPropertySafe(ESuitProperty::nPowerProduction).Clamp();

		for (auto * i : Parent->AttachmentModule->Attachments)
			if (i)
				powerCost += i->GetDeltaPowerConsumption(Parent->Delta);

		if (Parent->PowerConsumption != powerCost)
		{
			Parent->PowerConsumption = powerCost;
		}
	}

}

// Handle power consumption/generation and fuse break timer setting
// Server Only
void UEMC_PowerModule::PostTick() const
{
	// Check if producing power now (suit is on) *after* seeing if it should be re-enabled (now that FuseTimer reboot checking has been done)
	UpdateProductionState();
	if (Parent->nProducing)
	{
		// adjust this frame's power state
		RegenPower();
	}
}
