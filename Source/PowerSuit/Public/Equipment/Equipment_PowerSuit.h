

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMovementModeChanged, EMovementMode, PreviousMovementMode, uint8, PreviousCustomMode, EMovementMode, NewMovementMode, uint8, NewCustomMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSuitStateUpdate, TEnumAsByte<EPowerSuitState>, StateBefore,TEnumAsByte<EPowerSuitState>, State);

/**
 * 
 */
UCLASS()
class POWERSUIT_API APowerSuit : public AFGHoverPack
{
	GENERATED_BODY()
		APowerSuit();

	friend class UEMC_StateModule;
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void Equip(class AFGCharacterPlayer* character) override;
	virtual void WasSlottedIn(class AFGCharacterPlayer* holder);
	virtual void UnEquip() override;
	virtual void WasRemovedFromSlot();
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual bool ShouldSaveState() const override;
	virtual void AddEquipmentActionBindings() override;
	virtual void OnCharacterMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode, EMovementMode NewMovementMode, uint8 NewCustomMode) override;

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

	UPROPERTY(BlueprintAssignable, Category = "Equipment", DisplayName = "Event OnMovementModeChanged")
		FOnMovementModeChanged OnMovementModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Equipment", DisplayName = "Event OnSuitStateUpdate")
		FOnSuitStateUpdate OnSuitStateUpdate;

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


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName="EMC Module")
		UEquipmentModuleComponent * Module;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float BeltVelocityAcceptanceAngle = 35.f;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
		UFGPowerConnectionComponent* InnerBattery;

	// I think this prevents fuse break when past the time limit? used in TryBreakFuse, and TryRestart, so not really sure why this is BPRW - Rob
	UPROPERTY(BlueprintReadWrite, Replicated)
		bool OverrideReboot = false;

	// If pressing Ctrl+Space should start flight immediately regardless of jump time
	UPROPERTY(BlueprintReadWrite)
		bool EnableCtrlSpaceStartFlight;
};
