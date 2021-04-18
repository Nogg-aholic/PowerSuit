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

	void SubtractModuleStats(TSubclassOf<class UEquipmentModuleDescriptor> item, int32 Index);

	friend class UEquipmentModuleComponent;
	friend class UEMC_PowerModule;
	friend class UEMC_AttachmentModule;

	AFGCharacterPlayer* InitInventory();

	void UpdateInentorySize();

	void ResetInventoryStats();

	void BulkUpdateStats(UFGInventoryComponent * Inventory);

	void CreateAttachmentStateIfNeeded(FInventoryStack Stack, const UEquipmentModuleDescriptor* ItemObj, const TSubclassOf<class UEquipmentModuleDescriptor> item, const int32 index);

	bool MergeOnIndex(int32 ind, bool Safe = true);

	void MergeStats(FInventoryStack Stack, FEquipmentStats& StatsRef);

	FEquipmentStats GetModuleStats(FInventoryStack Stack, int32 ind);

public:

	UFUNCTION(BlueprintCallable)
		bool UpdateOnIndex(int32 index);

	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void RefreshInventory();
	UFUNCTION()
		void RefreshInventoryAdd(TSubclassOf<UFGItemDescriptor> itemClass, int32 numAdded);
	UFUNCTION()
		void RefreshInventoryRemove(TSubclassOf<UFGItemDescriptor> itemClass, int32 numAdded);

	// GC should stay away
	UPROPERTY() 
	TMap< TSubclassOf<class UFGItemDescriptor>, FEquipmentStatStack> ItemsRemembered;
	UPROPERTY()
	TArray<TSubclassOf<class UEquipmentModuleDescriptor>> UniquesActive;
};
