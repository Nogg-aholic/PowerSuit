#pragma once
#include "SubModules/EMC_SubModule.h"
#include "EMC_PowerModule.generated.h"

/**
 * 
 */
UCLASS()
class POWERSUIT_API UEMC_PowerModule : public UEMC_SubModule
{
	GENERATED_BODY()


	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;


private:
	friend class UEquipmentModuleComponent;


	void PreTick();
	void Tick();
	void PostTick();

	void UpdateProductionState();


	void RegenPower();

	
	void TryRestart();

	void BreakFuse();

public:

	void GoActive();

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetActiveTimerDuration();
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetFuseTimerDuration();
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetOverDrawDuration();
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		bool IsFuseIntact();

	// Power Draw in MW including External
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetPowerDraw();

	float CurrentPower;

	//  External Power Draw in MWs
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float PowerConsumption;

	// Are we producing power ? -> !IsFuseTriggered"
	UPROPERTY(BlueprintReadonly, Replicated, Category = "EquipmentModule")
		bool Producing;
	// Are we consuming extra power?"
	UPROPERTY(BlueprintReadonly, Replicated, Category = "EquipmentModule")
		bool Active;

	// Timer for when Active was Triggered"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
		FDateTime ActiveTimer;
	// Timer for when Fuse was Triggered"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
		FDateTime FuseBreak;
	// Timer for when Fuse was Triggered"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
		FDateTime FuseBreakOverDraw;


	UEquipmentModuleComponent* Parent;

};
