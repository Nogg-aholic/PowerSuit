#include "Stats/PowerSuitData.h"
#include "SubModules/EMC_FuelModule.h"
#include "Attachment/PowerSuitModuleAttachment.h"

DEFINE_LOG_CATEGORY(PowerSuit_Log);


FEquipmentStats::FEquipmentStats(): mCachedAttachment(nullptr)
{
	nMovementProperties = {};
	nFlightProperties = {};
	nSuitProperties = {};
	nDamageTypeResistance = {};
	nDamageResistance = {};
	DamageShieldAbsorption = 0;
	RemoveDamageShieldAbsorption = 0;
	SuitFlags = 0;
	RemoveSuitFlags = 0;
	InventorySlots = 0;
	mCachedInventorySlot = -1;
	nUnlockedAllowedFuels = {};
}

FModMultProperty::FModMultProperty() {
	Multiplier = 0;
	Modifier = 0;
}

void FEquipmentStats::SetupDefaults()
{

	if (nFlightProperties.Num() != static_cast<int32>(ESuitFlightProperty::EFP_MAX))
	{
		for (int32 i = 0; i < ESuitFlightProperty::EFP_MAX; ++i)
			if (!nFlightProperties.Contains(static_cast<ESuitFlightProperty>(i)))
				nFlightProperties.Add(static_cast<ESuitFlightProperty>(i), FModMultProperty());
	}

	if (nMovementProperties.Num() != static_cast<int32>(ESuitMovementProperty::ESMC_MAX))
	{
		for (int32 i = 0; i < ESuitMovementProperty::ESMC_MAX; ++i)
			if (!nMovementProperties.Contains(static_cast<ESuitMovementProperty>(i)))
				nMovementProperties.Add(static_cast<ESuitMovementProperty>(i), FModMultProperty());
	}

	if (nSuitProperties.Num() != static_cast<int32>(ESuitProperty::SuitPropertyMAX))
	{
		for (int32 i = 0; i < ESuitProperty::SuitPropertyMAX; ++i)
			if (!nSuitProperties.Contains(static_cast<ESuitProperty>(i)))
				nSuitProperties.Add(static_cast<ESuitProperty>(i), FModMultProperty());
	}

	if (nDamageTypeResistance.Num() != 5)
	{
		if (!nDamageTypeResistance.Contains(DamageTypePhysical))
			nDamageTypeResistance.Add(DamageTypePhysical, FModMultProperty());
		if (!nDamageTypeResistance.Contains(DamageTypeGas))
			nDamageTypeResistance.Add(DamageTypeGas, FModMultProperty());
		if (!nDamageTypeResistance.Contains(DamageTypePhysical))
			nDamageTypeResistance.Add(DamageTypePhysical, FModMultProperty());
		if (!nDamageTypeResistance.Contains(DamageTypeRadioActivity))
			nDamageTypeResistance.Add(DamageTypeRadioActivity, FModMultProperty());
		if (!nDamageTypeResistance.Contains(DamageTypeFallDamage))
			nDamageTypeResistance.Add(DamageTypeFallDamage, FModMultProperty());
		if (!nDamageTypeResistance.Contains(DamageTypeHeat))
			nDamageTypeResistance.Add(DamageTypeHeat, FModMultProperty());
	}

}
void FEquipmentStats::UnlockFuels(UEquipmentModuleComponent* Parent, TArray<TSubclassOf<class UFGItemDescriptor>> FuelsToUnlock)
{
	for (auto i : FuelsToUnlock)
	{
		if (!Parent->FuelModule->nAllowedFuels.Contains(i))
		{
			Parent->FuelModule->nAllowedFuels.Add(i);
			nUnlockedAllowedFuels.Add(i);
		}
	}
};
void FEquipmentStats::ForgetUnlockedFuels(UEquipmentModuleComponent* Parent)
{
	for (auto e : nUnlockedAllowedFuels) {
		if (Parent->FuelModule->nAllowedFuels.Contains(e))
		{
			Parent->FuelModule->nAllowedFuels.Remove(e);
		}
	}
}

