// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "Resources/FGItemDescriptor.h"
#include "FGEquipment.h"
#include "..\FactoryGame\Public\DamageTypes\FGDamageType.h"
#include "EquipmentModuleDescriptor.generated.h"

UENUM(Blueprinttype)
enum EPowerSuitState
{
	/** None (movement is disabled). */
	NONE,
	STANDING,
	WALKING	,
	SPRINTING,
	HOVER,
	FLYUP,
	FLYDOWN	,
	HOVERMOVE,
	HOVERSPRINT,
	PIPEHYPER,
	PowerSuitStateMAX UMETA(hidden),
};

UENUM(Blueprinttype)
enum ENDamageType
{
	DamageTypeNULL,
	DamageTypePhysical,
	DamageTypeGas,
	DamageTypeRadioActivity,
	DamageTypeFallDamage,
	DamageTypeHeat,
	DamageTypeMAX UMETA(hidden),
};


UENUM(Blueprinttype)
enum ESuitProperty
{
	nNullNadaDefault,
	nPowerCapacity,
	nPowerProduction,
	nPowerProductionActive,
	nExternalPower,
	nBaseFuelConsumption,
	nEmptyFuelPowerBasePenalty,
	nEmptyPowerFuelBasePenalty,
	nHealth,
	nHealthRegen,
	nShield,
	nShieldRegen,
	nFuseTime,
	nFuseTimeOverDraw,
	nActiveTime,
	nShieldRechargeDelay,
	nGravity,
	nSpeed,
	nAirSpeed,
	nDampening,
	nChargeDuration,
	SuitPropertyMAX UMETA(hidden),
};


USTRUCT(BlueprintType)
struct  POWERSUIT_API  FModMultProperty
{
	GENERATED_BODY()
public:
	FModMultProperty();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Multiplier = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Modifier = 0;

	float operator+(float  Input) {
			
		return Input + this->Modifier;
	}
	float operator*(float  Input) {

		return Input * (1 + this->Multiplier);
	}


	float result()
	{
		float resultMultiplier = Multiplier + 1;
		return resultMultiplier * Modifier;
	}

	float resultOrMult()
	{
		float resultMultiplier = Multiplier + 1;
		if (Modifier != 0)
		{
			return resultMultiplier * Modifier;
		}
		else
		{
			return resultMultiplier;
		}
	}

	FModMultProperty operator%(const FModMultProperty& Input) {
		FModMultProperty newout;
		newout.Modifier = Input.Modifier + this->Modifier;
		newout.Multiplier = Input.Multiplier + this->Multiplier;
		return newout;
	}

	~FModMultProperty() = default;
};

USTRUCT(BlueprintType)
struct  POWERSUIT_API  FMovementCompnentStats
{
	GENERATED_BODY()
public:
	FMovementCompnentStats();


	FMovementCompnentStats operator+(const FMovementCompnentStats& OtherStruct) {
		FMovementCompnentStats newout;
		
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
		return newout;
	}
	// Movement Compnent Stats

