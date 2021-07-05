

#pragma once

#include "CoreMinimal.h"
#include "FGInputLibrary.h"
#include "FGCharacterPlayer.h"
#include "FGPlayerController.h"
#include "InputCoreTypes.h"
#include "Equipment/FGHoverPack.h"
#include "EquipmentModuleComponent.h"
#include "Equipment_PowerSuit.generated.h"





DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPowerSuitFrictionToggle, FKey, Key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPowerSuitUIToggle, FKey, Key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPowerSuitStatUpdate, int32, Type);

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
	virtual bool ShouldSaveState() const override;
	virtual void AddEquipmentActionBindings() override;

	UFUNCTION()
	void OnConnectionStatusUpdatedReplacement(bool HasConnection);

public:

	UFUNCTION(BlueprintPure)
	float GetCurrentPowerNormalized() const;
	
	UFUNCTION(BlueprintPure)
	float GetSuitPowerCapacity() const;
	
	UFUNCTION(BlueprintPure)
	float GetCurrentPower() const;


	UFUNCTION(BlueprintImplementableEvent)
		void Server_WaitAndInitRemote();



	UPROPERTY(BlueprintAssignable, Category = "Equipment", DisplayName = "Event PowerSuitFrictionToggle")
		FPowerSuitFrictionToggle OnPowerSuitFrictionToggle;

	UPROPERTY(BlueprintAssignable, Category = "Equipment", DisplayName = "Event PowerSuitUIToggle")
		FPowerSuitUIToggle OnPowerSuitUIToggle;

	UPROPERTY(BlueprintAssignable, Category = "Equipment", DisplayName = "Event OnPowerSuitStatUpdate")
		FPowerSuitStatUpdate OnPowerSuitStatUpdate;

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
		void OnPlayAirBreakSound();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		UEquipmentModuleComponent * Module;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float BeltVelocityAcceptanceAngle = 35.f;
};
