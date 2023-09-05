#include "Stats/PowerSuitData.h"
#include "SubModules/EMC_FuelModule.h"
#include "Equipment/Equipment_PowerSuit.h"
#include "Attachment/PowerSuitModuleAttachment.h"
#include "SubModules/EMC_InventoryModule.h"

FEquipmentStats::FEquipmentStats(): SuitFlagsAdvanced(0), RemoveSuitFlagsAdvanced(0), mCachedAttachment(nullptr)
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

	Parent->DefaultStats.ForgetUnlockedFuels(Parent, true);

	
	// Add fuels that are granted by modules

	for (auto i : FuelsToUnlock)
	{
		if (!Parent->FuelModule->nAllowedFuels.Contains(i))
		{
			Parent->FuelModule->nAllowedFuels.Add(i);
			nUnlockedAllowedFuels.Add(i);
		}
		else
		{
			nSkippedAllowedFuels.Add(i);
		}
	}

	TArray<FItemAmount> SuitCostToUse; 
#ifdef FOR_MODSHIPPING
	// SuitCostToUse = Parent->EquipmentParent->mCostToUse;
	// TODOU5 - I had to switch this to an Accessor access transformer, and I'm not sure I did it right...
	SuitCostToUse = Parent->EquipmentParent->GetCostToUse();
#else
	SuitCostToUse = Parent->EquipmentParent->*get(steal_mCostToUse());
#endif

	// Add fuels from suit Cost to Use, ignoring specified counts (notably, this is done after module fuels. This means module fuels have higher priority than suit fuels.)
	for (FItemAmount e : SuitCostToUse)
	{
		if (!Parent->FuelModule->nAllowedFuels.Contains(e.ItemClass))
		{
			Parent->FuelModule->nAllowedFuels.Add(e.ItemClass);
			Parent->DefaultStats.nUnlockedAllowedFuels.Add(e.ItemClass);
		}
	}
};

void FEquipmentStats::ForgetUnlockedFuels(UEquipmentModuleComponent* Parent,bool IsDefault)
{
	for (auto e : nUnlockedAllowedFuels) {
		if (Parent->FuelModule->nAllowedFuels.Contains(e))
		{
			Parent->FuelModule->nAllowedFuels.Remove(e);
		}
	}
	nUnlockedAllowedFuels.Empty();

	for(auto & RememberedItem : Parent->InventoryModule->ItemsRemembered)
	{
		TArray<TSubclassOf<class UFGItemDescriptor>> SkippedAllowedFuels = RememberedItem.Value.nSkippedAllowedFuels;
		for(auto FuelClass: SkippedAllowedFuels)
		{
			if (!Parent->FuelModule->nAllowedFuels.Contains(FuelClass))
			{
				Parent->FuelModule->nAllowedFuels.Add(FuelClass);
				RememberedItem.Value.nUnlockedAllowedFuels.Add(FuelClass);
				RememberedItem.Value.nSkippedAllowedFuels.Remove(FuelClass);
			}
		}
	}

	TArray<FItemAmount> SuitCostToUse;
#ifdef FOR_MODSHIPPING
	// SuitCostToUse = Parent->EquipmentParent->mCostToUse;
	// TODOU5 - I had to switch this to an Accessor access transformer, and I'm not sure I did it right...
	SuitCostToUse = Parent->EquipmentParent->GetCostToUse();
#else
	SuitCostToUse = Parent->EquipmentParent->*get(steal_mCostToUse());
#endif

	if (!IsDefault)
	{
		// Add fuels from suit Cost to Use, ignoring specified counts (notably, this is done after module fuels. This means module fuels have higher priority than suit fuels.)
		for (FItemAmount e : SuitCostToUse)
		{
			if (!Parent->FuelModule->nAllowedFuels.Contains(e.ItemClass))
			{
				Parent->FuelModule->nAllowedFuels.Add(e.ItemClass);
				Parent->DefaultStats.nUnlockedAllowedFuels.Add(e.ItemClass);
			}
		}
	}
	
}

