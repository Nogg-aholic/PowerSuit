// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "Resources/FGItemDescriptor.h"
#include "FGEquipment.h"
#include "..\FactoryGame\Public\DamageTypes\FGDamageType.h"
#include "EquipmentModuleDescriptor.generated.h"


USTRUCT(BlueprintType)
struct  POWERSUIT_API  FEquipmentStats
{
	GENERATED_BODY()
public:
	FEquipmentStats();

	FEquipmentStats operator+(const FEquipmentStats& OtherStruct) {
		FEquipmentStats newout;
		// Power
		newout.nPowerCapacity = this->nPowerCapacity + OtherStruct.nPowerCapacity;
		newout.nPowerProduction = this->nPowerProduction + OtherStruct.nPowerProduction;
		newout.nPowerProductionActive = this->nPowerProductionActive + OtherStruct.nPowerProductionActive;
		// Resistances
		newout.nRadioactiveRestistance = this->nRadioactiveRestistance + OtherStruct.nRadioactiveRestistance;
		newout.nGasResistance = this->nGasResistance + OtherStruct.nGasResistance;
		newout.nPhysicalResistance = this->nPhysicalResistance + OtherStruct.nPhysicalResistance;
		newout.nHeatResistance = this->nHeatResistance + OtherStruct.nHeatResistance;
		newout.nFallDamageResistance = this->nFallDamageResistance + OtherStruct.nFallDamageResistance;
		newout.nFallDamageMod = this->nFallDamageMod + OtherStruct.nFallDamageMod;

		if (OtherStruct.nDamageResistance.Num() > 0)
		{
			newout.nDamageResistance = this->nDamageResistance;
			TArray< TSubclassOf<class UFGDamageType>> arr;
			OtherStruct.nDamageResistance.GetKeys(arr);
			for (int32 i = 0; i < arr.Num(); i++)
				if (newout.nDamageResistance.Contains(arr[i]))
					newout.nDamageResistance.Add(arr[i], *newout.nDamageResistance.Find(arr[i]) + OtherStruct.nDamageResistance[arr[i]]);
				else
					newout.nDamageResistance.Add(arr[i], OtherStruct.nDamageResistance[arr[i]]);
		}
		else
		{
			newout.nDamageResistance = this->nDamageResistance;
		}
		//Health
		newout.nHealthMod = this->nHealthMod + OtherStruct.nHealthMod;
		newout.nHealthRegen = this->nHealthRegen + OtherStruct.nHealthRegen;
		newout.nHealthMult = this->nHealthMult + OtherStruct.nHealthMult;
		newout.nHealthRegenMult = this->nHealthRegenMult + OtherStruct.nHealthRegenMult;

		//Shield
		newout.nShieldMod = this->nShieldMod + OtherStruct.nShieldMod;
		newout.nShieldMult = this->nShieldMult + OtherStruct.nShieldMult;
		// ShieldRegen
		newout.nShieldRegen = this->nShieldRegen + OtherStruct.nShieldRegen;
		newout.nShieldRegenMult = this->nShieldRegenMult + OtherStruct.nShieldRegenMult;

		//Timer
		
		newout.nFuseTimeMult = this->nFuseTimeMult + OtherStruct.nFuseTimeMult;
		newout.nActiveTimerMult = this->nActiveTimerMult + OtherStruct.nActiveTimerMult;
		newout.nShieldRegarcheDelayMult = this->nShieldRegarcheDelayMult + OtherStruct.nShieldRegarcheDelayMult;

		newout.nFuseTimeMod = this->nFuseTimeMod + OtherStruct.nFuseTimeMod;
		newout.nActiveTimerMod = this->nActiveTimerMod + OtherStruct.nActiveTimerMod;
		newout.nShieldRegarcheDelayMod = this->nShieldRegarcheDelayMod + OtherStruct.nShieldRegarcheDelayMod;

		// Misc
		newout.nGravityMod = this->nGravityMod + OtherStruct.nGravityMod;
		
		if (OtherStruct.nGravityMod)
			newout.nHasBeltImmunity = true;
		else
			newout.nHasBeltImmunity = this->nHasBeltImmunity;
		
		if (OtherStruct.nHasPipeAccel)
			newout.nHasPipeAccel = true;
		else
			newout.nHasPipeAccel = this->nHasPipeAccel;
		
		if (OtherStruct.nHasFlightUnlock)
			newout.nHasFlightUnlock = true;
		else
			newout.nHasFlightUnlock = this->nHasFlightUnlock;
		
		newout.nSpeedMod = this->nSpeedMod + OtherStruct.nSpeedMod;
		newout.nAirSpeedModSprint = this->nAirSpeedModSprint + OtherStruct.nAirSpeedModSprint;
		newout.nAirSpeedModNormal = this->nAirSpeedModNormal + OtherStruct.nAirSpeedModNormal;
		newout.nAirSpeedModUp = this->nAirSpeedModUp + OtherStruct.nAirSpeedModUp;
		newout.nAirSpeedModDown = this->nAirSpeedModDown + OtherStruct.nAirSpeedModDown;
		newout.nAirGlobalSpeed = this->nAirGlobalSpeed + OtherStruct.nAirGlobalSpeed;
		newout.nGroundSpeedModSprint = this->nGroundSpeedModSprint + OtherStruct.nGroundSpeedModSprint;
		newout.nGroundSpeedModNormal = this->nGroundSpeedModNormal + OtherStruct.nGroundSpeedModNormal;
		newout.nPipeAccel = this->nPipeAccel + OtherStruct.nPipeAccel;

		newout.InventorySlots = this->InventorySlots + OtherStruct.InventorySlots;

		newout.nDampeningCurve = this->nDampeningCurve;

		newout.nDampeningMult = this->nDampeningMult + OtherStruct.nDampeningMult;
		newout.nChargeDuration = this->nChargeDuration + OtherStruct.nChargeDuration;


		newout.JumpZVelocity = this->JumpZVelocity + OtherStruct.JumpZVelocity;
		newout.JumpOffJumpZFactor = this->JumpOffJumpZFactor + OtherStruct.JumpOffJumpZFactor;
		newout.GroundFriction = this->GroundFriction + OtherStruct.GroundFriction;
		newout.MaxWalkSpeed = this->MaxWalkSpeed + OtherStruct.MaxWalkSpeed;
		newout.MaxWalkSpeedCrouched = this->MaxWalkSpeedCrouched + OtherStruct.MaxWalkSpeedCrouched;
		newout.MaxSwimSpeed = this->MaxSwimSpeed + OtherStruct.MaxSwimSpeed;
		newout.BrakingFrictionFactor = this->BrakingFrictionFactor + OtherStruct.BrakingFrictionFactor;
		newout.BrakingFriction = this->BrakingFriction + OtherStruct.BrakingFriction;
		newout.BrakingDecelerationWalking = this->BrakingDecelerationWalking + OtherStruct.BrakingDecelerationWalking;
		newout.BrakingDecelerationFalling = this->BrakingDecelerationFalling + OtherStruct.BrakingDecelerationFalling;
		newout.BrakingDecelerationSwimming = this->BrakingDecelerationSwimming + OtherStruct.BrakingDecelerationSwimming;
		newout.FallingLateralFriction = this->FallingLateralFriction + OtherStruct.FallingLateralFriction;

		newout.mClimbSpeed = this->mClimbSpeed + OtherStruct.mClimbSpeed;
		newout.mMaxSprintSpeed = this->mMaxSprintSpeed + OtherStruct.mMaxSprintSpeed;
		newout.mMaxSlideAngle = this->mMaxSlideAngle + OtherStruct.mMaxSlideAngle;
		newout.mBoostJumpZMult = this->mBoostJumpZMult + OtherStruct.mBoostJumpZMult;
		newout.mBoostJumpVelocityMult = this->mBoostJumpVelocityMult + OtherStruct.mBoostJumpVelocityMult;
		newout.nExternalPowerMult = this->nExternalPowerMult + OtherStruct.nExternalPowerMult;
		newout.nExternalPowerMod = this->nExternalPowerMod + OtherStruct.nExternalPowerMod;
		newout.nExternalPowerMultAirOnly = this->nExternalPowerMultAirOnly + OtherStruct.nExternalPowerMultAirOnly;
		newout.nExternalPowerModAirOnly = this->nExternalPowerModAirOnly + OtherStruct.nExternalPowerModAirOnly;
		newout.nExternalPowerMultGroundOnly = this->nExternalPowerMultGroundOnly + OtherStruct.nExternalPowerMultGroundOnly;
		newout.nExternalPowerModGroundOnly = this->nExternalPowerModGroundOnly + OtherStruct.nExternalPowerModGroundOnly;

		newout.nBaseFuelConsumption = this->nBaseFuelConsumption + OtherStruct.nBaseFuelConsumption;
		newout.nGroundSprintFuelConsumption = this->nGroundSprintFuelConsumption + OtherStruct.nGroundSprintFuelConsumption;
		newout.nHoverFuelConsumption = this->nHoverFuelConsumption + OtherStruct.nHoverFuelConsumption;
		newout.nHoverMoveFuelConsumption = this->nHoverMoveFuelConsumption + OtherStruct.nHoverMoveFuelConsumption;
		newout.nSprintHoverFuelConsumption = this->nSprintHoverFuelConsumption + OtherStruct.nSprintHoverFuelConsumption;
		newout.nFuelConsumptionMult = this->nFuelConsumptionMult + OtherStruct.nFuelConsumptionMult;
		newout.nEmtpyFuelPowerPenalty = this->nEmtpyFuelPowerPenalty + OtherStruct.nEmtpyFuelPowerPenalty;
		newout.nEmtpyPowerFuelPenalty = this->nEmtpyPowerFuelPenalty + OtherStruct.nEmtpyPowerFuelPenalty;

		return newout;
	}


