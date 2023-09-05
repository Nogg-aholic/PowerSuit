#pragma once
#include "PowerSuit.h"

#include "SubModules/EMC_SubModule.h"
#include "EMC_ZiplineModule.generated.h"

/**
 * 
 */
UCLASS()
class POWERSUIT_API UEMC_ZiplineModule : public UEMC_SubModule
{
	GENERATED_BODY()
private:
	friend class UEquipmentModuleComponent;


	void PreTick();
	void Tick();
	void PostTick();
};