FEquipmentStats FEquipmentStats::operator+(const FEquipmentStats& OtherStruct)
{
	SetupDefaults();

	if (OtherStruct.nMovementProperties.Num() > 0)
	{
		for(auto i : OtherStruct.nMovementProperties)
		{
			if(nMovementProperties.Contains(i.Key))
				nMovementProperties.Add(i.Key, *nMovementProperties.Find(i.Key) % *OtherStruct.nMovementProperties.Find(i.Key));
			else
				nMovementProperties.Add(i.Key,*OtherStruct.nMovementProperties.Find(i.Key));
		}
	}

	if (OtherStruct.nFlightProperties.Num() > 0)
	{
		for(auto i : OtherStruct.nFlightProperties)
		{
			if(nFlightProperties.Contains(i.Key))
				nFlightProperties.Add(i.Key, *nFlightProperties.Find(i.Key) % *OtherStruct.nFlightProperties.Find(i.Key));
			else
				nFlightProperties.Add(i.Key,*OtherStruct.nFlightProperties.Find(i.Key));	
		}
	}
	if (OtherStruct.nSuitProperties.Num() > 0)
	{
		for(auto i : OtherStruct.nSuitProperties)
		{
			if(nSuitProperties.Contains(i.Key))
				nSuitProperties.Add(i.Key, *nSuitProperties.Find(i.Key) % *OtherStruct.nSuitProperties.Find(i.Key));
			else
				nSuitProperties.Add(i.Key,*OtherStruct.nSuitProperties.Find(i.Key));	
		}
	}

	if (OtherStruct.nDamageTypeResistance.Num() > 0)
	{
		for(auto i : OtherStruct.nDamageTypeResistance)
		{
			if(nDamageTypeResistance.Contains(i.Key))
				nDamageTypeResistance.Add(i.Key, *nDamageTypeResistance.Find(i.Key) % *OtherStruct.nDamageTypeResistance.Find(i.Key));
			else
				nDamageTypeResistance.Add(i.Key,*OtherStruct.nDamageTypeResistance.Find(i.Key));	
		}
	}

	if (OtherStruct.nNamedProperties.Num() > 0)
	{
		for(auto i : OtherStruct.nNamedProperties)
		{
			if(nNamedProperties.Contains(i.Key))
				nNamedProperties.Add(i.Key, *nNamedProperties.Find(i.Key) % *OtherStruct.nNamedProperties.Find(i.Key));
			else
				nNamedProperties.Add(i.Key,*OtherStruct.nNamedProperties.Find(i.Key));	
		}
	}

	for (int32 i = 1; i < ENDamageType::DamageTypeMAX; i = static_cast<ENDamageType>(i + i))
	{
		if (OtherStruct.HasDamageMask(i))
			if (!(HasDamageMask(i)))
				DamageShieldAbsorption += static_cast<ENDamageType>(i);
	}

	for (int32 i = 1; i < ENDamageType::DamageTypeMAX; i = static_cast<ENDamageType>(i + i))
	{
		if (OtherStruct.HasRemoveDamageMask(i))
			if (!(HasRemoveDamageMask(i)))
				RemoveDamageShieldAbsorption += static_cast<ENDamageType>(i);
	}

	for (int32 i = 1; i < ESuitFlag::SuitFlag_END; i = static_cast<ESuitFlag>(i + i))
	{
		if (OtherStruct.HasFlag(i))
			if (!(HasFlag(i)))
				SuitFlags += static_cast<ESuitFlag>(i);
	}

	for (int32 i = 1; i < ESuitFlag::SuitFlag_END; i = static_cast<ESuitFlag>(i + i))
	{
		if (OtherStruct.HasRemoveFlag(i))
			if (!(HasRemoveFlag(i)))
				RemoveSuitFlags += static_cast<ESuitFlag>(i);
	}

	for (int32 i = 1; i < ESuitFlagAdvanced::SuitFlagAdvanced_END; i = static_cast<ESuitFlagAdvanced>(i + i))
	{
		if (OtherStruct.HasAdvancedFlag(i))
			if (!(HasAdvancedFlag(i)))
				SuitFlagsAdvanced += static_cast<ESuitFlagAdvanced>(i);
	}


	for (int32 i = 1; i < ESuitFlagAdvanced::SuitFlagAdvanced_END; i = static_cast<ESuitFlagAdvanced>(i + i))
	{
		if (OtherStruct.HasAdvancedRemoveFlag(i))
			if (!(HasAdvancedRemoveFlag(i)))
				RemoveSuitFlagsAdvanced += static_cast<ESuitFlagAdvanced>(i);
	}


	if (OtherStruct.nDamageResistance.Num() > 0)
	{
		TArray<TSubclassOf<class UFGDamageType>> DamageTypeArray;
		OtherStruct.nDamageResistance.GetKeys(DamageTypeArray);
		for (auto i : DamageTypeArray)
			if (nDamageResistance.Contains(i))
				nDamageResistance.Add(i,  *nDamageResistance.Find(i) % OtherStruct.nDamageResistance[i]);
			else
				nDamageResistance.Add(i, OtherStruct.nDamageResistance[i]);
	}


	InventorySlots = InventorySlots + OtherStruct.InventorySlots;
	
	return *this;
}

