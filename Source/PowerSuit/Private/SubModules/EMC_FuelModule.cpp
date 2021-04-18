


#include "SubModules/EMC_FuelModule.h"
#include "FGCharacterPlayer.h"
#include "SubModules/EMC_PowerModule.h"
#include "SubModules/EMC_AttachmentModule.h"

#include "Attachment/PowerSuitModuleAttachment.h"
#include "Equipment/Equipment_PowerSuit.h"

void UEMC_FuelModule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEMC_FuelModule, DeltaFuelConsumption);
	DOREPLIFETIME(UEMC_FuelModule, FuelAmountBuffer);

}


// Consume fuel items if needed to refuel the suit
void UEMC_FuelModule::TryReload()
{
	

	AFGCharacterPlayer* pawn = Cast< AFGCharacterPlayer>(Parent->EquipmentParent->GetInstigator());
	if (pawn)
	{
		if (!pawn->HasAuthority())
			return;
		// cycle through each fuel item specified in 'cost to use' in the order they are listed (top has highest priority)
		for (const TSubclassOf<class UFGItemDescriptor> & i : nAllowedFuels)
			if (ConsumeFuelItem(pawn->GetInventory(), i, 1))
				break;
	}
}

bool UEMC_FuelModule::ConsumeFuelItem(UFGInventoryComponent* Inventory, TSubclassOf< class UFGItemDescriptor > inClass, int32 inAmount)
{
	if (Inventory && inClass && inAmount != 0)
	{
		if (Inventory->HasItems(inClass, inAmount))
		{
			Inventory->Remove(inClass, inAmount);
			FuelAmountBuffer = FMath::Clamp(inAmount* inClass.GetDefaultObject()->mEnergyValue * Parent->GetSuitPropertySafe(ESuitProperty::nFuelEfficiency).clampAdd(1), 0.f, Parent->GetSuitPropertySafe(ESuitProperty::nFuelTankSize).value());
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
		// only perform refueling actions if allowed to - NOT flying/falling, unless an installed module grants flight refueling
		if (!((Parent->nMovementMode != EMovementMode::MOVE_Flying && Parent->nMovementMode != EMovementMode::MOVE_Falling) || Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasAirReFuel)))
			return;

		// if the fuel buffer is empty, consume items to refill it
		if (FuelAmountBuffer <= 0)
		{
			TryReload();
		}
	}
}

// Server and Client
void UEMC_FuelModule::Tick()
{
	float fuel = 0;

	for (auto * i : Parent->AttachmentModule->Attachments)
		if (i)
			fuel += i->GetDeltaFuelConsumption(Parent->Delta);

	if (DeltaFuelConsumption != fuel)
	{
		DeltaFuelConsumption = fuel;
	}
}

// Server Only
void UEMC_FuelModule::PostTick()
{
	if (!Parent->EquipmentParent->HasAuthority())
	{
		return;
	}
	if (Parent->PowerModule->Producing)
	{
		if (!Parent->EquipmentParent)
			return;

		if (FuelAmountBuffer <= 0)
			return;

		// From Percent to Actual MJ Amount
		// Subtracting the Fuel Consumption Cost in MJ for this frame(Delta)
		// division by MJ of the Item brings us back to Percent
		const float MJcurrent = Parent->GetSuitPropertySafe(ESuitProperty::nFuelTankSize).value() * FMath::Clamp(FuelAmountBuffer, 0.f, 1.f);
		FuelAmountBuffer = FMath::Clamp(((FuelAmountBuffer - (DeltaFuelConsumption * Parent->Delta))), 0.f, 1.f);
	}
}


