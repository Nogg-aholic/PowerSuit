#pragma once
#include "PowerSuit.h"

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
	void PostTick();

	void TryReload();
	bool ConsumeFuelItem(UFGInventoryComponent* Inventory, TSubclassOf<class UFGItemDescriptor> inClass, int32 inAmount) const;
	void TryRestart() const;

	bool FuelFuseRecoveryPeriodElapsed() const;

public:

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetFuelFuseTimerDuration() const;

	// How long has it been since the fuel fuse break event
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		FTimespan TimeSinceFuelFuseBreak() const;

	// Forcefully break the suit fuse regardless of fuel levels, pass True to also drain the tank
	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void ForcefullyBreakFuelFuse(bool drainFuel);

	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule")
		TArray<TSubclassOf<class UFGItemDescriptor>> nAllowedFuels;
};