	// Power
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "EquipmentModule | Power")
		float nPowerCapacity = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Power")
		float nPowerProduction = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Power")
		float nPowerProductionActive = 0;

	/** Mult for External Power */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Power | External")
		float nExternalPowerMult;
	/** added before nExternalPowerMultiply Multiplication */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Power | External")
		float nExternalPowerMod;
	/** Mult for External Power */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Power | External")
		float nExternalPowerMultAirOnly;
	/** added before nExternalPowerMultiply Multiplication */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Power | External")
		float nExternalPowerModAirOnly;
	/** Mult for External Power */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Power | External")
		float nExternalPowerMultGroundOnly;
	/** added before nExternalPowerMultiply Multiplication */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Power | External")
		float nExternalPowerModGroundOnly;


	// Fuel

		/** MWs added at all times */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Fuel")
		float nBaseFuelConsumption;
	/** MWs added when Ground Sprint used */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Fuel")
		float nGroundSprintFuelConsumption;
	/** MWs added when Hovering*/
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Fuel")
		float nHoverFuelConsumption;
	/** MWs added when Hovering and Moving */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Fuel")
		float nHoverMoveFuelConsumption;
	/** MWs added when Sprint Hovering*/
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Fuel")
		float nSprintHoverFuelConsumption;
	/** -1 -> 100% reduction / .5 -> 50% increase  */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Fuel")
		float nFuelConsumptionMult;
	/** When Fuel is Empty, this is !subtracted! from PowerProduction*/
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Fuel")
		float nEmtpyFuelPowerPenalty;
	/** When Fuel is Empty, this is !subtracted! from PowerProduction*/
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Fuel")
		float nEmtpyPowerFuelPenalty;

	// Resistances
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Resistances")
		float nRadioactiveRestistance = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Resistances")
		float nGasResistance = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Resistances")
		float nPhysicalResistance = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Resistances")
		float nHeatResistance = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Resistances")
		float nFallDamageResistance = 0;
	// if this is -40 , every Fall Damage under 40 wont be applied, everything above is reduced by it
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Resistances")
		float nFallDamageMod = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Resistances")
		TMap<TSubclassOf<class UFGDamageType>, float> nDamageResistance;

	// Health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Health")
		float nHealthMod = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Health")
		float nHealthMult = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Health")
		float nHealthRegen = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Health")
		float nHealthRegenMult = 0;

	// Shield
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Shield")
		float nShieldMod = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Shield")
		float nShieldMult = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Shield")
		float nShieldRegen = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Shield")
		float nShieldRegenMult = 0;


	// The time the Suits needs to boot up after Power Out ->  FuseTime = FuseTime * 1 + nFuseTime 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Timer")
		float nFuseTimeMult = 0;
	// When recharing Energy Shield the Suits becomes active, the Min Duration for That ->  ActiveTimer = ActiveTimer * 1 + nActiveTimerMult 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Timer")
		float nActiveTimerMult = 0;
	// When damaged Energy Shield needs time before recharge begins ->  ShieldRegarcheDelay = ShieldRegarcheDelay * 1 + nShieldRegarcheDelayMult 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Timer")
		float nShieldRegarcheDelayMult = 0;

	// The time the Suits needs to boot up after Power Out ->  FuseTime = (FuseTime-nFuseTimeMod) * 1 + nFuseTime 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Timer")
		float nFuseTimeMod = 0;
	// When recharing Energy Shield the Suits becomes active, the Min Duration for That ->  ActiveTimer = (ActiveTimer-nActiveTimerMod) * 1 + nActiveTimerMult 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Timer")
		float nActiveTimerMod = 0;
	// When damaged Energy Shield needs time before recharge begins ->  ShieldRegarcheDelay = (ShieldRegarcheDelay -nShieldRegarcheDelayMod) * 1 + nShieldRegarcheDelayMult 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Timer")
		float nShieldRegarcheDelayMod = 0;
	
	// Misc
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Misc")
		float nGravityMod = 0;

	// Unlocks
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Misc")
		bool nHasBeltImmunity = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Misc")
		bool nHasPipeAccel = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Misc")
		bool nHasFlightUnlock = false;


	//speed control
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "EquipmentModule | Movement")
		float nSpeedMod = 0;
	// Air Boost Mod
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nAirSpeedModSprint = 0;
	// Air Normal Mod
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nAirSpeedModNormal = 0;
	// Air up velocity Mod
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nAirSpeedModUp = 0;
	// Air down Velocity Mod
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nAirSpeedModDown = 0;
	// Air Both Sprint and Normal
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nAirGlobalSpeed = 0;
	// Groundspeed with Suit Sprint
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Ground")
		float nGroundSpeedModSprint = 0;
	// Groundspeed Normal
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Ground")
		float nGroundSpeedModNormal = 0;
	// Really carefull here ! this is added ontop of current velocity each frame multiplied with DeltaTime
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Pipe")
		float nPipeAccel = 0;


	// Inventory 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Inventory")
		int32 InventorySlots = 0;
	// Controls flight Dampening of the Suit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		UCurveFloat * nDampeningCurve = nullptr;

	// additive on currents Suits Damp Mult.
	// basically simulates amplification of air friction
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nDampeningMult = 0;

	// ChargeDuration of Fuel
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement")
		float nChargeDuration = 0;



	// Movement Compnent Stats

	/** Additive on Default: 500.0f; Initial velocity (instantaneous vertical acceleration) when jumping. */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Jump Z Velocity", ClampMin = "0", UIMin = "0"))
		float JumpZVelocity = 0;
	/** Additive on Default: 0.5f; Fraction of JumpZVelocity to use when automatically "jumping off" of a base actor that's not allowed to be a base for a character. (For example, if you're not allowed to stand on other players.) */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (ClampMin = "0", UIMin = "0"))
		float JumpOffJumpZFactor = 0;
	/* Additive on Default :  1.5.f Mult for boost jump for Z velocity */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | MovementComponent")
		float mBoostJumpZMult;

	/* Additive on Default : 1.29999995231628f Mult for velocity in 2D when boost jumping */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | MovementComponent")
		float mBoostJumpVelocityMult;
	/**
	* Additive on Default: 8.0f;
	* Setting that affects movement control. Higher values allow faster changes in direction.
	* If bUseSeparateBrakingFriction is false, also affects the ability to stop more quickly when braking (whenever Acceleration is zero), where it is multiplied by BrakingFrictionFactor.
	* When braking, this property allows you to control how much friction is applied when moving across the ground, applying an opposing force that scales with current velocity.
	* This can be used to simulate slippery surfaces such as ice or oil by changing the value (possibly based on the material pawn is standing on).
	* @see BrakingDecelerationWalking, BrakingFriction, bUseSeparateBrakingFriction, BrakingFrictionFactor
	*/
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float GroundFriction = 0;
	/** Additive on Default: 500.0f; -> 5m/s The maximum ground speed when walking. Also determines maximum lateral speed when falling. */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float MaxWalkSpeed = 0;
	/** Additive on Default: 240.0f; -> 2,4m/s The maximum ground speed when walking and crouched. */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float MaxWalkSpeedCrouched = 0;
	/**  Additive on Default: 300.0f; -> 3m/s The maximum swimming speed. */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float MaxSwimSpeed = 0;
	/**  Additive on Default : 500.f; -> 5m/s The speed at which a character climbs ladders */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | MovementComponent")
		float mClimbSpeed;
	/** Additive on Default : 900.f; -> 9m/s The speed at witch a character sprints */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EquipmentModule | MovementComponent")//ReadWrite is only to enable blueprint prototoyping of shit.
		float mMaxSprintSpeed;
	/**
	 * Additive on Default: 2.0f;
	 * Factor used to multiply actual value of friction used when braking.
	 * This applies to any friction value that is currently used, which may depend on bUseSeparateBrakingFriction.
	 * @note This is 2 by default for historical reasons, a value of 1 gives the true drag equation.
	 * @see bUseSeparateBrakingFriction, GroundFriction, BrakingFriction
	 */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float BrakingFrictionFactor = 0;
	/**
	 * Additive on Default: 0.f;
	 * Friction (drag) coefficient applied when braking (whenever Acceleration = 0, or if character is exceeding max speed); actual value used is this multiplied by BrakingFrictionFactor.
	 * When braking, this property allows you to control how much friction is applied when moving across the ground, applying an opposing force that scales with current velocity.
	 * Braking is composed of friction (velocity-dependent drag) and constant deceleration.
	 * This is the current value, used in all movement modes; if this is not desired, override it or bUseSeparateBrakingFriction when movement mode changes.
	 * @note Only used if bUseSeparateBrakingFriction setting is true, otherwise current friction such as GroundFriction is used.
	 * @see bUseSeparateBrakingFriction, BrakingFrictionFactor, GroundFriction, BrakingDecelerationWalking
	 */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", EditCondition = "bUseSeparateBrakingFriction"))
		float BrakingFriction = 0;
	/**
	 * additive on Default: 2048.f
	 * Deceleration when walking and not applying acceleration. This is a constant opposing force that directly lowers velocity by a constant value.
	 * @see GroundFriction, MaxAcceleration
	 */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float BrakingDecelerationWalking = 0;
	/**
	 * Additive on Default : 0.f;
	 * Lateral deceleration when falling and not applying acceleration.
	 * @see MaxAcceleration
	 */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float BrakingDecelerationFalling = 0;
	/**
	 * Additive on Default : 100.f;
	 * Deceleration when swimming and not applying acceleration.
	 * @see MaxAcceleration
	 */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float BrakingDecelerationSwimming = 0;
	/**
	 * Additive on Default : 0.0f; 
	 * Friction to apply to lateral air movement when falling.
	 * If bUseSeparateBrakingFriction is false, also affects the ability to stop more quickly when braking (whenever Acceleration is zero).
	 * @see BrakingFriction, bUseSeparateBrakingFriction
	 */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float FallingLateralFriction = 0;
	
	/** Max angle ( in radians ) for allowing to slide */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | MovementComponent")
		float mMaxSlideAngle;


	~FEquipmentStats() = default;

};


