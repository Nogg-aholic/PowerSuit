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
	void Tick();
	void PostTick();

	void TryReload();
	bool ConsumeFuelItem(UFGInventoryComponent* Inventory, TSubclassOf<class UFGItemDescriptor> inClass, int32 inAmount);

public:

	//	External Fuel Draw in MWs	( Power overdraw is negativly reducing ChargeDuration on which we base the amount to remove per frame)
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float DeltaFuelConsumption;

	/** Energy value for this Buffer In megawatt seconds (MWs), a.k.a. mega joule (MJ) */
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Equipment")
		float FuelAmountBuffer = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule")
		TArray<TSubclassOf<class UFGItemDescriptor>> nAllowedFuels;


};
