


#include "SubModules/EMC_FuelModule.h"
#include "FGCharacterPlayer.h"
#include "SubModules/EMC_PowerModule.h"
#include "SubModules/EMC_AttachmentModule.h"

#include "Attachment/PowerSuitModuleAttachment.h"
#include "Equipment/Equipment_PowerSuit.h"

void UEMC_FuelModule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UEMC_FuelModule::FuelFuseRecoveryPeriodElapsed() const {
	return FTimespan(TimeSinceFuelFuseBreak()).GetTotalSeconds() >= GetFuelFuseTimerDuration();
}

float UEMC_FuelModule::GetFuelFuseTimerDuration() const
{
	return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nFuelFuseTime).value(), 1.f, 10000.f);
}

FTimespan UEMC_FuelModule::TimeSinceFuelFuseBreak() const
{
	return FDateTime::Now() - Parent->nFuelFuseBreak;
}

void UEMC_FuelModule::ForcefullyBreakFuelFuse(bool drainFuel)
{
	// Shut down the suit since the fuse has blown, starting the restart process

	if (drainFuel) {
		Parent->nFuelAmount = 0.0f;
	}

	bool wasFuelFuseIntact = Parent->nFuelFuseStatus;

	Parent->nFuelFuseBreak = FDateTime::Now();
	Parent->nFuelFuseStatus = false;
	Parent->Client_FuelFuseBreak();
	if (wasFuelFuseIntact) {
		// Only broadcast the event if it's a "new" fuse break, not a continued still down
		UE_LOG(PowerSuit_Log, Display, TEXT("Broadcasting event for FuelFuse break"));
		Parent->OnFuelFuseTriggered.Broadcast(Parent->nFuelFuseStatus, Parent->nFuelFuseBreak);
	}
}

// Consume fuel items if needed to refuel the suit
void UEMC_FuelModule::TryReload()
{
	AFGCharacterPlayer* pawn = Cast< AFGCharacterPlayer>(Parent->EquipmentParent->GetInstigator());
	if (pawn)
	{
		if (!pawn->HasAuthority())
			return;

		// allow custom refuel implementations
		if (!Parent->Stats.HasAdvancedFlag(ESuitFlagAdvanced::SuitFlagAdvanced_NoRefuel)) {
			// cycle through each fuel item specified in 'cost to use' in the order they are listed (top has highest priority)
			for (const TSubclassOf<class UFGItemDescriptor>& i : nAllowedFuels) {
				if (ConsumeFuelItem(pawn->GetInventory(), i, 1)) {
					break;
				}
			}
		}
		
	}
}

bool UEMC_FuelModule::ConsumeFuelItem(UFGInventoryComponent* Inventory, TSubclassOf< class UFGItemDescriptor > inClass, int32 inAmount) const
{
	if (Inventory && inClass && inAmount != 0)
	{
		if (Inventory->HasItems(inClass, inAmount))
		{
			Inventory->Remove(inClass, inAmount);
			const float FuelEfficiency = Parent->GetSuitPropertySafe(ESuitProperty::nFuelEfficiency).ClampAdd(1);
			const float ItemMJ = inAmount * inClass.GetDefaultObject()->mEnergyValue;
			const float TankSize = FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nFuelTankSize).value(), 1.f, 99999999999.f);
			const float MJPercent = (ItemMJ * FuelEfficiency) / TankSize;
			Parent->nFuelAmount = FMath::Clamp(MJPercent, 0.f, 1.f);
			Parent->OnConsumeFuelItem.Broadcast(inClass, 0, inAmount);
			return true;
		}
	}
	return false;
}

// Server Only
void UEMC_FuelModule::PreTick()
{
	if (Parent->EquipmentParent->HasAuthority()) 
	{
		if (Parent->nMovementMode == EMovementMode::MOVE_Flying || Parent->nMovementMode == EMovementMode::MOVE_Falling || Parent->nMovementMode == EMovementMode::MOVE_Custom)
		{
			// incase we are not moving on the ground 
			if (!Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasAirReFuel))
				return;
		}
		
		// if the fuel buffer is empty, consume items to refill it
		if (Parent->nFuelAmount <= 0)
		{
			TryReload();
		}
	}
}

