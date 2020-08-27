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
		newout.nFallDamageModifier = this->nFallDamageModifier + OtherStruct.nFallDamageModifier;
		if (OtherStruct.nDamageResistance.Num() > 0)
		{
			TArray< TSubclassOf<class UFGDamageType>> arr;
			OtherStruct.nDamageResistance.GetKeys(arr);
			for (int32 i = 0; i < arr.Num(); i++)
				if (this->nDamageResistance.Contains(arr[i]))
					this->nDamageResistance.Add(arr[i], *this->nDamageResistance.Find(arr[i]) + OtherStruct.nDamageResistance[arr[i]]);
				else
					this->nDamageResistance.Add(arr[i], OtherStruct.nDamageResistance[arr[i]]);
		}
		else
		{
			newout.nDamageResistance = this->nDamageResistance;
		}
		//Health
		newout.nHealthModifier = this->nHealthModifier + OtherStruct.nHealthModifier;
		newout.nHealthRegen = this->nHealthRegen + OtherStruct.nHealthRegen;
		newout.nHealthMultiplier = this->nHealthMultiplier + OtherStruct.nHealthMultiplier;

		//Shield
		newout.nShieldModifier = this->nShieldModifier + OtherStruct.nShieldModifier;
		newout.nShieldMultiplier = this->nShieldMultiplier + OtherStruct.nShieldMultiplier;
		// ShieldRegen
		newout.nShieldRegen = this->nShieldRegen + OtherStruct.nShieldRegen;
		newout.nShieldRegenMultiplier = this->nShieldRegenMultiplier + OtherStruct.nShieldRegenMultiplier;

		//Timer
		
		newout.nFuseTimeMult = this->nFuseTimeMult + OtherStruct.nFuseTimeMult;
		newout.nActiveTimerMult = this->nActiveTimerMult + OtherStruct.nActiveTimerMult;
		newout.nShieldRegarcheDelayMult = this->nShieldRegarcheDelayMult + OtherStruct.nShieldRegarcheDelayMult;

		newout.nFuseTimeMod = this->nFuseTimeMod + OtherStruct.nFuseTimeMod;
		newout.nActiveTimerMod = this->nActiveTimerMod + OtherStruct.nActiveTimerMod;
		newout.nShieldRegarcheDelayMod = this->nShieldRegarcheDelayMod + OtherStruct.nShieldRegarcheDelayMod;

		// Misc
		newout.nGravityModifier = this->nGravityModifier + OtherStruct.nGravityModifier;
		
		if (OtherStruct.nGravityModifier)
			newout.nHasBeltImmunity = true;
		else
			newout.nHasBeltImmunity = this->nHasBeltImmunity;
		
		if (OtherStruct.nHasPipeAccel)
			newout.nHasPipeAccel = true;
		else
			newout.nHasPipeAccel = this->nHasPipeAccel;
		
		newout.nSpeedModifier = this->nSpeedModifier + OtherStruct.nSpeedModifier;
		newout.nAirSpeedModifierSprint = this->nAirSpeedModifierSprint + OtherStruct.nAirSpeedModifierSprint;
		newout.nAirSpeedModifierNormal = this->nAirSpeedModifierNormal + OtherStruct.nAirSpeedModifierNormal;
		newout.nAirSpeedModifierUp = this->nAirSpeedModifierUp + OtherStruct.nAirSpeedModifierUp;
		newout.nAirSpeedModifierDown = this->nAirSpeedModifierDown + OtherStruct.nAirSpeedModifierDown;
		newout.nAirGlobalSpeed = this->nAirGlobalSpeed + OtherStruct.nAirGlobalSpeed;
		newout.nGroundSpeedModifierSprint = this->nGroundSpeedModifierSprint + OtherStruct.nGroundSpeedModifierSprint;
		newout.nGroundSpeedModifierNormal = this->nGroundSpeedModifierNormal + OtherStruct.nGroundSpeedModifierNormal;
		newout.nPipeAccel = this->nPipeAccel + OtherStruct.nPipeAccel;

		newout.InventorySlots = this->InventorySlots + OtherStruct.InventorySlots;

		newout.nDampeningCurve = this->nDampeningCurve;

		newout.nDampeningMultiplier = this->nDampeningMultiplier + OtherStruct.nDampeningMultiplier;
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
		newout.mBoostJumpZMultiplier = this->mBoostJumpZMultiplier + OtherStruct.mBoostJumpZMultiplier;
		newout.mBoostJumpVelocityMultiplier = this->mBoostJumpVelocityMultiplier + OtherStruct.mBoostJumpVelocityMultiplier;

		return newout;
	}


	// Power
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "EquipmentModule | Power")
		float nPowerCapacity = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Power")
		float nPowerProduction = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Power")
		float nPowerProductionActive = 0;

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
		float nFallDamageModifier = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Resistances")
		TMap<TSubclassOf<class UFGDamageType>, float> nDamageResistance;

	// Health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Health")
		float nHealthModifier = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Health")
		float nHealthRegen = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Health")
		float nHealthMultiplier = 0;


	// Shield
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Shield")
		float nShieldModifier = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Shield")
		float nShieldMultiplier = 0;



	// ShieldRegen
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Shield")
		float nShieldRegen = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Shield")
		float nShieldRegenMultiplier = 0;

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
		float nGravityModifier = 0;
	// Unlocks
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Misc")
		bool nHasBeltImmunity = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Misc")
		bool nHasPipeAccel = false;



	//speed control
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "EquipmentModule | Movement")
		float nSpeedModifier = 0;
	// Air

	// Air Boost Modifier
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nAirSpeedModifierSprint = 0;
	// Air Normal Modifier
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nAirSpeedModifierNormal = 0;
	// Air up velocity Modifier
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nAirSpeedModifierUp = 0;
	// Air down Velocity Modifier
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nAirSpeedModifierDown = 0;

	// Air Both Sprint and Normal
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nAirGlobalSpeed = 0;

	// Ground

	// Groundspeed with Suit Sprint
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Ground")
		float nGroundSpeedModifierSprint = 0;
	// Groundspeed Normal
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Ground")
		float nGroundSpeedModifierNormal = 0;
	
	// Pipe 

	// Really carefull here ! this is added ontop of current velocity each frame multiplied with DeltaTime
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Pipe")
		float nPipeAccel = 0;

	// Inventory 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Inventory")
		int32 InventorySlots = 0;


	// Controls flight Dampening of the Suit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		UCurveFloat * nDampeningCurve = nullptr;

	// additive on currents Suits Damp multiplier
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		float nDampeningMultiplier = 0;

	// ChargeDuration of Fuel
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement")
		float nChargeDuration = 0;

	// Movement Compnent Stats
	/** Additive on Default: 500.0f; Initial velocity (instantaneous vertical acceleration) when jumping. */
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Jump Z Velocity", ClampMin = "0", UIMin = "0"))
		float JumpZVelocity = 0;

	/** Additive on Default: 0.5f; Fraction of JumpZVelocity to use when automatically "jumping off" of a base actor that's not allowed to be a base for a character. (For example, if you're not allowed to stand on other players.) */
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (ClampMin = "0", UIMin = "0"))
		float JumpOffJumpZFactor = 0;
	
	
	/* Additive on Default :  1.5.f Multiplier for boost jump for Z velocity */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float mBoostJumpZMultiplier;

	/* Additive on Default : 1.29999995231628f Multiplier for velocity in 2D when boost jumping */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float mBoostJumpVelocityMultiplier;
	
	/**
	* Additive on Default: 8.0f;
	* Setting that affects movement control. Higher values allow faster changes in direction.
	* If bUseSeparateBrakingFriction is false, also affects the ability to stop more quickly when braking (whenever Acceleration is zero), where it is multiplied by BrakingFrictionFactor.
	* When braking, this property allows you to control how much friction is applied when moving across the ground, applying an opposing force that scales with current velocity.
	* This can be used to simulate slippery surfaces such as ice or oil by changing the value (possibly based on the material pawn is standing on).
	* @see BrakingDecelerationWalking, BrakingFriction, bUseSeparateBrakingFriction, BrakingFrictionFactor
	*/
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float GroundFriction = 0;
	
	/** Additive on Default: 500.0f; -> 5m/s The maximum ground speed when walking. Also determines maximum lateral speed when falling. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float MaxWalkSpeed = 0;

	/** Additive on Default: 240.0f; -> 2,4m/s The maximum ground speed when walking and crouched. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float MaxWalkSpeedCrouched = 0;

	/**  Additive on Default: 300.0f; -> 3m/s The maximum swimming speed. */
	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float MaxSwimSpeed = 0;

	/**  Additive on Default : 500.f; -> 5m/s The speed at which a character climbs ladders */
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
		float mClimbSpeed;

	/** Additive on Default : 900.f; -> 9m/s The speed at witch a character sprints */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sprint")//ReadWrite is only to enable blueprint prototoyping of shit.
		float mMaxSprintSpeed;


	/**
	 * Additive on Default: 2.0f;
	 * Factor used to multiply actual value of friction used when braking.
	 * This applies to any friction value that is currently used, which may depend on bUseSeparateBrakingFriction.
	 * @note This is 2 by default for historical reasons, a value of 1 gives the true drag equation.
	 * @see bUseSeparateBrakingFriction, GroundFriction, BrakingFriction
	 */
	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
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
	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", EditCondition = "bUseSeparateBrakingFriction"))
		float BrakingFriction = 0;

	/**
	 * additive on Default: 2048.f
	 * Deceleration when walking and not applying acceleration. This is a constant opposing force that directly lowers velocity by a constant value.
	 * @see GroundFriction, MaxAcceleration
	 */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float BrakingDecelerationWalking = 0;

	/**
	 * Additive on Default : 0.f;
	 * Lateral deceleration when falling and not applying acceleration.
	 * @see MaxAcceleration
	 */
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float BrakingDecelerationFalling = 0;

	/**
	 * Additive on Default : 100.f;
	 * Deceleration when swimming and not applying acceleration.
	 * @see MaxAcceleration
	 */
	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float BrakingDecelerationSwimming = 0;


	/**
	 * Additive on Default : 0.0f; 
	 * Friction to apply to lateral air movement when falling.
	 * If bUseSeparateBrakingFriction is false, also affects the ability to stop more quickly when braking (whenever Acceleration is zero).
	 * @see BrakingFriction, bUseSeparateBrakingFriction
	 */
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float FallingLateralFriction = 0;


	/** Max angle ( in radians ) for allowing to slide */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
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
