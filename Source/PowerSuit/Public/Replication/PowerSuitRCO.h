

#pragma once

#include "CoreMinimal.h"
#include "FGRemoteCallObject.h"
#include "EquipmentModuleComponent.h"

#include "PowerSuitRCO.generated.h"

/**
 * 
 */
UCLASS()
class POWERSUIT_API UPowerSuitRCO : public UFGRemoteCallObject
{
	GENERATED_BODY()
	
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(replicated)
		bool Dummy;
    UFUNCTION(BlueprintCallable, unreliable, Server)
    void ServerSetEMovementMode(UEquipmentModuleComponent* Component, uint8 Enum);
    UFUNCTION(BlueprintCallable, unreliable, Server)
        void ServerSetEMovementModeCustom(UEquipmentModuleComponent* Component, uint8 Enum);

    UFUNCTION(BlueprintCallable, unreliable, Server)
        void ServerSetHotKeyDirectionalAccel(UEquipmentModuleComponent* Component, bool State);

	UFUNCTION(BlueprintCallable, unreliable, Server)
		void ServerSetHotKeyDirectionalDeAccel(UEquipmentModuleComponent* Component, bool State);

    UFUNCTION(BlueprintCallable, unreliable, Server)
        void ServerSetHotKeyDownAccel(UEquipmentModuleComponent* Component, bool State);

    UFUNCTION(BlueprintCallable, unreliable, Server)
        void ServerSetHotKeyUpAccel(UEquipmentModuleComponent* Component, bool State);

    UFUNCTION(BlueprintCallable, unreliable, Server)
        void ServerSetFlying(UEquipmentModuleComponent* Component, bool State);

    UFUNCTION(BlueprintCallable, unreliable, Server)
        void ServerSetIsSliding(UEquipmentModuleComponent* Component, bool State);

    UFUNCTION(BlueprintCallable, unreliable, Server)
    void ServerSetIsSprinting(UEquipmentModuleComponent* Component, bool State);

    UFUNCTION(BlueprintCallable, unreliable, Server)
       void ServerSetHovering(UEquipmentModuleComponent* Component, bool State);

	UFUNCTION(BlueprintCallable, Server, unreliable)
	void ServerSetAttachmentFloatValue(APowerSuitModuleAttachment* Component, float State, uint8 Index);

	UFUNCTION(BlueprintCallable, Server, unreliable)
	void ServerSetAttachmentBoolValue(APowerSuitModuleAttachment* Component, bool State, uint8 Index);

	UFUNCTION(BlueprintCallable, Server, unreliable)
	void ServerUpdateOnIndex(UEquipmentModuleComponent* Component, int32 Index);

	UFUNCTION(BlueprintCallable, Server, unreliable)
	void ServerSetSuitMovementProperty(APowerSuitModuleAttachment* Attachment, ESuitMovementProperty Enum, FModMultProperty Property);
	UFUNCTION(BlueprintCallable, Server, unreliable)
	void ServerSetSuitFlightProperty(APowerSuitModuleAttachment* Attachment, ESuitFlightProperty Enum, FModMultProperty Property);
	UFUNCTION(BlueprintCallable, Server, unreliable)
	void ServerSetSuitProperty(APowerSuitModuleAttachment* Attachment, ESuitProperty Enum, FModMultProperty Property);
	UFUNCTION(BlueprintCallable, Server, unreliable)
	void ServerSetPropertyGeneral(APowerSuitModuleAttachment* Attachment, EEquipmentStatsProperty PropertyType, uint8 Index, FModMultProperty Property);
	UFUNCTION(BlueprintCallable, Server, unreliable)
	void ServerSetSuitFlag(APowerSuitModuleAttachment* Attachment, ESuitFlag Flag, bool Enabled);
	UFUNCTION(BlueprintCallable, Server, unreliable)
		void ServerUpdateCurrentHoverMode(UEquipmentModuleComponent* Component, EHoverPackMode Index);
};