FEquipmentStats FEquipmentStats::operator+(const FEquipmentStats& OtherStruct)
{
	SetupDefaults();

	if (OtherStruct.nMovementProperties.Num() > 0)
	{
		TArray<TEnumAsByte<ESuitMovementProperty>> MovementProperties;
		OtherStruct.nMovementProperties.GetKeys(MovementProperties);
		for (auto i : MovementProperties)
		{
			nMovementProperties.Add(i, *nMovementProperties.Find(i) % *OtherStruct.nMovementProperties.Find(i));
		}
	}

	if (OtherStruct.nFlightProperties.Num() > 0)
	{
		TArray<TEnumAsByte<ESuitFlightProperty>> FlightProperties;
		OtherStruct.nFlightProperties.GetKeys(FlightProperties);
		for (auto i : FlightProperties)
		{
			nFlightProperties.Add(i, *nFlightProperties.Find(i) % *OtherStruct.nFlightProperties.Find(i));
		}
	}

	if (OtherStruct.nSuitProperties.Num() > 0)
	{
		TArray<TEnumAsByte<ESuitProperty>> SuitProps;
		OtherStruct.nSuitProperties.GetKeys(SuitProps);
		for (auto i : SuitProps)
		{
			nSuitProperties.Add(i, *nSuitProperties.Find(i) % *OtherStruct.nSuitProperties.Find(i));
		}
	}

	if (OtherStruct.nDamageTypeResistance.Num() > 0)
	{
		TArray<TEnumAsByte<ENDamageType>> nDamageTypeArray;
		OtherStruct.nDamageTypeResistance.GetKeys(nDamageTypeArray);
		for (int32 i = 0; i < nDamageTypeArray.Num(); i++)
		{
			nDamageTypeResistance.Add(nDamageTypeArray[i],
			                          *nDamageTypeResistance.Find(nDamageTypeArray[i]) % OtherStruct.
			                          nDamageTypeResistance[nDamageTypeArray[i]]);
		}
	}

	for (int32 i = 1; i < ENDamageType::DamageTypeMAX; i = static_cast<ENDamageType>(i + i))
	{
		if (OtherStruct.HasDamageMask(i))
			if (!(HasDamageMask(i)))
				DamageShieldAbsorption += static_cast<ENDamageType>(i);
	}

	for (int32 i = 1; i < ESuitFlag::SuitFlag_END; i = static_cast<ESuitFlag>(i + i))
	{
		if (OtherStruct.HasFlag(i))
			if (!(HasFlag(i)))
				SuitFlags += static_cast<ESuitFlag>(i);
	}

	for (int32 i = 1; i < ENDamageType::DamageTypeMAX; i = static_cast<ENDamageType>(i + i))
	{
		if (OtherStruct.HasRemoveDamageMask(i))
			if (!(HasDamageMask(i)))
				RemoveDamageShieldAbsorption += static_cast<ENDamageType>(i);
	}

	for (int32 i = 1; i < ESuitFlag::SuitFlag_END; i = static_cast<ESuitFlag>(i + i))
	{
		if (OtherStruct.HasRemoveFlag(i))
			if (!(HasFlag(i)))
				RemoveSuitFlags += static_cast<ESuitFlag>(i);
	}


	if (OtherStruct.nDamageResistance.Num() > 0)
	{
		TArray<TSubclassOf<class UFGDamageType>> DamageTypeArray;
		OtherStruct.nDamageResistance.GetKeys(DamageTypeArray);
		for (int32 i = 0; i < DamageTypeArray.Num(); i++)
			if (nDamageResistance.Contains(DamageTypeArray[i]))
				nDamageResistance.Add(DamageTypeArray[i],
				                      *nDamageResistance.Find(DamageTypeArray[i]) % OtherStruct.nDamageResistance[
					                      DamageTypeArray[i]]);
			else
				nDamageResistance.Add(DamageTypeArray[i], OtherStruct.nDamageResistance[DamageTypeArray[i]]);
	}


	InventorySlots = InventorySlots + OtherStruct.InventorySlots;

	return *this;
}

