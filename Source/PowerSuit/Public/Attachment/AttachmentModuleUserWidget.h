#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttachmentModuleUserWidget.generated.h"

class APowerSuitModuleAttachment;
/**
 * 
 */
UCLASS()
class POWERSUIT_API UAttachmentModuleUserWidget : public UUserWidget
{
	GENERATED_BODY()
protected: 

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
		void WidgetCreated(APowerSuitModuleAttachment * Parent);

	UPROPERTY(BlueprintReadWrite)
		APowerSuitModuleAttachment* ParentModule;
};