FEquipmentStats FEquipmentStats::operator-(const FEquipmentStats& OtherStruct)
{
	SetupDefaults();
	
	if (OtherStruct.nMovementProperties.Num() > 0)
	{
		for(auto i : OtherStruct.nMovementProperties)
		{
			if(nMovementProperties.Contains(i.Key))
				nMovementProperties.Add(i.Key, *nMovementProperties.Find(i.Key) - *OtherStruct.nMovementProperties.Find(i.Key));
			else
				nMovementProperties.Add(i.Key,FModMultProperty() - *OtherStruct.nMovementProperties.Find(i.Key));	
		}
	}

	if (OtherStruct.nFlightProperties.Num() > 0)
	{
		for(auto i : OtherStruct.nFlightProperties)
		{
			if(nFlightProperties.Contains(i.Key))
				nFlightProperties.Add(i.Key, *nFlightProperties.Find(i.Key) - *OtherStruct.nFlightProperties.Find(i.Key));
			else
				nFlightProperties.Add(i.Key,FModMultProperty() - *OtherStruct.nFlightProperties.Find(i.Key));	
		}
	}

	if (OtherStruct.nSuitProperties.Num() > 0)
	{
		for(auto i : OtherStruct.nSuitProperties)
		{
			if(nSuitProperties.Contains(i.Key))
				nSuitProperties.Add(i.Key, *nSuitProperties.Find(i.Key) - *OtherStruct.nSuitProperties.Find(i.Key));
			else
				nSuitProperties.Add(i.Key,FModMultProperty() - *OtherStruct.nSuitProperties.Find(i.Key));	
		}
	}
	if (OtherStruct.nDamageTypeResistance.Num() > 0)
	{
		for(auto i : OtherStruct.nDamageTypeResistance)
		{
			if(nDamageTypeResistance.Contains(i.Key))
				nDamageTypeResistance.Add(i.Key, *nDamageTypeResistance.Find(i.Key) - *OtherStruct.nDamageTypeResistance.Find(i.Key));
			else
				nDamageTypeResistance.Add(i.Key,FModMultProperty() - *OtherStruct.nDamageTypeResistance.Find(i.Key));	
		}
	}

	if (OtherStruct.nNamedProperties.Num() > 0)
	{
		for(auto i : OtherStruct.nNamedProperties)
		{
			if(nNamedProperties.Contains(i.Key))
				nNamedProperties.Add(i.Key, *nNamedProperties.Find(i.Key) - *OtherStruct.nNamedProperties.Find(i.Key));
			else
				nNamedProperties.Add(i.Key,FModMultProperty() - *OtherStruct.nNamedProperties.Find(i.Key));	
		}
	}

	for (int32 i = 1; i < ENDamageType::DamageTypeMAX; i = static_cast<ENDamageType>(i + i))
	{
		if (OtherStruct.HasDamageMask(i))
			if (!(HasDamageMask(i)))
				DamageShieldAbsorption -= static_cast<ENDamageType>(i);
	}

	for (int32 i = 1; i < ENDamageType::DamageTypeMAX; i = static_cast<ENDamageType>(i + i))
	{
		if (OtherStruct.HasRemoveDamageMask(i))
			if (!(HasRemoveDamageMask(i)))
				RemoveDamageShieldAbsorption -= static_cast<ENDamageType>(i);
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


	for (int32 i = 1; i < ESuitFlagAdvanced::SuitFlagAdvanced_END; i = static_cast<ESuitFlagAdvanced>(i + i))
	{
		if (OtherStruct.HasAdvancedFlag(i))
			if ((HasAdvancedFlag(i)))
				SuitFlagsAdvanced -= static_cast<ESuitFlagAdvanced>(i);
	}

	for (int32 i = 1; i < ESuitFlagAdvanced::SuitFlagAdvanced_END; i = static_cast<ESuitFlagAdvanced>(i + i))
	{
		if (OtherStruct.HasAdvancedRemoveFlag(i))
			if (!(HasAdvancedRemoveFlag(i)))
				RemoveSuitFlagsAdvanced -= static_cast<ESuitFlagAdvanced>(i);
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

bool FEquipmentStats::HasFlag(const int32 Bitmask) const
{
	return (Bitmask & SuitFlags) == Bitmask;
}
bool FEquipmentStats::HasAdvancedFlag(const int32 Bitmask) const
{
	return (Bitmask & SuitFlagsAdvanced) == Bitmask;
}


bool FEquipmentStats::HasDamageMask(const int32 Bitmask) const
{
	return (Bitmask & DamageShieldAbsorption) == Bitmask;
}

bool FEquipmentStats::HasRemoveFlag(const int32 Bitmask) const
{
	return (Bitmask & RemoveSuitFlags) == Bitmask;
}


bool FEquipmentStats::HasAdvancedRemoveFlag(const int32 Bitmask) const
{
	return (Bitmask & RemoveSuitFlagsAdvanced) == Bitmask;
}

bool FEquipmentStats::HasRemoveDamageMask(const int32 Bitmask) const
{
	return (Bitmask & RemoveDamageShieldAbsorption) == Bitmask;
}
