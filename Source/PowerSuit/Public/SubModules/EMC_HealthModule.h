#pragma once
#include "PowerSuit.h"

#include "SubModules/EMC_SubModule.h"
#include "FGHealthComponent.h"
#include "EMC_HealthModule.generated.h"

UCLASS()
class POWERSUIT_API UEMC_HealthModule : public UEMC_SubModule
{
	GENERATED_BODY()



	friend class UEquipmentModuleComponent;
	void PreTick();
	void Tick();
	void PostTick();


public:

	
	void SetMaxHealth() const;

	float HealthBuffer;

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetNewMaxHealth() const;

};