	/** Additive on Default: 500.0f; Initial velocity (instantaneous vertical acceleration) when jumping. */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadOnly)
		float JumpZVelocity = 0;
	/** Additive on Default: 0.5f; Fraction of JumpZVelocity to use when automatically "jumping off" of a base actor that's not allowed to be a base for a character. (For example, if you're not allowed to stand on other players.) */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadOnly)
		float JumpOffJumpZFactor = 0;
	/* Additive on Default :  1.5.f Mult for boost jump for Z velocity */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | MovementComponent")
		float mBoostJumpZMult = 0;

	/* Additive on Default : 1.29999995231628f Mult for velocity in 2D when boost jumping */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | MovementComponent")
		float mBoostJumpVelocityMult  = 0;
	/**
	* Additive on Default: 8.0f;
	* Setting that affects movement control. Higher values allow faster changes in direction.
	* If bUseSeparateBrakingFriction is false, also affects the ability to stop more quickly when braking (whenever Acceleration is zero), where it is multiplied by BrakingFrictionFactor.
	* When braking, this property allows you to control how much friction is applied when moving across the ground, applying an opposing force that scales with current velocity.
	* This can be used to simulate slippery surfaces such as ice or oil by changing the value (possibly based on the material pawn is standing on).
	* @see BrakingDecelerationWalking, BrakingFriction, bUseSeparateBrakingFriction, BrakingFrictionFactor
	*/
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadOnly)
		float GroundFriction = 0;
	/** Additive on Default: 500.0f; -> 5m/s The maximum ground speed when walking. Also determines maximum lateral speed when falling. */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadOnly)
		float MaxWalkSpeed = 0;
	/** Additive on Default: 240.0f; -> 2,4m/s The maximum ground speed when walking and crouched. */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadOnly)
		float MaxWalkSpeedCrouched = 0;
	/**  Additive on Default: 300.0f; -> 3m/s The maximum swimming speed. */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere)
		float MaxSwimSpeed = 0;
	/**  Additive on Default : 500.f; -> 5m/s The speed at which a character climbs ladders */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | MovementComponent")
		float mClimbSpeed = 0;
	/** Additive on Default : 900.f; -> 9m/s The speed at witch a character sprints */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | MovementComponent")//ReadWrite is only to enable blueprint prototoyping of shit.
		float mMaxSprintSpeed = 0;
	/**
	 * Additive on Default: 2.0f;
	 * Factor used to multiply actual value of friction used when braking.
	 * This applies to any friction value that is currently used, which may depend on bUseSeparateBrakingFriction.
	 * @note This is 2 by default for historical reasons, a value of 1 gives the true drag equation.
	 * @see bUseSeparateBrakingFriction, GroundFriction, BrakingFriction
	 */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadOnly)
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
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadOnly)
		float BrakingFriction = 0;
	/**
	 * additive on Default: 2048.f
	 * Deceleration when walking and not applying acceleration. This is a constant opposing force that directly lowers velocity by a constant value.
	 * @see GroundFriction, MaxAcceleration
	 */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadOnly)
		float BrakingDecelerationWalking = 0;
	/**
	 * Additive on Default : 0.f;
	 * Lateral deceleration when falling and not applying acceleration.
	 * @see MaxAcceleration
	 */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadOnly)
		float BrakingDecelerationFalling = 0;
	/**
	 * Additive on Default : 100.f;
	 * Deceleration when swimming and not applying acceleration.
	 * @see MaxAcceleration
	 */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadOnly)
		float BrakingDecelerationSwimming = 0;
	/**
	 * Additive on Default : 0.0f;
	 * Friction to apply to lateral air movement when falling.
	 * If bUseSeparateBrakingFriction is false, also affects the ability to stop more quickly when braking (whenever Acceleration is zero).
	 * @see BrakingFriction, bUseSeparateBrakingFriction
	 */
	UPROPERTY(Category = "EquipmentModule | MovementComponent", EditAnywhere, BlueprintReadOnly)
		float FallingLateralFriction = 0;

	/** Max angle ( in radians ) for allowing to slide */
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | MovementComponent")
		float mMaxSlideAngle = 0;

	~FMovementCompnentStats() = default;
};

USTRUCT(BlueprintType)
struct  POWERSUIT_API  FEquipmentStats
{
	GENERATED_BODY()
public:
	FEquipmentStats();

