#pragma once

#include "CoreMinimal.h"
#include "FactoryGame.h"
#include "Equipment/FGEquipment.h"
#include "Equipment/FGHoverPack.h"
#include "FGInventoryComponent.h"
#include "FGCharacterMovementComponent.h"
#include "FGHealthComponent.h"
#include "Misc/AssertionMacros.h"
#include "DamageTypes/FGDamageType.h"
#include "PowerSuitData.generated.h"



DECLARE_LOG_CATEGORY_EXTERN(PowerSuit_Log, Log, Log);

#ifdef FOR_MODSHIPPING
// Header Edits 
#else

	template<typename Tag, typename Tag::type M>
	struct Steal {
		friend typename Tag::type get(Tag) {
			return M;
		}
	};

	struct steal_mSlideTime {
		typedef float UFGCharacterMovementComponent::* type;
		friend type get(steal_mSlideTime);
	};

	struct steal_mMaxSlideAngle {
		typedef float UFGCharacterMovementComponent::* type;
		friend type get(steal_mMaxSlideAngle);
	};

	struct steal_mBoostJumpZMultiplier {
		typedef float UFGCharacterMovementComponent::* type;
		friend type get(steal_mBoostJumpZMultiplier);
	};

	struct steal_mBoostJumpVelocityMultiplier {
		typedef float UFGCharacterMovementComponent::* type;
		friend type get(steal_mBoostJumpVelocityMultiplier);
	};

	struct steal_mBoostJumpTimeWindow {
		typedef float UFGCharacterMovementComponent::* type;
		friend type get(steal_mBoostJumpTimeWindow);
	};

	struct steal_mZiplineSpeed {
		typedef float UFGCharacterMovementComponent::* type;
		friend type get(steal_mZiplineSpeed);
	};

	struct steal_mZiplineCorrectionSpeedMultiplier {
		typedef float UFGCharacterMovementComponent::* type;
		friend type get(steal_mZiplineCorrectionSpeedMultiplier);
	};

	struct steal_mZiplineVelocityInterpolationSpeed {
		typedef float UFGCharacterMovementComponent::* type;
		friend type get(steal_mZiplineVelocityInterpolationSpeed);
	};

	struct steal_mZiplineSpeedMultiplierUp {
		typedef float UFGCharacterMovementComponent::* type;
		friend type get(steal_mZiplineSpeedMultiplierUp);
	};

	struct steal_mZiplineSpeedMultiplierDown {
		typedef float UFGCharacterMovementComponent::* type;
		friend type get(steal_mZiplineSpeedMultiplierDown);
	};

	struct steal_mPipeData {
		typedef FPlayerPipeHyperData UFGCharacterMovementComponent::* type;
		friend type get(steal_mPipeData);
	};

	struct steal_mHoverSpeed {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mHoverSpeed);
	};
	struct steal_mHoverAccelerationSpeed {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mHoverAccelerationSpeed);
	};
	struct steal_mHoverSprintMultiplier {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mHoverSprintMultiplier);
	};
	struct steal_mRailRoadSurfSpeed {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mRailRoadSurfSpeed);
	};
	struct steal_mRailroadSurfSensitivity {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mRailroadSurfSensitivity);
	};
	struct steal_mHoverFriction {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mHoverFriction);
	};

	struct steal_mJumpKeyHoldActivationTime {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mJumpKeyHoldActivationTime);
	};

	struct steal_mFallSpeedLimitWhenPowered {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mFallSpeedLimitWhenPowered);
	};

	struct steal_mPowerConnectionSearchRadius {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mPowerConnectionSearchRadius);
	};
	struct steal_mPowerConnectionSearchTickRate {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mPowerConnectionSearchTickRate);
	};
	struct steal_mPowerConnectionDisconnectionTime {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mPowerConnectionDisconnectionTime);
	};
	struct steal_mCrouchHoverCancelTime {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mCrouchHoverCancelTime);
	};
	struct steal_mCharacterUseDistanceWhenActive {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mCharacterUseDistanceWhenActive);
	};

	struct steal_mPowerDrainRate {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mPowerDrainRate);
	};
	struct steal_mPowerConsumption {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mPowerConsumption);
	};
	struct steal_mPowerCapacity {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mPowerCapacity);
	};

	struct steal_mCurrentHoverMode {
		typedef EHoverPackMode AFGHoverPack::* type;
		friend type get(steal_mCurrentHoverMode);
	};

	struct steal_mCurrentPowerLevel {
		typedef float AFGHoverPack::* type;
		friend type get(steal_mCurrentPowerLevel);
	};

	template struct Steal<steal_mSlideTime, &UFGCharacterMovementComponent::mSlideTime>;
	template struct Steal<steal_mMaxSlideAngle, &UFGCharacterMovementComponent::mMaxSlideAngle>;
	template struct Steal<steal_mBoostJumpZMultiplier, &UFGCharacterMovementComponent::mBoostJumpZMultiplier>;
	template struct Steal<steal_mBoostJumpVelocityMultiplier, &UFGCharacterMovementComponent::mBoostJumpVelocityMultiplier>;
	template struct Steal<steal_mBoostJumpTimeWindow, &UFGCharacterMovementComponent::mBoostJumpTimeWindow>;
	template struct Steal<steal_mZiplineSpeed, &UFGCharacterMovementComponent::mZiplineSpeed>;
	template struct Steal<steal_mZiplineCorrectionSpeedMultiplier, &UFGCharacterMovementComponent::mZiplineCorrectionSpeedMultiplier>;
	template struct Steal<steal_mZiplineVelocityInterpolationSpeed, &UFGCharacterMovementComponent::mZiplineVelocityInterpolationSpeed>;
	template struct Steal<steal_mZiplineSpeedMultiplierUp, &UFGCharacterMovementComponent::mZiplineSpeedMultiplierUp>;
	template struct Steal<steal_mZiplineSpeedMultiplierDown, &UFGCharacterMovementComponent::mZiplineSpeedMultiplierDown>;
	template struct Steal<steal_mPipeData, &UFGCharacterMovementComponent::mPipeData>;


	template struct Steal<steal_mHoverSpeed, &AFGHoverPack::mHoverSpeed>;
	template struct Steal<steal_mHoverAccelerationSpeed, &AFGHoverPack::mHoverAccelerationSpeed>;
	template struct Steal<steal_mHoverSprintMultiplier, &AFGHoverPack::mHoverSprintMultiplier>;
	template struct Steal<steal_mRailRoadSurfSpeed, &AFGHoverPack::mRailRoadSurfSpeed>;
	template struct Steal<steal_mRailroadSurfSensitivity, &AFGHoverPack::mRailroadSurfSensitivity>;
	template struct Steal<steal_mHoverFriction, &AFGHoverPack::mHoverFriction>;
	template struct Steal<steal_mJumpKeyHoldActivationTime, &AFGHoverPack::mJumpKeyHoldActivationTime>;
	template struct Steal<steal_mFallSpeedLimitWhenPowered, &AFGHoverPack::mFallSpeedLimitWhenPowered>;
	template struct Steal<steal_mPowerConnectionSearchRadius, &AFGHoverPack::mPowerConnectionSearchRadius>;
	template struct Steal<steal_mPowerConnectionSearchTickRate, &AFGHoverPack::mPowerConnectionSearchTickRate>;
	template struct Steal<steal_mPowerConnectionDisconnectionTime, &AFGHoverPack::mPowerConnectionDisconnectionTime>;
	template struct Steal<steal_mCrouchHoverCancelTime, &AFGHoverPack::mCrouchHoverCancelTime>;
	template struct Steal<steal_mCharacterUseDistanceWhenActive, &AFGHoverPack::mCharacterUseDistanceWhenActive>;
	template struct Steal<steal_mPowerDrainRate, &AFGHoverPack::mPowerDrainRate>;
	template struct Steal<steal_mPowerConsumption, &AFGHoverPack::mPowerConsumption>;
	template struct Steal<steal_mPowerCapacity, &AFGHoverPack::mPowerCapacity>;
	template struct Steal<steal_mCurrentHoverMode, &AFGHoverPack::mCurrentHoverMode>;
	template struct Steal<steal_mCurrentPowerLevel, &AFGHoverPack::mCurrentPowerLevel>;


	

	

