

#pragma once

#include "CoreMinimal.h"
#include "Resources/FGEquipmentDescriptor.h"
#include "Stats/PowerSuitData.h"
#include "EquipmentModuleDescriptor.generated.h"

class APowerSuitModuleAttachment;
class UPowerSuitWidget;
UCLASS()
class POWERSUIT_API UEquipmentModuleDescriptor : public UFGItemDescriptor
{
	GENERATED_BODY()


	UEquipmentModuleDescriptor();
public:
	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static bool IsAllowed(TSubclassOf<UEquipmentModuleDescriptor> InClass, AFGEquipment* Equipment);

	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static bool GetnUniqueUsage(TSubclassOf<UEquipmentModuleDescriptor> InClass);

	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static TSubclassOf<class APowerSuitModuleAttachment> GetnAttachment(TSubclassOf<UEquipmentModuleDescriptor> InClass);

	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static TArray<TSubclassOf<class AFGEquipment>> GetnAllowedUsage(TSubclassOf<UEquipmentModuleDescriptor> InClass);

	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static FEquipmentStats GetEquipmentStats(TSubclassOf<UEquipmentModuleDescriptor> InClass);

	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static bool IsAllowedByUnique(TSubclassOf<UEquipmentModuleDescriptor> InClass,TArray< TSubclassOf<class UEquipmentModuleDescriptor>> Uniques);

	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule", meta = (Displayname = "Equipment Stats - Hover over things and read tooltip for descriptions!", Tooltip = "Module properties"))
		FEquipmentStats EquipmentStats;


	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		TSubclassOf<class APowerSuitModuleAttachment> nModuleAttachment;

	/**
	* Enables Usage of this Fuel Type. Must Have MJ Value
	*/
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		TArray<TSubclassOf<class UFGItemDescriptor>> nAllowedFuels;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule", meta = (Tooltip = "If checked, installing more than 1 of this module has no additional effects"))
		bool nUniqueUsage;

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


	// Added to Suit Overlay
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Tooltip = "Overlay Module Widget, Added above the Suit's Overlay Widget in the HUD ( permanently in view )"))
		TSubclassOf<UPowerSuitWidget> OMW;


	// Added to Inventory UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Tooltip = "Inventory Module Widget, Added to the Character Inventory Widget"))
		TSubclassOf<UPowerSuitWidget> IMW;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Tooltip = "Sound for the suit to play when equipped, this is implemented in the Suit"))
		USoundBase * EquipSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Tooltip = "Sound for the suit to play when unequipped, this is implemented in the Suit"))
		USoundBase* UnEquipSound;
};
