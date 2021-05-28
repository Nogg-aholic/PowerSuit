

#pragma once
#include "SubModules/EMC_SubModule.h"
#include "EMC_HyperTubeModule.generated.h"

/**
 * 
 */
UCLASS()
class POWERSUIT_API UEMC_HyperTubeModule : public UEMC_SubModule
{
	GENERATED_BODY()



	friend class UEquipmentModuleComponent;
	void PreTick();
	void Tick();
	void PostTick();



public:
	void ModifyPipeSpeed();

	void HandlePipeVelocity();


	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	float nPipeVelocity;

	float LastPipeVelocity = 0.f;

};
