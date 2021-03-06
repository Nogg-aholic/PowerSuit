

#include "Equipment/EquipmentModuleDescriptor.h"

UEquipmentModuleDescriptor::UEquipmentModuleDescriptor()
{
}

bool UEquipmentModuleDescriptor::IsAllowed(TSubclassOf<UEquipmentModuleDescriptor> InClass, AFGEquipment* Equipment)
{
	if (InClass && Equipment) return InClass.GetDefaultObject()->nAllowedUsage.Contains(Equipment->GetClass());
	else return false;
}

bool UEquipmentModuleDescriptor::GetnUniqueUsage(TSubclassOf<UEquipmentModuleDescriptor> InClass)
{
	if (InClass) return InClass.GetDefaultObject()->nUniqueUsage;
	else return bool();
}

TSubclassOf<APowerSuitModuleAttachment> UEquipmentModuleDescriptor::GetnAttachment(
	TSubclassOf<UEquipmentModuleDescriptor> InClass)
{
	if (InClass)
	{
		const TSubclassOf<class APowerSuitModuleAttachment> C = InClass.GetDefaultObject()->nModuleAttachment;
		return C;
	}
	else return nullptr;
}

TArray<TSubclassOf<AFGEquipment>> UEquipmentModuleDescriptor::GetnAllowedUsage(
	TSubclassOf<UEquipmentModuleDescriptor> InClass)
{
	if (InClass) return InClass.GetDefaultObject()->nAllowedUsage;
	else return TArray<TSubclassOf<class AFGEquipment>>();
}

FEquipmentStats UEquipmentModuleDescriptor::GetEquipmentStats(TSubclassOf<UEquipmentModuleDescriptor> InClass)
{
	if (InClass) return InClass.GetDefaultObject()->EquipmentStats;
	else return FEquipmentStats();
}

bool UEquipmentModuleDescriptor::IsAllowedByUnique(TSubclassOf<UEquipmentModuleDescriptor> InClass, TArray<TSubclassOf<class UEquipmentModuleDescriptor>> Uniques)
{
	if (InClass)
	{
		auto* Obj = InClass.GetDefaultObject();
		if (Obj->nUniqueUsage)
		{
			if (Uniques.Contains(InClass))
				return false;
			for (TSubclassOf<class UEquipmentModuleDescriptor> i : Obj->nUniqueCounted)
			{
				if (Uniques.Contains(i))
				{
					return false;
				}
			}
		}
	}


	return true;
}
