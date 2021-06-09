#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Stats/PowerSuitData.h"
#include "PowerSuitWidget.generated.h"

/**
 * 
 */
UCLASS()
class POWERSUIT_API UPowerSuitWidget : public UUserWidget
{
	GENERATED_BODY()
public:
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void WidgetInit(APowerSuit* Suit, FInventoryStack Stack);

};
