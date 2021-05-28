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
	void Tick() const;
	void PostTick() const;

	void UpdateProductionState() const;


	void RegenPower() const;

	
	void TryRestart() const;

	void BreakFuse();

public:
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
	float GetPowerCapacity() const;

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
	float GetFuseTimerDuration() const;
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetOverDrawDuration() const;
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		bool IsFuseIntact() const;

	// Power Draw in MW including External
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetPowerDraw() const;

};
