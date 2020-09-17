// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "Equipment/FGJetPack.h"
#include "FGCheatManager.h"
#include "EquipmentModuleComponent.h"
#include "PowerSuit.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeDampCurve, UCurveFloat *, DampeningCurve);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConsumePower, float, Delta, float, Mod);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnConsumeFuel, TSubclassOf<class UFGItemDescriptor>, item, int32, slotid, int32, amleft);


UCLASS()
class POWERSUIT_API APowerSuit : public AFGJetPack
{
	GENERATED_BODY()
		APowerSuit();

		virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:

	void ReplicateStates();
	void CheckFuel();

	float CalculateZVelocity(FVector VelInput, FVector PlayerVelocity, float dt, UCurveFloat * FloatTable);
	
	UFUNCTION(BlueprintCallable)
		FVector CalculateDampening(FVector PlayerVelocity, UCurveFloat * FloatTable);
	UFUNCTION(BlueprintCallable)
		void ResetMovementComponent(UFGCharacterMovementComponent  * ref);


	void HandlePipeVelocity(float dt);

	void HandlePowerConsumption(float dt);

	void HandleFuelConsumption(float dt);

	UFUNCTION(BlueprintCallable)
		FVector ApplyMultipleir(FVector Vel1, FVector PlayerVelocity, FVector Forward, UCurveFloat * FloatTable, float dt);
	
	UFUNCTION(BlueprintCallable)
	void ToggleCameraMode(UFGCheatManager * manager) {
		if(manager)
			manager->ToggleCameraMode();
	}

	UFUNCTION(BlueprintImplementableEvent)
		void OnIsSprintingChanged(bool mIsSprinting);
	UFUNCTION(BlueprintImplementableEvent)
		void OnIsSlidingChanged(bool mIsSliding);
	UFUNCTION(BlueprintImplementableEvent)
		void OnCheckHotkeys();
	UFUNCTION(BlueprintImplementableEvent)
		void OnHoverChanged(bool Hover);

	UFUNCTION(BlueprintImplementableEvent)
		void OnPipeVelocityChanged(float newVel);
	UFUNCTION(BlueprintImplementableEvent)
		void OnMoveRep(uint8 MovementMode);
	UFUNCTION(BlueprintImplementableEvent)
		void OnCustomMovementMode(uint8 CustomMovementMode);

	UPROPERTY(BlueprintAssignable, Category = "Equipment", DisplayName = "OnConsumePower")
		FOnConsumePower OnPowerConsumption;

	UPROPERTY(BlueprintAssignable, Category = "Equipment", DisplayName = "OnConsumeFuel")
		FOnConsumeFuel OnFuelConsumption;

	UPROPERTY(BlueprintAssignable, Category = "Equipment", DisplayName = "OnChangeDampCurve")
		FOnChangeDampCurve OnChangeDampCurve;
	
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Equipment")
		bool Hovering;

	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool HotKeyUpAccelration;

	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool HotKeyDownAccelration;

	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool HotKeyDirectionalAccelration;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Equipment")
		float FuelAmount = 1.f;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		float nPipeVelocity;

	/** Keeps is the player sprinting this update or not? */
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		bool nIsSprinting;

	/** Keeps is the player sprinting this update or not? */
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		bool nIsSliding;


	/** is Controlled by a Remote Connection  */
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		bool nRemoteControlled;

	/** is Controlled by a Remote Connection  */
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Movement")
		TSubclassOf<class UFGItemDescriptor> nCurrentFuelItem;

	/**
	* Actor's current movement mode (walking, falling, etc).
	*    - walking:  Walking on a surface, under the effects of friction, and able to "step up" barriers. Vertical velocity is zero.
	*    - falling:  Falling under the effects of gravity, after jumping or walking off the edge of a surface.
	*    - flying:   Flying, ignoring the effects of gravity.
	*    - swimming: Swimming through a fluid volume, under the effects of gravity and buoyancy.
	*    - custom:   User-defined custom movement mode, including many possible sub-modes.
	* This is automatically replicated through the Character owner and for client-server movement functions. 
	* @see SetMovementMode(), CustomMovementMode
	* we are replicating this custom ourselfs bc mentioned replication above doesnt seem to work both ways for us.
	*/
	UPROPERTY(Category = "Character Movement: MovementMode", BlueprintReadWrite)
		TEnumAsByte<enum EMovementMode> nMovementMode;

	/**
	 * Current custom sub-mode if MovementMode is set to Custom.
	 * This is automatically replicated through the Character owner and for client-server movement functions.
	 * @see SetMovementMode()
	 */
	UPROPERTY(Category = "Character Movement: MovementMode", BlueprintReadWrite)
		uint8 nCustomMovementMode;

	UPROPERTY(Category = "Character Movement: MovementMode", BlueprintReadWrite)
		TEnumAsByte<EPowerSuitState> SuitState;

	/** Input X  */
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		float Xaxis;
	/** Input Y  */
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		float Yaxis;



	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	UEquipmentModuleComponent * Module;

	float lastPipeVelocity = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TMap<TEnumAsByte< EPowerSuitState>, float> nBasePowerUsage;
};
