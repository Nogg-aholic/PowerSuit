


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
	return (Parent->PowerConsumption - Parent->PowerProduction);
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

// I think this is used to record the fuse timer when we're going to blow the fuse, since modules get uninstalled during fuse break -> we wouldn't know the real time if we didn't cache it first - Rob
void UEMC_PowerModule::CacheFuseTimerDuration()
{
	CachedFuseTime = GetFuseTimerDuration();
}

float UEMC_PowerModule::GetOverDrawDuration() const
{
	return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value(), 0.f, 10000.f);
};

FTimespan UEMC_PowerModule::TimeSinceFuseBreak() const
{
	return FDateTime::Now() - Parent->nFuseBreak;
}

FTimespan UEMC_PowerModule::TimeSinceOverdrawStart() const
{
	return FDateTime::Now() - Parent->nFuseBreakOverDraw;
}

float UEMC_PowerModule::RemainingSecondsUntilFuseRestart() const
{
	return CachedFuseTime - TimeSinceFuseBreak().GetTotalSeconds();
}

// Is the time since Overdraw starting less than the suit's allowed overdraw time? If so, then the fuse is still intact
bool UEMC_PowerModule::IsFuseIntact() const
{
	//// If the suit has been overdrawn for more than the suit's max overdraw time ...
	//if (FTimespan(TimeSinceOverdrawStart()).GetTotalSeconds() > Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value())
	//{
	//	// ... then suit should stop producing because it has exceeded the overdraw timeframe
	//	return false;
	//}
	//else
	//{
	//	// ... then the suit still should be producing because it's within the overdraw timeframe
	//	return true;
	//}
	return FTimespan(TimeSinceOverdrawStart()).GetTotalSeconds() <= Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value();
}

// Run checks to update and return if suit is producing power
bool UEMC_PowerModule::UpdateProductionState() const
{
	// If the suit is out of power...
	if (Parent->nCurrentPower <= 0.01f)
	{
		if (Parent->Stats.HasFlag(ESuitFlag::SuitFlag_FuseNeverBreaksWhenFueled)  && Parent->nFuelAmount > 0)
		{
			// ... the suit has fuel AND the suit/modules prevent fuse breaking when there is fuel ...
			// (don't break fuse)
		}
		else 
		{
			// If we have been overdrawing longer than the overdraw time (fuse no longer intact)
			if (!IsFuseIntact()) {
				// Set to not producing to set up for a fuse break
				Parent->nProducing = false;
				return false;
			}
		}
	}
	return true;
}

// Handle power consumption/generation and fuse break timer setting
void UEMC_PowerModule::RegenPower()
{
	const float DeltaPower = GetPowerDraw() * Parent->LastDeltaTime;

	// reading value from hover pack since it modified power last frame after we did
	const float powerAfterHoverPackModified = Parent->nCurrentPower;

	const float newCurrentPower = FMath::Clamp(powerAfterHoverPackModified - DeltaPower, 0.f, GetPowerCapacity());
	Parent->nCurrentPower = newCurrentPower;
	
	// Are we out of power?
	if (Parent->nCurrentPower <= 0.01f)
	{
		// UE_LOG(PowerSuit_Log, Display, TEXT("Time since overdraw is %f / %f"), (float) TimeSinceOverdrawStart().GetTotalSeconds(), (float) Parent->GetSuitPropertySafe(ESuitProperty::nFuseTimeOverDraw).value());
		// Are we still within the overdraw time?
		if (IsFuseIntact()) {

			if (!SentOverdrawStartEvent) {
				// Send out the "overdraw start" notification if we haven't yet
				SentOverdrawStartEvent = true;
				Parent->Client_FuseBreakOverDraw();

				Parent->OnFuseOverDrawTriggered.Broadcast(false, Parent->nFuseBreakOverDraw);
			}

		} else {
			// This block is never called? Seems like restart happens first
			// Exceeded overdraw -> should fuse break now?

			/*
			// This should only end up called once per overdraw since these calls should put it in a mode where it's no longer producing power -> RegenPower not called?
			UE_LOG(PowerSuit_Log, Display, TEXT("23131 Overdraw called"));
			Parent->Client_FuseBreakOverDraw();
			
			// TODO not sure if this is still needed here?
			Parent->OnFuseOverDrawTriggered.Broadcast(false, Parent->nFuseBreakOverDraw);
			*/ 
		}
	}
	else
	{
		// Record now as the last time we still had power
		Parent->nFuseBreakOverDraw = FDateTime::Now();

		if (SentOverdrawStartEvent) {
			// Send the "overdraw recovery" event if we haven't yet
			SentOverdrawStartEvent = false;
			Parent->Client_FuseBreakOverDraw();
			Parent->OnFuseOverDrawTriggered.Broadcast(true, Parent->nFuseBreakOverDraw);
		}
		
	}
}

