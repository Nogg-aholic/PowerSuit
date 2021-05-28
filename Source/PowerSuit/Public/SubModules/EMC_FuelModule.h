#pragma once
#include "SubModules/EMC_SubModule.h"
#include "EMC_FuelModule.generated.h"

/**
 * 
 */
UCLASS()
class POWERSUIT_API UEMC_FuelModule : public UEMC_SubModule
{
	GENERATED_BODY()
public:

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:

	friend class UEquipmentModuleComponent;
	void PreTick();
	void Tick() const;
	void PostTick() const;

	void TryReload();
	bool ConsumeFuelItem(UFGInventoryComponent* Inventory, TSubclassOf<class UFGItemDescriptor> inClass, int32 inAmount) const;

public:



	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule")
		TArray<TSubclassOf<class UFGItemDescriptor>> nAllowedFuels;


};
