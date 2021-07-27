#pragma once

#include "SubModules/EMC_SubModule.h"
#include "Attachment/PowerSuitModuleAttachment.h"
#include "EMC_InventoryModule.generated.h"


USTRUCT()
struct  POWERSUIT_API  FEquipmentStatStack
{
	GENERATED_BODY()
public:
	FEquipmentStatStack() { Stats = {}; };


	TArray<FEquipmentStats> Stats;

};


/**
 * 
 */
UCLASS()
class POWERSUIT_API UEMC_InventoryModule : public UEMC_SubModule
{
	GENERATED_BODY()



private:

	void PreTick();
	void Tick();
	void PostTick();

	void SubtractModuleStats(TSubclassOf<class UEquipmentModuleDescriptor> Item, int32 Index);

	bool CheckCreateModuleStats(const FInventoryStack Stack, const int32 Ind);

	friend class UEquipmentModuleComponent;
	friend class UEMC_PowerModule;
	friend class UEMC_AttachmentModule;
	UFUNCTION()
	AFGCharacterPlayer* InitInventory();

	void UpdateInventorySize();

	void ResetInventoryStats();

	void BulkUpdateStats(UFGInventoryComponent * Inventory);

	APowerSuitModuleAttachment* CreateAttachmentStateIfNeeded(FInventoryStack Stack, const UEquipmentModuleDescriptor* ItemObj, const TSubclassOf<class UEquipmentModuleDescriptor> Item, const int32 Index);

	bool MergeOnIndex(int32 Ind, bool Safe = true);

	void MergeStats(FInventoryStack Stack, FEquipmentStats& StatsRef);

	FEquipmentStats& GetModuleStats(FInventoryStack Stack, int32 Ind);

public:

	UFUNCTION(BlueprintCallable)
		bool UpdateOnIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void RefreshInventory();
	
	UFUNCTION()
		void RefreshInventoryAdd(TSubclassOf<UFGItemDescriptor> ItemClass, int32 NumAdded);
	
	UFUNCTION()
		void RefreshInventoryRemove(TSubclassOf<UFGItemDescriptor> ItemClass, int32 NumAdded);
	

	// GC should stay away
	UPROPERTY() 
	TMap< int32, FEquipmentStats> ItemsRemembered;
	UPROPERTY()
	TArray<TSubclassOf<class UEquipmentModuleDescriptor>> UniquesActive;
};