// Server Only
void UEMC_FuelModule::Tick() const
{
	if (!Parent->EquipmentParent->HasAuthority()) {
		return;
	}
	float fuelConsumptionFromAttachments = 0;

	for (auto* attach : Parent->AttachmentModule->Attachments) {
		if (attach) {
			fuelConsumptionFromAttachments += attach->GetDeltaFuelConsumption(Parent->LastDeltaTime);
		}
	}

	// UE_LOG(PowerSuit_Log, Display, TEXT("Tick: Fuel consumption from attach is %f"), fuelConsumptionFromAttachments);

	if (Parent->nFuelConsumption != fuelConsumptionFromAttachments) {
		Parent->nFuelConsumption = fuelConsumptionFromAttachments;
	}
}

// Server Only
void UEMC_FuelModule::PostTick()
{
	if (!Parent->EquipmentParent->HasAuthority()) {
		return;
	}
	if (Parent->nProducing) {
		if (!Parent->EquipmentParent) {
			return;
		}

		const bool desiresFuel = (Parent->nFuelConsumption > 0.f) || Parent->Stats.HasAdvancedFlag(ESuitFlagAdvanced::SuitFlagAdvanced_AlwaysWantsFuel);
		if (Parent->nFuelAmount <= 0.01 && desiresFuel) {
			if (Parent->nFuelFuseStatus) {
				// UE_LOG(PowerSuit_Log, Display, TEXT("PostTick: Fuel amount <= 0.01 and there was consumption, breaking fuel fuse"));
				ForcefullyBreakFuelFuse(false);
			}
			else {
				// UE_LOG(PowerSuit_Log, Display, TEXT("PostTick: Fuel amount <= 0.01 and there was consumption, BUT fuse already broken"));
			}
			// return;
		} else if (!Parent->nFuelFuseStatus) {
			// UE_LOG(PowerSuit_Log, Display, TEXT("PostTick: TryRestart"));
			TryRestart();
		}

		// From Percent to Actual MJ Amount
		// Subtracting the Fuel Consumption Cost in MJ for this frame(Delta)
		// division by MJ of the Tank brings us back to Percent
		const float FuelTankSizeMJ = FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nFuelTankSize).value(), 1.f, 999999999.f);
		const float StoredFuelMJ = FuelTankSizeMJ * FMath::Clamp(Parent->nFuelAmount, 0.f, 1.f);
		const float FuelChangeThisDeltaTimeMJ = Parent->nFuelConsumption * Parent->LastDeltaTime;
		// UE_LOG(PowerSuit_Log, Display, TEXT("PostTick: Fuel Delta is %f"), FuelChangeThisDeltaTimeMJ);

		const float NewFuelAmount = FMath::Clamp((StoredFuelMJ - FuelChangeThisDeltaTimeMJ) / FuelTankSizeMJ, 0.f, 1.f);
		// UE_LOG(PowerSuit_Log, Display, TEXT("PostTick: NewFuelAmount is %f"), NewFuelAmount);
		Parent->nFuelAmount = NewFuelAmount;
	}
}

void UEMC_FuelModule::TryRestart() const
{
	if (FuelFuseRecoveryPeriodElapsed()) {
		if (Parent->nFuelAmount > 0.01) {
			UE_LOG(PowerSuit_Log, Display, TEXT("Restarting fuel fuse because enough time has elapsed and we have > 0.01 fuel"));
			Parent->nFuelFuseStatus = true;
			Parent->Client_FuelFuseRecover();
			Parent->OnFuelFuseTriggered.Broadcast(Parent->nFuelFuseStatus, Parent->nFuelFuseBreak);
		} else {
			// UE_LOG(PowerSuit_Log, Display, TEXT("Enough time elapsed, but not enough fuel to restart"));
		}
	} else {
		// UE_LOG(PowerSuit_Log, Display, TEXT("Waiting on time elapse to try restarting"));
	}
}
