


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


// Consume fuel items if needed to refuel the suit
void UEMC_FuelModule::TryReload()
{
	

	AFGCharacterPlayer* pawn = Cast< AFGCharacterPlayer>(Parent->EquipmentParent->GetInstigator());
	if (pawn)
	{
		if (!pawn->HasAuthority())
			return;

		if (Parent->Stats.HasAdvancedFlag(ESuitFlagAdvanced::SuitFlagAdvanced_NoRefuel))
		{
			return;
		}
		// cycle through each fuel item specified in 'cost to use' in the order they are listed (top has highest priority)
		for (const TSubclassOf<class UFGItemDescriptor> & i : nAllowedFuels)
			if (ConsumeFuelItem(pawn->GetInventory(), i, 1))
				break;
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
			Parent->OnConsumeFuelItem.Broadcast(inClass, 0, 1);
			return true;
		}
	}
	return false;
}

// Only Server
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
	if (!Parent->EquipmentParent->HasAuthority())
	{
		return;
	}
	float Fuel = 0;

	for (auto * i : Parent->AttachmentModule->Attachments)
		if (i)
			Fuel += i->GetDeltaFuelConsumption(Parent->Delta);

	if (Parent->nFuelConsumption != Fuel)
	{
		Parent->nFuelConsumption = Fuel;
	}
}

// Server Only
void UEMC_FuelModule::PostTick() const
{
	if (!Parent->EquipmentParent->HasAuthority())
	{
		return;
	}
	if (Parent->nProducing)
	{
		if (!Parent->EquipmentParent)
			return;

		if (Parent->nFuelAmount <= 0 && Parent->nFuelConsumption >= 0.f)
			return;

		// From Percent to Actual MJ Amount
		// Subtracting the Fuel Consumption Cost in MJ for this frame(Delta)
		// division by MJ of the Item brings us back to Percent
		const float MJCurrent = Parent->GetSuitPropertySafe(ESuitProperty::nFuelTankSize).value() * FMath::Clamp(Parent->nFuelAmount, 0.f, 1.f);
		Parent->nFuelAmount = FMath::Clamp(((MJCurrent - (Parent->nFuelConsumption * Parent->Delta)))/FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nFuelTankSize).value(), 1.f, 999999999.f), 0.f, 1.f);
	}
}