/**
 * 
 */
UCLASS()
class POWERSUIT_API UEquipmentModuleDescriptor : public UFGItemDescriptor
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static bool IsAllowed(TSubclassOf< UEquipmentModuleDescriptor > inClass, AFGEquipment * equipment) { if (inClass && equipment) return inClass.GetDefaultObject()->nAllowedUsage.Contains(equipment->GetClass()); else return false; }

	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static bool GetnUniqueUsage(TSubclassOf< UEquipmentModuleDescriptor > inClass) { if (inClass) return inClass.GetDefaultObject()->nUniqueUsage; else return bool(); }
	
	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static TSubclassOf<class AActor> GetnAttachment(TSubclassOf< UEquipmentModuleDescriptor > inClass) { if (inClass) return inClass.GetDefaultObject()->nAttachment; else return nullptr; }
	
	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static TArray<TSubclassOf<class AFGEquipment>> GetnAllowedUsage(TSubclassOf< UEquipmentModuleDescriptor > inClass) { if (inClass) return inClass.GetDefaultObject()->nAllowedUsage; else return TArray<TSubclassOf<class AFGEquipment>>(); }
	
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		FEquipmentStats EquipmentStats;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		bool nUniqueUsage;
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		TSubclassOf<class AActor> nAttachment;
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		TArray<TSubclassOf<class AFGEquipment>> nAllowedUsage;


};