FEquipmentStats FEquipmentStats::operator-(const FEquipmentStats& OtherStruct)
{
	SetupDefaults();

	// Power
	if (OtherStruct.nMovementProperties.Num() > 0)
	{
		TArray<TEnumAsByte<ESuitMovementProperty>> MovementProperties;
		OtherStruct.nMovementProperties.GetKeys(MovementProperties);
		for (auto i : MovementProperties)
		{
			nMovementProperties.Add(i, *nMovementProperties.Find(i) - *OtherStruct.nMovementProperties.Find(i));
		}
	}

	if (OtherStruct.nFlightProperties.Num() > 0)
	{
		TArray<TEnumAsByte<ESuitFlightProperty>> FlightProperties;
		OtherStruct.nFlightProperties.GetKeys(FlightProperties);
		for (auto i : FlightProperties)
		{
			nFlightProperties.Add(i, *nFlightProperties.Find(i) - *OtherStruct.nFlightProperties.Find(i));
		}
	}

	if (OtherStruct.nSuitProperties.Num() > 0)
	{
		TArray<TEnumAsByte<ESuitProperty>> SuitProps;
		OtherStruct.nSuitProperties.GetKeys(SuitProps);
		for (auto i : SuitProps)
		{
			nSuitProperties.Add(i, *nSuitProperties.Find(i) - *OtherStruct.nSuitProperties.Find(i));
		}
	}

	if (OtherStruct.nDamageTypeResistance.Num() > 0)
	{
		TArray<TEnumAsByte<ENDamageType>> arr;
		OtherStruct.nDamageTypeResistance.GetKeys(arr);
		for (int32 i = 0; i < arr.Num(); i++)
		{
			nDamageTypeResistance.Add(
				arr[i], *nDamageTypeResistance.Find(arr[i]) - OtherStruct.nDamageTypeResistance[arr[i]]);
		}
	}

	for (int32 i = 1; i < ENDamageType::DamageTypeMAX; i = static_cast<ENDamageType>(i + i))
	{
		if (OtherStruct.HasDamageMask(i))
			if (!(HasDamageMask(i)))
				DamageShieldAbsorption -= static_cast<ENDamageType>(i);
	}

	for (int32 i = 1; i < ESuitFlag::SuitFlag_END; i = static_cast<ESuitFlag>(i + i))
	{
		if (OtherStruct.HasFlag(i))
			if ((HasFlag(i)))
				SuitFlags -= static_cast<ESuitFlag>(i);
	}

	for (int32 i = 1; i < ESuitFlag::SuitFlag_END; i = static_cast<ESuitFlag>(i + i))
	{
		if (OtherStruct.HasRemoveFlag(i))
			if (!(HasRemoveFlag(i)))
				RemoveSuitFlags -= static_cast<ESuitFlag>(i);
	}

	if (OtherStruct.nDamageResistance.Num() > 0)
	{
		TArray<TSubclassOf<class UFGDamageType>> Arr;
		OtherStruct.nDamageResistance.GetKeys(Arr);
		for (auto i : Arr)
			if (nDamageResistance.Contains(i))
				nDamageResistance.Add(i, *nDamageResistance.Find(i) - OtherStruct.nDamageResistance[i]);
	}
	
	InventorySlots = InventorySlots - OtherStruct.InventorySlots;


	return *this;
}

bool FEquipmentStats::HasFlag(int32 Bitmask) const
{
	return (Bitmask & SuitFlags) == Bitmask;
}

bool FEquipmentStats::HasDamageMask(int32 Bitmask) const
{
	return (Bitmask & DamageShieldAbsorption) == Bitmask;
}

bool FEquipmentStats::HasRemoveFlag(int32 Bitmask) const
{
	return (Bitmask & RemoveSuitFlags) == Bitmask;
}

bool FEquipmentStats::HasRemoveDamageMask(int32 Bitmask) const
{
	return (Bitmask & RemoveDamageShieldAbsorption) == Bitmask;
}
