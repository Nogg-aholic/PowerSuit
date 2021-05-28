#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../EquipmentModuleComponent.h"
#include "EMC_SubModule.generated.h"

/**
 * 
 */
UCLASS()
class POWERSUIT_API UEMC_SubModule : public UObject
{
	GENERATED_BODY()

public:

	
	UPROPERTY()
	UEquipmentModuleComponent* Parent;

};
