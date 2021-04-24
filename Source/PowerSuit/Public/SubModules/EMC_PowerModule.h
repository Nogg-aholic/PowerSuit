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
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
	float GetPowerCapacity();

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
	float GetFuseTimerDuration();
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetOverDrawDuration();
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		bool IsFuseIntact();

	// Power Draw in MW including External
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetPowerDraw();

	UEquipmentModuleComponent* Parent;

};
