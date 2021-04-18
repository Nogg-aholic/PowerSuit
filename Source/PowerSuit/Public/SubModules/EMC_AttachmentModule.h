#pragma once

#include "SubModules/EMC_SubModule.h"
#include "EMC_AttachmentModule.generated.h"

/**
 * 
 */
UCLASS()
class POWERSUIT_API UEMC_AttachmentModule : public UEMC_SubModule
{
	GENERATED_BODY()

	friend class UEquipmentModuleComponent;
	void PreTick();
	void Tick();
	void PostTick();

	void ResetAttachments();

public:
	UPROPERTY(BlueprintReadOnly)
		TArray<APowerSuitModuleAttachment *> Attachments;
	UPROPERTY(BlueprintReadOnly)
		TArray<APowerSuitModuleAttachment *> InactiveAttachments;

};