#endif

/**
 * 
 */
UCLASS()
class POWERSUIT_API UPowerSuitData : public UObject
{
	GENERATED_BODY()

public:


};


UENUM(Blueprinttype)
enum EPowerSuitState
{
	/** None (movement is disabled). */
	PS_NONE UMETA(Displayname = "None"),
	PS_STANDING UMETA(Displayname = "Standing", Tooltip = "On the ground and standing still. Modifying this state's speed doesn't do anything."),
	PS_WALKING UMETA(Displayname = "Walking", Tooltip = "On the ground and pressing a movement key."),
	PS_SPRINTING UMETA(Displayname = "Sprinting", Tooltip = "On the ground and pressing a movement key AND the boost key."),
	PS_HOVER UMETA(Displayname = "Hovering", Tooltip = "Hovering and not pressing any movement keys. The hover equivalent of the Flight Falling state."),
	PS_FLYUP UMETA(Displayname = "Flying/Hovering Up", Tooltip = "Either hovering or flying and pressing the spacebar to ascend."),
	PS_FLYDOWN	UMETA(Displayname = "Flying/Hovering Down", Tooltip = "Either hovering or flying and pressing left alt to descend. This speed seems to be capped at terminal velocity of ~41m/s."),
	PS_HOVERMOVE UMETA(Displayname = "Flying/Hovering & Moving", Tooltip = "Either hovering or flying and actively pressing a movement key."),
	PS_HOVERSPRINT UMETA(Displayname = "Flying/Hovering & Sprinting", Tooltip = "Either hovering or flying and pressing the boost key."),
	PS_PIPEHYPER_RIDE UMETA(Displayname = "HyperTube Riding", Tooltip = "Riding in a hypertube. NOTE: This state can't have speed modified as of 0.3.0, but you can still apply power/fuel costs to it."),
	PS_PIPEHYPER_SPRINT UMETA(Displayname = "HyperTube Sprinting", Tooltip = "Actively accelerating or decelerating in a hypertube."),
	PS_FALLING UMETA(Displayname = "Flight Falling", Tooltip = "Flying and not pressing any movement keys. The non-hover equivalent of the Hovering state."),
	PS_SLIDING UMETA(Displayname = "Sliding", Tooltip = "Sliding and not Boosting."),
	PS_POWERSLIDE UMETA(Displayname = "Power Sliding", Tooltip = "Sliding with Boost (pressing left alt)."),
	PS_ZIPLINE UMETA(Displayname = "Zipline Sliding", Tooltip = "Sliding with Boost (pressing left alt)."),
	PS_ZIPLINE_SPRINT UMETA(Displayname = "Zipline Sliding Boost", Tooltip = "Sliding with Boost (pressing left alt)."),
	PS_REBOOTING UMETA(Displayname = "Rebooting", Tooltip = "Rebooting"),
	PS_SLOWFALL UMETA(Displayname = "SlowFall", Tooltip = ""),

