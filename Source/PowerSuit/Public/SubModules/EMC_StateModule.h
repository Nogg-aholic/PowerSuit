#pragma once

#include "SubModules/EMC_SubModule.h"
#include "Equipment/Equipment_PowerSuit.h"

#include "EMC_StateModule.generated.h"

/**
 * 
 */
UCLASS()
class POWERSUIT_API UEMC_StateModule : public UEMC_SubModule
{
	GENERATED_BODY()


	friend class UEquipmentModuleComponent;

	void PreTick();
	void Tick();
	void PostTick();
public:

	void ReplicateStates();

	void CheckHotkeys();

	void UpdateSuitState();


	void HoverModeChange();


	UFUNCTION(BlueprintCallable)
		void UpdateHotkeys();

	// State Bools

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Toggle;
	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool HKey_Up;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Up;
	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool HKey_Down;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Down;
	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool HKey_Accel;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Accel;
	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool HKey_Breaks;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Breaks;


	/** Keeps is the player sprinting this update or not? */
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		bool nIsSprinting;
	/** Keeps is the player sprinting this update or not? */
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		bool nIsSliding;


	DECLARE_DELEGATE_OneParam(FPowerSuitFrictionToggle, FKey);
	FPowerSuitFrictionToggle OnPowerSuitFrictionToggle;

	DECLARE_DELEGATE_OneParam(FPowerSuitUIToggle, FKey);
	FPowerSuitUIToggle OnPowerSuitUIToggle;


};
