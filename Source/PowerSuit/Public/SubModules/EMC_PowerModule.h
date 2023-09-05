#pragma once
#include "PowerSuit.h"

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
	void PostTick();

	bool UpdateProductionState() const;

	void RegenPower();

	void TryRestart() const;

	void TryBreakFuse();

	bool SentOverdrawStartEvent;

public:
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetPowerCapacity() const;

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetFuseTimerDuration() const;

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetOverDrawDuration() const;

	// How long has it been since the fuse break event (not the overdraw, which started before the fuse break)
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		FTimespan TimeSinceFuseBreak() const;

	// How long has it been since the suit last had power (since overdraw started)
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		FTimespan TimeSinceOverdrawStart() const;

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		bool IsFuseIntact() const;

	// Forcefully break the suit fuse regardless of power levels and overdraw time, triggering a restart
	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void ForcefullyBreakFuse(bool drainPower);

	// How many seconds until the restart happens? If negative, restart has already taken place
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float RemainingSecondsUntilFuseRestart() const;

	// Power Draw in MW including External
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetPowerDraw() const;

	void CacheFuseTimerDuration();

	float CachedFuseTime = 0.f;

};