	/** Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change. */
	PowerSuitStateMAX UMETA(hidden, Displayname = "Invalid value!"),
};

UENUM(Blueprinttype, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum ENDamageType
{
	DamageTypeNULL = 0x00 UMETA(hidden, Displayname = "Invalid value!", Tooltip = "Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change."),
	DamageTypePhysical = 1 << 0,
	DamageTypeGas = 1 << 1,
	DamageTypeRadioActivity = 1 << 2,
	DamageTypeFallDamage = 1 << 3,
	DamageTypeHeat = 1 << 4,
	DamageTypeMAX = 1 << 5 UMETA(hidden, Displayname = "Invalid value!", Tooltip = "Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change.")

};
//SuitFlag_HasBeltImmunity = 1 << 0 UMETA(Tooltip = "Prevents belts from moving the character against their intended direction of movement"),

UENUM(Blueprinttype, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum ESuitFlag
{
	SuitFlag_Null = 0x00 UMETA(hidden, Displayname = "Invalid value!", Tooltip = "Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change."),
	SuitFlag_HasPipeAccel = 1 << 0 UMETA(Tooltip = "Allows hotkey-bound accelerating and decelerating in hypertubes"),
	SuitFlag_HasFlightUnlocked = 1 << 1 UMETA(Tooltip = "Allows flight. Be sure to also set flight speed properties and such"),
	SuitFlag_FuseNeverBreaksWhenFueled = 1 << 2 UMETA(Tooltip = "If the suit has fuel available, using too much power will never result in the suit fuse breaking. Works well with EmptyPowerFuelPenalty"),
	SuitFlag_AllowsToggleGravityMode = 1 << 3 UMETA(Tooltip = "HoverPack's flight has no Gravity (results in hover flight) - can Gravity be enabled by the user with the mod's keybind?"),
	SuitFlag_ForceGravityMode = 1 << 4 UMETA(Tooltip = "Force the Suit to use Gravity Mode - it's on by default, but this overwrites toggle"),
	SuitFlag_HasAirReFuel = 1 << 5 UMETA(Tooltip = "Allow refilling the fuel tank while in flight-related movement states"),
	SuitFlag_HasInfiniteSlide = 1 << 6 UMETA(Tooltip = "Allows the user to slide forever as long as the angle is acceptable (continually resets their slide timer"),
	SuitFlag_HasNoFrictionMode = 1 << 7 UMETA(Tooltip = "Toggle Hotkey for Friction SuitValue or 0 "),
	SuitFlag_END = 1 << 8 UMETA(hidden, Displayname = "Invalid value!", Tooltip = "Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change.")

};

UENUM(Blueprinttype)
enum ESuitProperty
{
	/**
	* Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change.
	*/
	SuitPropertyNULL UMETA(hidden, Displayname = "NULL"),
	/**
	* Maximum power in MW that the Suit can store.
	*	Additive
	*/
	nPowerCapacity UMETA(Displayname = "nPowerCapacity"),

	/**
	* Power in MW that the Suit produces/consumes.
	*	Additive
	*	Negative for power consumption
	*/
	nPowerProduction UMETA(Displayname = "nPowerProduction"),

	/**
	* Additional Power in MW that the suit produces/consumed. It is intended for Attachments to draw power through this category.
	* Applied IN ADDITION TO nPowerProduction
	*	Additive
	*	Positive for power consumption (verify?)
	*/
	nPowerConsumption UMETA(Displayname = "nPowerConsumption"),

	/**
	* Additional max health. Default max health is 100.0f.
	*	Additive
	*/
	nHealth UMETA(Displayname = "nMaxHealth"),

	/**
	* Health regeneration active at all times. Default max health is 100.0f.
	*	Additive
	*/
	nHealthRegen UMETA(Displayname = "nHealthRegen"),

	/**
	* Shield HP that is taken from before regular health pool.
	* Needs time after getting hit to start to recharging.
	* Gas and Radiation damage bypass it.
	*	Additive
	*/
	nShield UMETA(Displayname = "nShield"),

	/**
	* Rate at which shield regenerates while it is recharging.
	*	Additive
	*/
	nShieldRegen UMETA(Displayname = "nShieldRegen"),

	/**
	* The time the Suit needs to reboot after blowing the fuse (when out of power for too long).
	*	Additive
	8	Positive for increased time
	*/
	nFuseTime UMETA(Displayname = "nFuseTime"),

	/**
	* The 'grace period' the Suit for being out of power before it blows the fuse and has to reboot.
	*	Additive
	*	Positive for increased time
	*/
	nFuseTimeOverDraw UMETA(Displayname = "nFuseTimeOverDraw"),

	/**
	* The minimum time the Suit needs after taking damage before the shield starts regenerating.
	*	Additive
	*	Positive for increased time
	*/
	nShieldRechargeDelay UMETA(Displayname = "nShieldRechargeDelay"),

	/**
	* Increases the amount energy units we get out of Fuel used by the suit.
	* When the suit refuels, it consumes some amount of items (set by suit) to fill a whole fuel tank's worth of juice.
	* 1 based, where 1 is a full tank of fuel
	* Modifier and Multiplier do basically the same thing here because of the one-based nature
	* Fuel buffer value upon refill = (1 + mod) * (1 + mult)
	*	Ex. Multiplier 1.0	-> fuel worth 100% more (twice as much)
	*	Ex. Multiplier 0.5	-> fuel worth 50% more
	*	Ex. Multiplier -0.5	-> fuel worth -50% (half as much)

	*	Ex. Modifier 1.0	-> fuel worth 100% more (twice as much)
	*	Ex. Modifier 0.5	-> fuel worth 50% more
	*	Ex. Modifier -0.5	-> fuel worth -50% (half as much)

	*	Ex. Modifier 0.5 and Multiplier 0.5	-> fuel worth 125% more (2.25 full tanks)
	*	Ex. Modifier -0.5 and Multiplier 0.5	-> fuel worth -25% (75% as much)
	*	Positive means you get more than a full tank's worth
	*	Additive
	*/
	nFuelEfficiency UMETA(Displayname = "nFuelEfficiency"),

	/**
	* How much Fuel can we Load from the Buffer to the Tank?
	*/
	nFuelTankSize  UMETA(Displayname = "FuelTankSize"),

	/**
	* Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change.
	*/
	SuitPropertyMAX UMETA(hidden, Displayname = "Invalid value!"),
};

UENUM(Blueprinttype)
enum ESuitMovementProperty
{
	/**
	* Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change.
	*/
	ESMC_NULL UMETA(hidden, Displayname = "NULL"),
	/** Additive on Default: 500.0f;
	* Initial velocity (instantaneous vertical acceleration) when jumping.
	* Blade Runners is roughly 300.0f
	*/
	ESMC_JumpZVelocity UMETA(Displayname = "JumpZVelocity"),
	/** Additive on Default: 0.5f; Fraction of JumpZVelocity to use when automatically "jumping off" of a base actor that's not allowed to be a base for a character. (For example, if you're not allowed to stand on other players.) */
	ESMC_JumpOffJumpZFactor UMETA(Displayname = "JumpOffJumpZFactor"),
	/** GravityScale */
	ESMC_GravityScale UMETA(Displayname = "GravityScale"),
	/** Additive on Default :  1.5.f Mult for boost jump for Z velocity */
	ESMC_mBoostJumpZMultiplier UMETA(Displayname = "mBoostJumpZMultiplier"),
	/** Additive on Default : 1.29999995231628f Mult for velocity in 2D when boost jumping */
	ESMC_mBoostJumpVelocityMultiplier UMETA(Displayname = "mBoostJumpVelocityMultiplier"),

	/**
	* Additive on Default: 8.0f;
	* Setting that affects movement control. Higher values allow faster changes in direction.
	* If bUseSeparateBrakingFriction is false, also affects the ability to stop more quickly when braking (whenever Acceleration is zero), where it is multiplied by BrakingFrictionFactor.
	* When braking, this property allows you to control how much friction is applied when moving across the ground, applying an opposing force that scales with current velocity.
	* This can be used to simulate slippery surfaces such as ice or oil by changing the value (possibly based on the material pawn is standing on).
	* @see BrakingDecelerationWalking, BrakingFriction, bUseSeparateBrakingFriction, BrakingFrictionFactor
	*/
	ESMC_GroundFriction UMETA(Displayname = "GroundFriction"),
	/** Additive on Default: 500.0f; -> 5m/s
	* The maximum ground speed when walking. Also determines maximum lateral speed when falling.
	*/
	ESMC_MaxWalkSpeed UMETA(Displayname = "MaxWalkSpeed"),
	/** Additive on Default: 240.0f; -> 2.4m/s
	* The maximum ground speed when walking and crouched.
	*/
	ESMC_MaxWalkSpeedCrouched UMETA(Displayname = "MaxWalkSpeedCrouched"),
	/** Additive on Default: 300.0f; -> 3m/s
	* The maximum swimming speed.
	*/
	ESMC_MaxSwimSpeed UMETA(Displayname = "MaxSwimSpeed"),
	/**
	* Additive on Default: 2.0f;
	* Factor used to multiply actual value of friction used when braking.
	* This applies to any friction value that is currently used, which may depend on bUseSeparateBrakingFriction.
	* @note This is 2 by default for historical reasons, a value of 1 gives the true drag equation.
	* @see bUseSeparateBrakingFriction, GroundFriction, BrakingFriction
	*/
	ESMC_BrakingFrictionFactor UMETA(Displayname = "BrakingFrictionFactor"),
	/**
	* Additive on Default: 0.f;
	* Friction (drag) coefficient applied when braking (whenever Acceleration = 0, or if character is exceeding max speed); actual value used is this multiplied by BrakingFrictionFactor.
	* When braking, this property allows you to control how much friction is applied when moving across the ground, applying an opposing force that scales with current velocity.
	* Braking is composed of friction (velocity-dependent drag) and constant deceleration.
	* This is the current value, used in all movement modes; if this is not desired, override it or bUseSeparateBrakingFriction when movement mode changes.
	* @note Only used if bUseSeparateBrakingFriction setting is true, otherwise current friction such as GroundFriction is used.
	* @see bUseSeparateBrakingFriction, BrakingFrictionFactor, GroundFriction, BrakingDecelerationWalking
	*/
	ESMC_BrakingFriction UMETA(Displayname = "BrakingFriction"),
	/**
	* Additive on Default: 2048.f
	* Deceleration when walking and not applying acceleration. This is a constant opposing force that directly lowers velocity by a constant value.
	* @see GroundFriction, MaxAcceleration
	*/
	ESMC_BrakingDecelerationWalking  UMETA(Displayname = "BrakingDecelerationWalking"),
	/**
	* Additive on Default : 0.f;
	* Lateral deceleration when falling and not applying acceleration.
	* @see MaxAcceleration
	*/
	ESMC_BrakingDecelerationFalling UMETA(Displayname = "BrakingDecelerationFalling"),
	/**
	* Additive on Default : 100.f;
	* Deceleration when swimming and not applying acceleration.
	* @see MaxAcceleration
	*/
	ESMC_BrakingDecelerationSwimming UMETA(Displayname = "BrakingDecelerationSwimming"),
	/**
	* Additive on Default : 0.0f;
	* Friction to apply to lateral air movement when falling.
	* If bUseSeparateBrakingFriction is false, also affects the ability to stop more quickly when braking (whenever Acceleration is zero).
	* @see BrakingFriction, bUseSeparateBrakingFriction
	*/
	ESMC_FallingLateralFriction UMETA(Displayname = "FallingLateralFriction"),
	/** Additive on Default : 500.f; -> 5m/s
	* The speed at which a character climbs ladders. Seems to be capped by base game? Needs further investigating
	*/
	ESMC_mClimbSpeed UMETA(Displayname = "mClimbSpeed"),
	/** Additive on Default : 900.f; -> 9m/s
	* The speed at which a character sprints.
	* Blade Runners is roughly 450.0f
	*/
	ESMC_mMaxSprintSpeed UMETA(Displayname = "mMaxSprintSpeed"),
	/**
	* Max angle ( in radians ) for allowing to slide
	*/
	ESMC_mMaxSlideAngle UMETA(Displayname = "mMaxSlideAngle"),
	/* How long time after a slide a jump can be input and be counted as a boost jump*/
	ESMC_mBoostJumpTimeWindow UMETA(Displayname = "mBoostJumpTimeWindow"),
	/* Base speed for moving straight on zipline */
	ESMC_mZiplineSpeed UMETA(Displayname = "mZiplineSpeed"),
	/* The speed of which the character corrects its position when ziplining. */
	ESMC_mZiplineCorrectionSpeedMultiplier UMETA(Displayname = "mZiplineCorrectionSpeedMultiplier"),
	/* How fast we interpolate to max speed when ziplining */
	ESMC_mZiplineVelocityInterpolationSpeed UMETA(Displayname = "mZiplineVelocityInterpolationSpeed"),
	/* Speed multiplier used when going upwards on zipline */
	ESMC_mZiplineSpeedMultiplierUp UMETA(Displayname = "mZiplineSpeedMultiplierUp"),
	/* Speed multiplier used when going downwards on zipline */
	ESMC_mZiplineSpeedMultiplierDown UMETA(Displayname = "mZiplineSpeedMultiplierDown"),

	ESMC_mMaxSpeedInPipes UMETA(Displayname = "mMaxSpeedInPipes"),
	/**
	* Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change.
	*/
	ESMC_MAX UMETA(hidden, Displayname = "Invalid value!"),
};

UENUM(Blueprinttype)
enum ESuitFlightProperty
{
	/**
	* Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change.
	*/
	EFP_NULL UMETA(hidden, Displayname = "NULL"),
	/** How fast the character moves with the hover pack. Base game is 650.0f */
	EFP_mHoverSpeed UMETA(Displayname = "mHoverSpeed"),
	/** How fast the hoverpack accelerates. Units / Second. Might need to be higher / lower depending on hover friction. */
	EFP_mHoverAccelerationSpeed UMETA(Displayname = "mHoverAccelerationSpeed"),
	/** How fast the character moves with the hover pack when sprinting. Base game is is 2.0f*/
	EFP_mHoverSprintMultiplier UMETA(Displayname = "mHoverSprintMultiplier"),
	/** How fast the character moves with the hover pack when rail surfing. */
	EFP_mRailRoadSurfSpeed UMETA(Displayname = "mRailRoadSurfSpeed"),
	/** Determines how loosely you need to follow the track with the view in order to surf it. */
	EFP_mRailroadSurfSensitivity UMETA(Displayname = "mRailroadSurfSensitivity"),
	/** Friction of the movement while airborne (0-1). */
	EFP_mHoverFriction UMETA(Displayname = "mHoverFriction"),
	/** How long the user needs to hold jump in order to activate the hoverpack. */
	EFP_mJumpKeyHoldActivationTime UMETA(Displayname = "mJumpKeyHoldActivationTime"),
	/** How fast the character is falls if hoverpack has power but is disconnected from grid. */
	EFP_mFallSpeedLimitWhenPowered UMETA(Displayname = "mFallSpeedLimitWhenPowered"),
	/** How far away we look for power connections. */
	EFP_mPowerConnectionSearchRadius UMETA(Displayname = "mPowerConnectionSearchRadius"),
	/** How often we look for new connections. */
	EFP_mPowerConnectionSearchTickRate UMETA(Displayname = "mPowerConnectionSearchTickRate"),
	/** How much time needs to pass after leaving a power connection before getting disconnected. */
	EFP_mPowerConnectionDisconnectionTime UMETA(Displayname = "mPowerConnectionDisconnectionTime"),
	/** How fast the player has to double tap crouch in order to exit hover mode. */
	EFP_mCrouchHoverCancelTime UMETA(Displayname = "mCrouchHoverCancelTime"),
	/** How far the character using the hoverpack can reach when it's active. */
	EFP_mCharacterUseDistanceWhenActive UMETA(Displayname = "mCharacterUseDistanceWhenActive"),
	/** How much power is drained per second when using the slow fall mode. */
	EFP_mPowerConsumption UMETA(Displayname = "mPowerConsumption"),
	/** How much power the hoverpack drains when connected to a grid.
	* 
	* PowerSuit note: Modified to be in MWs. This is what the hoverpack will actually consume when flying. 
	* But don't use this for flying power consumption, because it can't shut off the suit when you run out (base game clamped).
	* Use an attachment to do movement state dependent power consumption instead.
	*/
	EFP_mPowerDrainRate UMETA(Displayname = "mPowerDrainRate"),
	/**
	* Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change.
	*/
	EFP_MAX UMETA(hidden, Displayname = "Invalid value!"),
};

UENUM(Blueprinttype)
enum EEquipmentStatsProperty
{
	ESSP_nMovementProperties UMETA(Displayname = "nMovementProperties"),
	ESSP_nFlightProperties UMETA(Displayname = "nFlightProperties"),
	ESSP_nSuitProperties UMETA(Displayname = "nSuitProperties"),
	ESSP_nDamageTypeResistance UMETA(Displayname = "nDamageTypeResistance"),
	ESSP_nDamageResistance UMETA(Displayname = "nDamageResistance"),
	ESSP_MAX UMETA(hidden, Displayname = "Invalid value!"),
};

USTRUCT(BlueprintType)
struct  POWERSUIT_API  FModMultProperty
{
	GENERATED_BODY()
public:
	FModMultProperty();

	/**
	* Additive multiplier for the property. This number is first added to 1.0, then all Multiplier values are summed, then multiplied with the sum of the Modifiers.
	* End result value is Base + (SumOfModifiers * SumOfMultipliers)
	* Make sure you have a Modifier set by some module (or the suit) for it to act on - zero times any Multiplier value is still zero.
	* Examples:
	*	0.0f	->	No change
	*	0.1f	->	+10%
	*	1.0f	->	+100%
	*	-1.0f	->	-100% Keep in mind that this can still be counteracted with positive values - this doesn't necessarily mean "set to zero"
	*	-0.1f	->	-10%
	*/
	UPROPERTY(EditDefaultsOnly, SaveGame, BlueprintReadWrite)
		float Multiplier = 0;

	/**
	* Additive modifier for the property. This number is summed with the other Modifiers before multiplication with Multipliers.
	* End result value is Base + (SumOfModifiers * SumOfMultipliers)
	* Most speed-related values are in cm/s
	* Most time-related values are in seconds
	* Examples:
	*	0.0f	->	No change
	*	100.0	-> +100.0f to the value (ex. +1.0 m/s)
	*	-10.0	-> +10.0f to the value (ex. -0.1 m/s)
	*/
	UPROPERTY(EditDefaultsOnly, SaveGame, BlueprintReadWrite)
		float Modifier = 0;

	// This operator Adds only the Modifier to the Input Value 
	float operator+(const float  Input) const {
		return Input + Modifier;
	}
	// This operator Multiplies the Input Value only with the Multiplier 1 based
	float operator*(const float  Input) const {
		return Input * (1 + Multiplier);
	}

	// Returns Possibly 0 if Modifier is 0 , Multiplier is 1 Based
	float value() const {
		return (Multiplier + 1) * Modifier;
	}

	// 1 Based Multiplier * Mod + Add Clamped to [0 <-> 99999999]
	float ClampAdd(const int32 Add) const {
		return FMath::Clamp((Multiplier + 1) * (Modifier + Add), 0.f, 99999999.f);
	}

	// 1 Based Multiplier * Mod Clamped to [0 <-> 99999999]
	float Clamp() const {
		return FMath::Clamp((Multiplier + 1) * (Modifier), 0.f, 99999999.f);
	}
	// 1 Based Multiplier Clamped to [0 <-> 99999999]
	float ClampMult()const {
		return FMath::Clamp(Multiplier + 1, 0.f, 99999999.f);
	}
	// since + is already in use for Modifier Adding you can use % to add Both Mult and Mod together
	FModMultProperty operator%(const FModMultProperty& Input) {
		Modifier = Input.Modifier + Modifier;
		Multiplier = Input.Multiplier + Multiplier;
		return *this;
	}
	FModMultProperty operator-(const FModMultProperty& Input) {
		Modifier = Modifier - Input.Modifier;
		Multiplier = Multiplier - Input.Multiplier;
		return *this;
	}
	~FModMultProperty() = default;
};

class UEquipmentModuleComponent;
class APowerSuitModuleAttachment;

USTRUCT(BlueprintType)
struct  POWERSUIT_API  FEquipmentStats
{
	GENERATED_BODY()
public:
	FEquipmentStats();

	void SetupDefaults();

	void UnlockFuels(UEquipmentModuleComponent* Parent, TArray<TSubclassOf<class UFGItemDescriptor>> FuelsToUnlock);

	void ForgetUnlockedFuels(UEquipmentModuleComponent* Parent);

	FEquipmentStats operator+(const FEquipmentStats& OtherStruct);

	FEquipmentStats operator-(const FEquipmentStats& OtherStruct);

	bool HasFlag(UPARAM(meta = (Bitmask, BitmaskEnum = ESuitFlag)) int32 Bitmask) const;;

	bool HasDamageMask(UPARAM(meta = (Bitmask, BitmaskEnum = ENDamageType)) int32 Bitmask) const;;

	bool HasRemoveFlag(UPARAM(meta = (Bitmask, BitmaskEnum = ESuitFlag)) int32 Bitmask) const;;

	bool HasRemoveDamageMask(UPARAM(meta = (Bitmask, BitmaskEnum = ENDamageType)) int32 Bitmask) const;;


	/**
	* Global settings that change base numeric properties of the suit
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame, Category = "EquipmentModule")
		TMap< TEnumAsByte<ESuitMovementProperty>, FModMultProperty> nMovementProperties;

	/**
	* Global settings that change base numeric properties of the suit
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame, Category = "EquipmentModule")
		TMap< TEnumAsByte<ESuitFlightProperty>, FModMultProperty> nFlightProperties;

	/**
	* Global settings that change base numeric properties of the suit
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame, Category = "EquipmentModule")
		TMap< TEnumAsByte<ESuitProperty>, FModMultProperty> nSuitProperties;

	// Resistances

	/**
	* Resistance to certain categories of damage.
	*	Additive
	*	Multiplier 1.0 is full resistance
	*	Consider checking Damage Resistance options as well
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame, Category = "EquipmentModule | Resistances")
		TMap<TEnumAsByte< ENDamageType>, FModMultProperty> nDamageTypeResistance;

	/**
	* Resistances to specific DamageTypes.
	*	Additive
	*	Multiplier 1.0 is full resistance
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame, Category = "EquipmentModule | Resistances")
		TMap<TSubclassOf<class UFGDamageType>, FModMultProperty> nDamageResistance;

	/**
	*	What Damage Types the Suit's EnergyShield can Absorb
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, meta = (Bitmask, BitmaskEnum = ENDamageType))
		int32 DamageShieldAbsorption;

	/**
	*	Revoke Damage Types from the Suit's EnergyShield that were granted by other modules.
	*   Use this to remove suit damage types when a certain module is installed.
	*   Always takes precedence over DamageShieldAbsorption.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, meta = (Bitmask, BitmaskEnum = ENDamageType))
		int32 RemoveDamageShieldAbsorption;

	/**
	* Boolean flags for general suit abilities such as being able to enter flight mode.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, meta = (Bitmask, BitmaskEnum = ESuitFlag))
		int32 SuitFlags;

	/**
	* Unset the values that could have been set in SuitFlags by other modules.
	* Use this to remove suit abilities when a certain module is installed.
	* Always takes precedence over SuitFlags.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, meta = (Bitmask, BitmaskEnum = ESuitFlag))
		int32 RemoveSuitFlags;

	/*
	* Additional slots in the suit's inventory.
	* Not tested with much modules, only with the suit's built in module.
	* Seems to work with modules but need re equip? and might delete modules on change size.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame, Category = "EquipmentModule | Inventory")
		int32 InventorySlots;


	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Inventory")
	int32 mCachedInventorySlot = -1;

	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Inventory")
	APowerSuitModuleAttachment* mCachedAttachment;

	/**
	* Allow the suit to refill its Fuel by consuming these items from the inventory.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Inventory")
	TArray<TSubclassOf<class UFGItemDescriptor>> nUnlockedAllowedFuels;


	~FEquipmentStats() = default;

};