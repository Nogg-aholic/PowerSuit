#pragma once

#include "PowerSuit.h"
#include "CoreMinimal.h"
#include "FGInputLibrary.h"
#include "FGCharacterPlayer.h"
#include "FGPlayerController.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Equipment/FGHoverPack.h"
#include "Resources/FGItemDescriptor.h"
#include "EquipmentModuleComponent.h"
#include "Equipment_PowerSuit.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPowerSuitFrictionToggle, FKey, Key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPowerSuitUIToggle, FKey, Key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPowerSuitStatUpdate, int32, Type);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPowerSuitFuelPrefUpdate, TSubclassOf<UFGItemDescriptor>, NewFuel);
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
	// Handles walking/flying stats
	virtual void OnCharacterMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode, EMovementMode NewMovementMode, uint8 NewCustomMode) override;

	UFUNCTION()
	void OnConnectionStatusUpdatedReplacement(bool HasConnection);

	UFUNCTION()
		virtual void SuitClearEquipmentActionBindings();

public:

	UFUNCTION(BlueprintPure)
	float GetCurrentPowerNormalized() const;
	
	UFUNCTION(BlueprintPure)
	float GetSuitPowerCapacity() const;
	
	UFUNCTION(BlueprintPure)
	float GetCurrentPower() const;

	UFUNCTION(BlueprintImplementableEvent)
		void Server_WaitAndInitRemote();

	// Call from server side only, use RCO to set otherwise
	UFUNCTION(BlueprintCallable)
		void SetUserPreferredFuelType(TSubclassOf<UFGItemDescriptor> item);

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

	// Blueprint Callable so that clients can update their own fuel widgets, probably messy but a later problem
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Equipment", DisplayName = "Event OnUserSelectedFuelTypeChange")
		FPowerSuitFuelPrefUpdate OnUserSelectedFuelTypeChange;

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

	// When the player uses the instant start flight hotkey to being flight
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstantFlightStart();

	// When the player starts flight while in a hypertube when they have the flag that allows it
	UFUNCTION(BlueprintImplementableEvent)
		void OnHypertubeFlightEscape();


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

	// Input mapping context to apply when wearing the suit. Use this instead of FGEquipment's
	UPROPERTY(EditAnywhere, Category = "Input")
		TSoftObjectPtr<UInputMappingContext> SuitInputMapping;

	// Priority to give the mapping context
	UPROPERTY(EditAnywhere, Category = "Input")
		int SuitMappingPriority;

	// End user selected fuel they want to use, does nothing on its own without custom suit fueling implementation
	UPROPERTY(BlueprintReadOnly, Replicated, SaveGame)
		TSubclassOf<UFGItemDescriptor> UserPreferredFuelType;
};
