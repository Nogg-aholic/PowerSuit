

#pragma once

#include "CoreMinimal.h"
#include "FGInputLibrary.h"
#include "FGCharacterPlayer.h"
#include "FGPlayerController.h"
#include "Equipment/FGHoverPack.h"
#include "EquipmentModuleComponent.h"
#include "Equipment_PowerSuit.generated.h"






DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConsumePower, float, Delta, float, Mod);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnConsumeFuel, TSubclassOf<class UFGItemDescriptor>, item, int32, slotid, int32, amleft);


/**
 * 
 */
UCLASS()
class POWERSUIT_API APowerSuit : public AFGHoverPack
{
	GENERATED_BODY()
		APowerSuit();

	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	virtual void Equip(class AFGCharacterPlayer* character) override;
	virtual void UnEquip() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual bool ShouldSaveState() const;
public:

	UFUNCTION(BlueprintImplementableEvent)
		void Server_WaitAndInitRemote();


	UFUNCTION(BlueprintImplementableEvent)
		void OnCheckHotkeys();
	UFUNCTION(BlueprintImplementableEvent)
		void OnIsSprintingChanged(bool NewValue);
	UFUNCTION(BlueprintImplementableEvent)
		void OnIsSlidingChanged(bool NewValue);
	UFUNCTION(BlueprintImplementableEvent)
		void OnFlyingChanged(bool NewValue);
	UFUNCTION(BlueprintImplementableEvent)
		void OnPipeVelocityChanged(float NewValue);
	UFUNCTION(BlueprintImplementableEvent)
		void OnMoveRep(uint8 NewValue);
	UFUNCTION(BlueprintImplementableEvent)
		void OnCustomMovementMode(uint8 NewValue);
	UFUNCTION(BlueprintImplementableEvent)
		void OnPlayBufferSound();
	UFUNCTION(BlueprintImplementableEvent)
		void OnPlayAirBreakSound();

	UPROPERTY(BlueprintAssignable, Category = "Equipment", DisplayName = "OnConsumePower")
		FOnConsumePower OnPowerConsumption;
	UPROPERTY(BlueprintAssignable, Category = "Equipment", DisplayName = "OnConsumeFuel")
		FOnConsumeFuel OnFuelConsumption;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		UEquipmentModuleComponent * Module;
};