	FEquipmentStats operator+(const FEquipmentStats& OtherStruct) {
		FEquipmentStats newout;
		// Power
		newout.nSuitProperties = this->nSuitProperties;
		if (OtherStruct.nSuitProperties.Num() > 0)
		{
			TArray< TEnumAsByte<ESuitProperty>> arr;
			OtherStruct.nSuitProperties.GetKeys(arr);
			for (int32 i = 0; i < arr.Num(); i++)
			{
				if (!newout.nSuitProperties.Contains(arr[i]))
					newout.nSuitProperties.Add(arr[i], OtherStruct.nSuitProperties[arr[i]]);
				else
					newout.nSuitProperties.Add(arr[i], *newout.nSuitProperties.Find(arr[i]) % *OtherStruct.nSuitProperties.Find(arr[i]));
			}

		}
		if (newout.nSuitProperties.Num() != int32(ESuitProperty::SuitPropertyMAX))
		{
			for (int32 i = 0; i < ESuitProperty::SuitPropertyMAX; ++i)
			{
				if (!newout.nSuitProperties.Contains(ESuitProperty(i)))
					newout.nSuitProperties.Add(ESuitProperty(i), FModMultProperty());
			}
		}

		newout.nPowerProductionInState = this->nPowerProductionInState;
		if (OtherStruct.nPowerProductionInState.Num() > 0)
		{
			TArray< TEnumAsByte<EPowerSuitState>> arr;
			OtherStruct.nPowerProductionInState.GetKeys(arr);
			for (int32 i = 0; i < arr.Num(); i++)
				if (newout.nPowerProductionInState.Contains(arr[i]))
					newout.nPowerProductionInState.Add(arr[i], *newout.nPowerProductionInState.Find(arr[i]) % OtherStruct.nPowerProductionInState[arr[i]]);
				else
					newout.nPowerProductionInState.Add(arr[i], OtherStruct.nPowerProductionInState[arr[i]]);
		}
		if (newout.nPowerProductionInState.Num() != int32(EPowerSuitState::PowerSuitStateMAX))
		{
			for (int32 i = 0; i < EPowerSuitState::PowerSuitStateMAX; ++i)
			{
				if (!newout.nPowerProductionInState.Contains(EPowerSuitState(i)))
					newout.nPowerProductionInState.Add(EPowerSuitState(i), FModMultProperty());
			}
		}

		newout.nFuelConsumptionInState = this->nFuelConsumptionInState;
		if (OtherStruct.nFuelConsumptionInState.Num() > 0)
		{
			TArray< TEnumAsByte<EPowerSuitState>> arr;
			OtherStruct.nFuelConsumptionInState.GetKeys(arr);
			for (int32 i = 0; i < arr.Num(); i++)
				if (newout.nFuelConsumptionInState.Contains(arr[i]))
					newout.nFuelConsumptionInState.Add(arr[i], *newout.nFuelConsumptionInState.Find(arr[i]) % OtherStruct.nFuelConsumptionInState[arr[i]]);
				else
					newout.nFuelConsumptionInState.Add(arr[i], OtherStruct.nFuelConsumptionInState[arr[i]]);
		}
		if (newout.nFuelConsumptionInState.Num() != int32(EPowerSuitState::PowerSuitStateMAX))
		{
			for (int32 i = 0; i < EPowerSuitState::PowerSuitStateMAX; ++i)
			{
				if (!newout.nFuelConsumptionInState.Contains(EPowerSuitState(i)))
					newout.nFuelConsumptionInState.Add(EPowerSuitState(i), FModMultProperty());
			}
		}

		newout.nEmptyFuelPowerPenaltyInState = this->nEmptyFuelPowerPenaltyInState;
		if (OtherStruct.nEmptyFuelPowerPenaltyInState.Num() > 0)
		{
			TArray< TEnumAsByte<EPowerSuitState>> arr;
			OtherStruct.nEmptyFuelPowerPenaltyInState.GetKeys(arr);
			for (int32 i = 0; i < arr.Num(); i++)
				if (newout.nEmptyFuelPowerPenaltyInState.Contains(arr[i]))
					newout.nEmptyFuelPowerPenaltyInState.Add(arr[i], *newout.nEmptyFuelPowerPenaltyInState.Find(arr[i]) % OtherStruct.nEmptyFuelPowerPenaltyInState[arr[i]]);
				else
					newout.nEmptyFuelPowerPenaltyInState.Add(arr[i], OtherStruct.nEmptyFuelPowerPenaltyInState[arr[i]]);
		}
		if (newout.nEmptyFuelPowerPenaltyInState.Num() != int32(EPowerSuitState::PowerSuitStateMAX))
		{
			for (int32 i = 0; i < EPowerSuitState::PowerSuitStateMAX; ++i)
			{
				if (!newout.nEmptyFuelPowerPenaltyInState.Contains(EPowerSuitState(i)))
					newout.nEmptyFuelPowerPenaltyInState.Add(EPowerSuitState(i), FModMultProperty());
			}
		}

		newout.nEmptyPowerFuelPenaltyInState = this->nEmptyPowerFuelPenaltyInState;
		if (OtherStruct.nEmptyPowerFuelPenaltyInState.Num() > 0)
		{
			TArray< TEnumAsByte<EPowerSuitState>> arr;
			OtherStruct.nEmptyPowerFuelPenaltyInState.GetKeys(arr);
			for (int32 i = 0; i < arr.Num(); i++)
				if (newout.nEmptyPowerFuelPenaltyInState.Contains(arr[i]))
					newout.nEmptyPowerFuelPenaltyInState.Add(arr[i], *newout.nEmptyPowerFuelPenaltyInState.Find(arr[i]) % OtherStruct.nEmptyPowerFuelPenaltyInState[arr[i]]);
				else
					newout.nEmptyPowerFuelPenaltyInState.Add(arr[i], OtherStruct.nEmptyPowerFuelPenaltyInState[arr[i]]);
		}
		if (newout.nEmptyPowerFuelPenaltyInState.Num() != int32(EPowerSuitState::PowerSuitStateMAX))
		{
			for (int32 i = 0; i < EPowerSuitState::PowerSuitStateMAX; ++i)
			{
				if (!newout.nEmptyPowerFuelPenaltyInState.Contains(EPowerSuitState(i)))
					newout.nEmptyPowerFuelPenaltyInState.Add(EPowerSuitState(i), FModMultProperty());
			}
		}
		newout.nDamageTypeResistance = this->nDamageTypeResistance;
		if (OtherStruct.nDamageTypeResistance.Num() > 0)
		{
			TArray< TEnumAsByte<ENDamageType>> arr;
			OtherStruct.nDamageTypeResistance.GetKeys(arr);
			for (int32 i = 0; i < arr.Num(); i++)
				if (newout.nDamageTypeResistance.Contains(arr[i]))
					newout.nDamageTypeResistance.Add(arr[i], *newout.nDamageTypeResistance.Find(arr[i]) % OtherStruct.nDamageTypeResistance[arr[i]]);
				else
					newout.nDamageTypeResistance.Add(arr[i], OtherStruct.nDamageTypeResistance[arr[i]]);
		}
		if (newout.nDamageTypeResistance.Num() != int32(ENDamageType::DamageTypeMAX))
		{
			for (int32 i = 0; i < ENDamageType::DamageTypeMAX; ++i)
			{
				if (!newout.nDamageTypeResistance.Contains(ENDamageType(i)))
					newout.nDamageTypeResistance.Add(ENDamageType(i), FModMultProperty());
			}
		}
		newout.nDamageResistance = this->nDamageResistance;
		if (OtherStruct.nDamageResistance.Num() > 0)
		{
			TArray< TSubclassOf<class UFGDamageType>> arr;
			OtherStruct.nDamageResistance.GetKeys(arr);
			for (int32 i = 0; i < arr.Num(); i++)
				if (newout.nDamageResistance.Contains(arr[i]))
					newout.nDamageResistance.Add(arr[i], *newout.nDamageResistance.Find(arr[i]) % OtherStruct.nDamageResistance[arr[i]]);
				else
					newout.nDamageResistance.Add(arr[i], OtherStruct.nDamageResistance[arr[i]]);
		}


		
		if (OtherStruct.nHasBeltImmunity)
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

		if (OtherStruct.nFuseNeverBreaksWhenFueled)
			newout.nFuseNeverBreaksWhenFueled = true;
		else
			newout.nFuseNeverBreaksWhenFueled = this->nFuseNeverBreaksWhenFueled;

		newout.nSpeedInState = this->nSpeedInState;
		if (OtherStruct.nSpeedInState.Num() > 0)
		{
			TArray< TEnumAsByte<EPowerSuitState>> arr;
			OtherStruct.nSpeedInState.GetKeys(arr);
			for (int32 i = 0; i < arr.Num(); i++)
				if (newout.nSpeedInState.Contains(arr[i]))
					newout.nSpeedInState.Add(arr[i], *newout.nSpeedInState.Find(arr[i]) % OtherStruct.nSpeedInState[arr[i]]);
				else
					newout.nSpeedInState.Add(arr[i], OtherStruct.nSpeedInState[arr[i]]);
		}
		if (newout.nSpeedInState.Num() != int32(EPowerSuitState::PowerSuitStateMAX))
		{
			for (int32 i = 0; i < EPowerSuitState::PowerSuitStateMAX; ++i)
			{
				if (!newout.nSpeedInState.Contains(EPowerSuitState(i)))
					newout.nSpeedInState.Add(EPowerSuitState(i), FModMultProperty());
			}
		}
	
		newout.InventorySlots = this->InventorySlots + OtherStruct.InventorySlots;

		newout.nDampeningCurve = this->nDampeningCurve;

		newout.nMovementComponentStats = this->nMovementComponentStats + OtherStruct.nMovementComponentStats;
		return newout;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Fuel")
		TMap< TEnumAsByte<ESuitProperty>, FModMultProperty> nSuitProperties;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Fuel")
		TMap<TEnumAsByte< EPowerSuitState>, FModMultProperty> nPowerProductionInState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Fuel")
		TMap<TEnumAsByte< EPowerSuitState>, FModMultProperty> nFuelConsumptionInState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Fuel")
		TMap<TEnumAsByte< EPowerSuitState>, FModMultProperty> nEmptyFuelPowerPenaltyInState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Fuel")
		TMap<TEnumAsByte< EPowerSuitState>, FModMultProperty> nEmptyPowerFuelPenaltyInState;

	// Resistances
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Fuel")
		TMap<TEnumAsByte< ENDamageType>, FModMultProperty> nDamageTypeResistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Resistances")
		TMap<TSubclassOf<class UFGDamageType>, FModMultProperty> nDamageResistance;

	// Unlocks
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Misc")
		bool nHasBeltImmunity = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Misc")
		bool nHasPipeAccel = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Misc")
		bool nHasFlightUnlock = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Misc")
		bool nFuseNeverBreaksWhenFueled = false;

	// Really carefull here ! this is added ontop of current velocity each frame multiplied with DeltaTime
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Fuel")
		TMap<TEnumAsByte< EPowerSuitState>, FModMultProperty> nSpeedInState;

	// Inventory 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Inventory")
		int32 InventorySlots = 0;
	// Controls flight Dampening of the Suit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | Movement | Air")
		UCurveFloat * nDampeningCurve = nullptr;

	/** Max angle ( in radians ) for allowing to slide */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentModule | MovementComponent")
		FMovementCompnentStats nMovementComponentStats;

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
	
	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static FEquipmentStats GetEquipmentStats(TSubclassOf< UEquipmentModuleDescriptor > inClass) { if (inClass) return inClass.GetDefaultObject()->EquipmentStats; else return  FEquipmentStats(); }


	UFUNCTION(BlueprintPure, Category = "FactoryGame|Descriptor|Item")
		static FString GetStatsAsText(TSubclassOf< UEquipmentModuleDescriptor > inClass) {
		UScriptStruct* Struct = inClass.GetDefaultObject()->EquipmentStats.StaticStruct();
		FString Output = TEXT("");
		Struct->ExportText(Output, &inClass.GetDefaultObject()->EquipmentStats, nullptr, nullptr, (PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient || PPF_ParsingDefaultProperties || PPF_SeparateDefine), nullptr);
		return Output;
	};

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
