


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

	return Parent->PowerConsumption;
}

// Get current power capacity from parent
float UEMC_PowerModule::GetPowerCapacity() const
{
	if (!Parent->EquipmentParent)
		return 0;

	return Parent->nPowerCapacity;
}
float UEMC_PowerModule::GetFuseTimerDuration() const
{
	return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nFuseTime).value(), 0.f, 10000.f);
}

void UEMC_PowerModule::CacheFuseTimerDuration()
{
	CachedFuseTime = GetFuseTimerDuration();
}
float UEMC_PowerModule::GetOverDrawDuration() const
{
	return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value(), 0.f, 10000.f);
};


bool UEMC_PowerModule::IsFuseIntact() const
{
	// If the suit has been overdrawn for more than the suit's max overdraw time ...
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
		if (Parent->Stats.HasFlag(ESuitFlag::SuitFlag_FuseNeverBreaksWhenFueled)  && Parent->nFuelAmount > 0)
		{
			// ... and the suit/modules does NOT prevent fuse breaking when there is fuel ...
			
		}
		else 
		{
			if(!IsFuseIntact())
				Parent->nProducing = false;
		}
	}
}

// Handle power consumption/generation and fuse break timer setting
void UEMC_PowerModule::RegenPower() const
{

	const float PowerBalance = GetPowerDraw();
	const float DeltaPower = PowerBalance* Parent->Delta;
	// reading value from hover pack since it modified power last frame after we did 
	Parent->nCurrentPower = FMath::Clamp(Parent->nCurrentPower - DeltaPower, 0.f, Parent->nPowerCapacity);
	
	// If out of power and the suit has been overdrawn for more than the suit's max overdraw time, set the fusebreak overdraw time to now
	if (Parent->nCurrentPower == 0.f)
		if (FTimespan(FDateTime::Now() - Parent->nFuseBreakOverDraw).GetTotalSeconds() > Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value() + 1)
		{
			Parent->nFuseBreakOverDraw = FDateTime::Now();
			Parent->Client_FuseBreakOverDraw();
		}

}

// How long has it been since the fuse break event (not the overdraw, which started before the fuse break)
FTimespan UEMC_PowerModule::TimeSinceFuseBreak() const
{
	return FDateTime::Now() - Parent->nFuseBreak;
}

float UEMC_PowerModule::RemainingSecondsUntilFuseRestart() const
{
	return CachedFuseTime - TimeSinceFuseBreak().GetTotalSeconds();
}

void UEMC_PowerModule::TryRestart() const
{
	// If the FuseTimer has ran out ...
	if (CachedFuseTime - TimeSinceFuseBreak().GetTotalSeconds() <= 0.0f)
	{
		Parent->nCurrentPower = 0.01f;
		Parent->nProducing = true; // restart power production
		Parent->SuitState = EPowerSuitState::PS_NONE;
		Parent->OnFuseTriggered.Broadcast(Parent->nProducing, Parent->nFuseBreak);
		Parent->InventoryModule->BulkUpdateStats(Parent->nInventory);
	}
}

void UEMC_PowerModule::ForcefullyBreakFuse(bool drainPower)
{
	// Shut down the suit since the fuse has blown, starting the restart process

	if (drainPower)
		Parent->nCurrentPower = 0.0f;

	if (Parent->nProducing)
		Parent->nProducing = false;

	Parent->SuitState = EPowerSuitState::PS_REBOOTING;
	CacheFuseTimerDuration();
	Parent->ResetStats(); // Reset movement stats back to player default
	Parent->nFuseBreak = FDateTime::Now();
	Parent->nFuseBreakOverDraw = FDateTime::Now();
	Parent->OnFuseTriggered.Broadcast(Parent->nProducing, Parent->nFuseBreak);
	Parent->nShieldDmg = FDateTime::Now();

	Parent->Client_FuseBreak();
	Parent->Client_FuseBreakOverDraw();
	Parent->Client_ShieldDmg();
}

void UEMC_PowerModule::TryBreakFuse()
{
	// we already failed to Produce but aren't Rebooting yet 
		// sanity check ... we haven't just done this right?
	if (FTimespan(FDateTime::Now() - Parent->nFuseBreak).GetTotalSeconds() > GetFuseTimerDuration())
	{
		ForcefullyBreakFuse(false);
	}
	else
	{
		UE_LOG(PowerSuit_Log, Error, TEXT(" IF you see this, Debug me. Something Toggled Suit State while Fuse was broken"));
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
		TryBreakFuse();
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
	if (Parent->nProducing)
	{
		UpdateProductionState();
		if (Parent->nProducing)
			RegenPower();
	}
}