void UEMC_PowerModule::TryRestart() const
{
	// If the FuseTimer has ran out ...
	if (FMath::Clamp(CachedFuseTime,0.1f,GetFuseTimerDuration()) - TimeSinceFuseBreak().GetTotalSeconds() <= 0.0f)
	{
		Parent->nCurrentPower = 20.f; // 20 MWs
		Parent->nProducing = true; // restart power production
		Parent->SuitState = EPowerSuitState::PS_NONE;
		Parent->OnFuseTriggered.Broadcast(Parent->nProducing, Parent->nFuseBreak);
		Parent->InventoryModule->BulkUpdateStats(Parent->nInventory);
		Parent->nFuseBreakOverDraw = FDateTime::Now();
		Parent->EquipmentParent->OverrideReboot = false;
	}
}

// Break the fuse regardless of suit state. If passed `true`, also remove all of the power from the suit
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
	if (FTimespan(TimeSinceFuseBreak()).GetTotalSeconds() > GetFuseTimerDuration())
	{
		if (Parent->EquipmentParent->OverrideReboot)
			TryRestart();
		else
			ForcefullyBreakFuse(false); 
	}
	else
	{
		UE_LOG(PowerSuit_Log, Error, TEXT(" IF you see this, Debug me. Something Toggled Suit State while Fuse was broken"));
	}
}

// Server Only. Handle trying to restart the suit or break its fuse if needed
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


// Server Only. Gather power production/costs from SuitProps and attachments, to be used by PostTick to actuall do power cost calc
void UEMC_PowerModule::Tick() const
{
	if (Parent->nProducing)
	{
		float powerCost = 0.f;
		float powerProduction = 0.f;

		powerCost += Parent->GetSuitPropertySafe(ESuitProperty::nPowerConsumption).Clamp();
		powerProduction += Parent->GetSuitPropertySafe(ESuitProperty::nPowerProduction).Clamp();

		for (auto* i : Parent->AttachmentModule->Attachments) {
			if (i)
			{
				powerCost += i->GetDeltaPowerConsumption(Parent->LastDeltaTime);
				powerProduction += i->GetDeltaPowerProduction(Parent->LastDeltaTime);
			}
		}

		if (Parent->PowerConsumption != powerCost)
		{
			Parent->PowerConsumption = FMath::Clamp(powerCost,0.f, 999999999.f);
		}
		if (Parent->PowerProduction!= powerProduction)
		{
			Parent->PowerProduction = FMath::Clamp(powerProduction, 0.f, 999999999.f);
		}
	}

}

// Server Only. Handle power consumption/generation and fuse break timer setting
void UEMC_PowerModule::PostTick()
{
	// Check if producing power now (suit is on) *after* seeing if it should be re-enabled (now that FuseTimer reboot checking has been done)
	if (Parent->nProducing)
	{
		bool shouldStillBeProducing = UpdateProductionState();
		if (shouldStillBeProducing) {
			RegenPower();
		}
	}
}
