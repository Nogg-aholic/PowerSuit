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


	// State Bools


	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool HKey_Up = false;

	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool HKey_Down = false;

	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool HKey_Accel = false;

	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool HKey_Breaks = false;



	/** Keeps is the player sprinting this update or not? */
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		bool nIsSprinting = false;
	/** Keeps is the player sprinting this update or not? */
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		bool nIsSliding = false;
};
