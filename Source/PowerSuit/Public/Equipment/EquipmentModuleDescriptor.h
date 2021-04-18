

#pragma once

#include "CoreMinimal.h"
#include "Resources/FGEquipmentDescriptor.h"
#include "Stats/PowerSuitData.h"
#include "EquipmentModuleDescriptor.generated.h"

class APowerSuitModuleAttachment;

UCLASS()
class POWERSUIT_API UEquipmentModuleDescriptor : public UFGItemDescriptor
{
	GENERATED_BODY()


protected:
	/** Internal function to get the display description. */
	virtual FText GetItemDescriptionInternal() const override;

public:
	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static bool IsAllowed(TSubclassOf< UEquipmentModuleDescriptor > inClass, AFGEquipment * equipment) { if (inClass && equipment) return inClass.GetDefaultObject()->nAllowedUsage.Contains(equipment->GetClass()); else return false; }

	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static bool GetnUniqueUsage(TSubclassOf< UEquipmentModuleDescriptor > inClass) { if (inClass) return inClass.GetDefaultObject()->nUniqueUsage; else return bool(); }
	
	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static TSubclassOf<class APowerSuitModuleAttachment> GetnAttachment(TSubclassOf< UEquipmentModuleDescriptor > inClass) { if (inClass) {
		TSubclassOf< class APowerSuitModuleAttachment > C = inClass.GetDefaultObject()->nModuleAttachment;
	return C;
	}
	else return nullptr; }
	
	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static TArray<TSubclassOf<class AFGEquipment>> GetnAllowedUsage(TSubclassOf< UEquipmentModuleDescriptor > inClass) { if (inClass) return inClass.GetDefaultObject()->nAllowedUsage; else return TArray<TSubclassOf<class AFGEquipment>>(); }
	
	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static FEquipmentStats GetEquipmentStats(TSubclassOf< UEquipmentModuleDescriptor > inClass) { if (inClass) return inClass.GetDefaultObject()->EquipmentStats; else return  FEquipmentStats(); }

	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		FEquipmentStats EquipmentStats UMETA(Displayname = "Equipment Stats - Hover over things and read tooltip for descriptions!", Tooltip = "Module properties");


	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		TSubclassOf<class APowerSuitModuleAttachment> nModuleAttachment;

	/**
	* Enables Usage of this Fuel Type. Must Have MJ Value
	*/
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		TArray<TSubclassOf<class UFGItemDescriptor>> nAllowedFuels;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		bool nUniqueUsage UMETA(Tooltip = "If checked, installing more than 1 of this module has no additional effects");


	/**
	* If this is Unique Usage , are there other UEquipmentModuleDescriptor we should count as Equals?
	*/
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		TArray<TSubclassOf<class UEquipmentModuleDescriptor>> nUniqueCounted;

	/**
	* Restrict usage to only certain Suits. Default is allowed in all suits.
	* To reference other mods' suits, put in empty stub blueprints in their intended location. The game will resolve it at runtime.
	*/

	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		TArray<TSubclassOf<class AFGEquipment>> nAllowedUsage;




};
