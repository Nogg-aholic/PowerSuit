#include "Stats/PowerSuitData.h"
#include "SubModules/EMC_FuelModule.h"
#include "Attachment/PowerSuitModuleAttachment.h"

FEquipmentStats::FEquipmentStats()
{
	nMovementProperties = {};
	nFlightProperties = {};
	nSuitProperties = {};
	nDamageTypeResistance = {};
	nDamageResistance = {};
	DamageShieldAbsorbtion = 0;
	RemoveDamageShieldAbsorbtion = 0;
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

FText UEquipmentModuleDescriptor::GetItemDescriptionInternal() const {
	FText sText = mDescription;
	return sText;
}

void FEquipmentStats::SetupDefaults()
{

	if (nFlightProperties.Num() != int32(ESuitFlightProperty::EFP_MAX))
	{
		for (int32 i = 0; i < ESuitFlightProperty::EFP_MAX; ++i)
			if (!nFlightProperties.Contains(ESuitFlightProperty(i)))
				nFlightProperties.Add(ESuitFlightProperty(i), FModMultProperty());
	}

	if (nMovementProperties.Num() != int32(ESuitMovementProperty::ESMC_MAX))
	{
		for (int32 i = 0; i < ESuitMovementProperty::ESMC_MAX; ++i)
			if (!nMovementProperties.Contains(ESuitMovementProperty(i)))
				nMovementProperties.Add(ESuitMovementProperty(i), FModMultProperty());
	}

	if (nSuitProperties.Num() != int32(ESuitProperty::SuitPropertyMAX))
	{
		for (int32 i = 0; i < ESuitProperty::SuitPropertyMAX; ++i)
			if (!nSuitProperties.Contains(ESuitProperty(i)))
				nSuitProperties.Add(ESuitProperty(i), FModMultProperty());
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