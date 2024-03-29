#pragma once

#include "CoreMinimal.h"
#include "FactoryGame.h"
#include "Equipment/FGEquipment.h"
#include "Equipment/FGHoverPack.h"
#include "FGInventoryComponent.h"
#include "FGCharacterMovementComponent.h"
#include "FGCharacterPlayer.h"
#include "FGHealthComponent.h"
#include "Misc/AssertionMacros.h"
#include "DamageTypes/FGDamageType.h"
#include "PowerSuitData.generated.h"

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

	struct steal_mCostToUse {
		typedef TArray< FItemAmount > AFGEquipment::* type;
		friend type get(steal_mCostToUse);
	};

	struct steal_mUseDistance {
		typedef float AFGCharacterPlayer::* type;
		friend type get(steal_mUseDistance);
	};

	struct steal_mHasConnection {
		typedef bool AFGHoverPack::* type;
		friend type get(steal_mHasConnection);
	};

	struct steal_mCurrentPowerConnection {
		typedef UFGPowerConnectionComponent* AFGHoverPack::* type;
		friend type get(steal_mCurrentPowerConnection);
	};

	struct steal_mCurrentConnectionLocation {
		typedef FVector AFGHoverPack::* type;
		friend type get(steal_mCurrentConnectionLocation);
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

	template struct Steal<steal_mCostToUse, &AFGEquipment::mCostToUse>;
	template struct Steal<steal_mUseDistance, &AFGCharacterPlayer::mUseDistance>;
	template struct Steal<steal_mHasConnection, &AFGHoverPack::mHasConnection>;
	template struct Steal<steal_mCurrentPowerConnection, &AFGHoverPack::mCurrentPowerConnection>;
	template struct Steal<steal_mCurrentConnectionLocation, &AFGHoverPack::mCurrentConnectionLocation>;

	
	
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
	PS_HOVER UMETA(Displayname = "Hovering", Tooltip = "Hovering and not pressing any movement keys. Never entered with gravity flight. The hover equivalent of the Flight Falling state."),
	PS_FLYUP UMETA(Displayname = "Flying Up", Tooltip = "Either hovering or flying and pressing the spacebar to ascend."),
	PS_FLYDOWN	UMETA(Displayname = "Flying Down", Tooltip = "Either hovering or flying and pressing descend. This speed seems to be capped at terminal velocity of ~41m/s."),
	PS_HOVERMOVE UMETA(Displayname = "Flying & Moving", Tooltip = "Either hovering or flying and actively pressing a movement key."),
	PS_HOVERSPRINT UMETA(Displayname = "Flying & Boosting", Tooltip = "Either hovering or flying, pressing a walking key, and pressing the boost key."),
	PS_PIPEHYPER_RIDE UMETA(Displayname = "HyperTube Riding", Tooltip = "Riding in a hypertube. NOTE: This state can't have speed modified as of 0.3.0, but you can still apply power/fuel costs to it."),
	PS_PIPEHYPER_SPRINT UMETA(Displayname = "HyperTube Boosting", Tooltip = "Actively accelerating or decelerating in a hypertube."),
	PS_FALLING UMETA(Displayname = "Falling", Tooltip = "Falling. Also when no keys are pressed for Gravity flight."),
	PS_SLIDING UMETA(Displayname = "Sliding", Tooltip = "Sliding and not Boosting."),
	PS_POWERSLIDE UMETA(Displayname = "Power Sliding", Tooltip = "Sliding with Boost."),
	PS_ZIPLINE UMETA(Displayname = "Zipline Riding", Tooltip = "Riding on a Zipline"),
	PS_ZIPLINE_SPRINT UMETA(Displayname = "Zipline Riding Boost", Tooltip = "Riding on a Zipline with Boost or Brakes(?) key."),
	PS_REBOOTING UMETA(Displayname = "Rebooting", Tooltip = "Rebooting"),
	PS_SLOWFALL UMETA(Displayname = "SlowFall", Tooltip = "Base game Hoverpack out of power state, the suit should never be in this state"),
	PS_FLYUPSPRINT	UMETA(Displayname = "Flying Up & Boosting", Tooltip = "Ascent key with Boost key"),
	PS_FLYDOWNSPRINT	UMETA(Displayname = "Flying Down & Boosting", Tooltip = "Descent key with Boost key"),
	PS_FLYUPDIAGONAL	UMETA(Displayname = "Flying Up Diagonal", Tooltip = "Ascent key and movement key"),
	PS_FLYDOWNDIAGONAL	UMETA(Displayname = "Flying Down Diagonal", Tooltip = "Descent key and movement key"),
	PS_FLYUPDIAGONALSPRINT	UMETA(Displayname = "Flying Up Diagonal & Boosting", Tooltip = "Ascent key and movement key, with Boost key"),
	PS_FLYDOWNDIAGONALSPRINT	UMETA(Displayname = "Flying Down Diagonal & Boosting", Tooltip = "Descent key and movement key, with Boost key"),
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

UENUM(Blueprinttype, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum ESuitFlag
{
	SuitFlag_Null = 0x00 UMETA(hidden, Displayname = "Invalid value!", Tooltip = "Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change."),
	SuitFlag_HasPipeAccel = 1 << 0 UMETA(Tooltip = "Allows hotkey-bound accelerating and decelerating in hypertubes"),
	SuitFlag_HasFlightUnlocked = 1 << 1 UMETA(Tooltip = "Allows flight. Be sure to also set flight speed properties and such"),
	SuitFlag_FuseNeverBreaksWhenFueled = 1 << 2 UMETA(Tooltip = "If the suit has fuel available, using too much power will never result in the suit fuse breaking. You should probably write custom logic for what to do when this is the case."),
	SuitFlag_AllowsToggleGravityMode = 1 << 3 UMETA(Tooltip = "HoverPack's flight has no Gravity (results in hover flight) - can Gravity be enabled by the user with the mod's keybind?"),
	SuitFlag_ForceGravityMode = 1 << 4 UMETA(Tooltip = "Force the Suit to use Gravity Mode on equip. The suit has Hover Flight by default, but this overwrites toggle"),
	SuitFlag_HasAirReFuel = 1 << 5 UMETA(Tooltip = "Allow refilling the fuel tank while in flight-related movement states"),
	SuitFlag_HasInfiniteSlide = 1 << 6 UMETA(Tooltip = "Allows the user to slide forever as long as the angle is acceptable (continually resets their slide timer"),
	SuitFlag_HasNoFrictionMode = 1 << 7 UMETA(Displayname = "Suit Flag Allows Toggle Friction Mode", Tooltip = "Allows user to use the mod's Frictionless Flight Toggle keybind. Switches between suit's calculated value (no hud icon), or 0 (hud icon shown)"),
	SuitFlag_END = 1 << 8 UMETA(hidden, Displayname = "Invalid value!", Tooltip = "Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change.")

};

UENUM(Blueprinttype, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum ESuitFlagAdvanced
{
	SuitFlagAdvanced_Null = 0x00 UMETA(hidden, Displayname = "Invalid value!", Tooltip = "Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change."),
	SuitFlagAdvanced_NoRefuel = 1 << 0 UMETA(Tooltip = "With this on, the suit will not follow its normal refueling logic (eat item at 0 to full fill). It is up to a custom implementation to manage refueling instead."),
	SuitFlagAdvanced_IgnoreBelts = 1 << 1 UMETA(Tooltip = "Smart Belt Immunity, belts can't move the player except if the player is boosting and looking in that direction. Also see source for available named properties to boost speed."),
	SuitFlagAdvanced_ZipLineAccel = 1 << 2 UMETA(Tooltip = "Allow boost/brakes key to be used on ziplines and put into the relevant movestate"),
	SuitFlagAdvanced_AlwaysWantsFuel = 1 << 3 UMETA(Tooltip = "Affects Fuel Fuse logic. When set, the suit is always considered as having fuel consumption even if there isn't any right now -> suit will Fuel Fuse break when out of fuel."),
	SuitFlagAdvanced_ExitPipesEarly = 1 << 4 UMETA(Tooltip = "Allow exiting hypertubes early by flying with thrust flight"),
	SuitFlagAdvanced_Temp5 = 1 << 5 UMETA(Tooltip = "Placeholder for future implementation"),
	SuitFlagAdvanced_Temp6 = 1 << 6 UMETA(Tooltip = "Placeholder for future implementation"),
	SuitFlagAdvanced_Temp7 = 1 << 7 UMETA(Tooltip = "Placeholder for future implementation"),
	SuitFlagAdvanced_END = 1 << 8 UMETA(hidden, Displayname = "Invalid value!", Tooltip = "Invalid value entry; this was probably caused by an update to PowerSuit that caused something's name to change.")

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
	* The time in seconds the Suit needs to reboot after blowing the fuse (when out of power for too long).
	*	Additive
	*	Positive for increased time
	*/
	nFuseTime UMETA(Displayname = "nFuseTime"),

	/**
	* The time in seconds the Suit keeps Fuel-consuming modules disabled for when out of Fuel.
	* If this is too low, immense lag will be caused as the suit frequently recalculates stats.
	* Be very careful with offering anything that allows this.
	* Minimum final value for this is 1 second as a safety measure
	*	Additive
	*	Positive for increased time
	*/
	nFuelFuseTime UMETA(Displayname = "nFuelFuseTime"),

	/**
	* The 'grace period' in seconds the Suit has for being out of power before it blows the fuse and has to reboot.
	* The suit has free effectively infinite power for this time
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
	* TODO update this comment with new behavior for refueling. No longer seems to be 1-based.
	* 
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
	* How much Fuel (in MW seconds) can the fuel tank hold?
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
	/** How much power is drained per second when using the slow fall mode. */
	EFP_mPowerCapacity UMETA(Displayname = "mPowerCapacity"),
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
	* Additive multiplier for the property. This number is first added to 1.0, then all Multiplier values are summed, then multiplied with the sum of the Modifiers of base.
	* End result value is (Base + SumOfModifiers) * SumOfMultipliers
	* Note that Multipliers will apply to the base game values for stats as well, not just the Modifier.
	    Ex. walk speed Mult -1.0f means your walking speed becomes 0% of base + Mod -> you can't walk regardless of Mod
	* 
	* Examples:
	*	0.0f	->	No change
	*	0.1f	->	+10%
	*	1.0f	->	+100%
	*	-0.1f	->	-10%
	*	-1.0f	->	-100% Keep in mind that this can still be counteracted with positive Multipliers - this doesn't necessarily mean "permanent set to zero"
	*	-10.0f	->	-1000% Same as -1.0f if there is nothing else adding positive Multipliers.
	*/
	UPROPERTY(EditDefaultsOnly, SaveGame, BlueprintReadWrite)
		float Multiplier = 0;

	/**
	* Additive modifier for the property. This number is summed with the other Modifiers before multiplication with Multipliers.
	* End result value is (Base + SumOfModifiers) * SumOfMultipliers
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
		Modifier += Input.Modifier;
		Multiplier += Input.Multiplier;
		return *this;
	}
	FModMultProperty operator-(const FModMultProperty& Input) {
		Modifier -= Input.Modifier;
		Multiplier -= Input.Multiplier;
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

	void ForgetUnlockedFuels(UEquipmentModuleComponent* Parent, bool IsDefault);

	FEquipmentStats operator+(const FEquipmentStats& OtherStruct);

	FEquipmentStats operator-(const FEquipmentStats& OtherStruct);

	bool HasFlag(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ESuitFlag")) int32 Bitmask) const;;
	bool HasRemoveFlag(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ESuitFlag")) int32 Bitmask) const;;

	bool HasDamageMask(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ENDamageType")) int32 Bitmask) const;;
	bool HasRemoveDamageMask(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ENDamageType")) int32 Bitmask) const;;

	bool HasAdvancedFlag(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ESuitFlagAdvanced")) int32 Bitmask) const;;
	bool HasAdvancedRemoveFlag(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ESuitFlagAdvanced")) int32 Bitmask) const;;

	/**
	* Settings that change base numeric properties of the suit
	* These ones are related to the player's movement on the ground, when falling, in hypertubes, water, etc.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame, Category = "EquipmentModule")
		TMap< TEnumAsByte<ESuitMovementProperty>, FModMultProperty> nMovementProperties;

	/**
	* Settings that change base numeric properties of the suit
	* These ones are specifically from the Hover Pack, which the PowerSuit equipment extends.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame, Category = "EquipmentModule")
		TMap< TEnumAsByte<ESuitFlightProperty>, FModMultProperty> nFlightProperties;

	/**
	* Settings that change base numeric properties of the suit
	* These ones are specific to the suit itself, such as how much power and fuel it can store, shield capacity, etc.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame, Category = "EquipmentModule")
		TMap< TEnumAsByte<ESuitProperty>, FModMultProperty> nSuitProperties;

	/**
	* Named Custom Properties
	* Have no immediate effect on the suit unless you implement it yourself in attachment or custom suit logic.
	* Consider using this when the Suit doesn't already implement a value you can use.
	* 
	* Trying to fetch them when not present will result in an blank ModMult property.
	* Although it is suggested that they follow normal mod/mult property rules, they don't have to.
	* It is probably wise to make sure that they have no effects when their value is the blank ModMult property.
	* 
	* Suggested name format: ModReference_PropertyName
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, SaveGame, Category = "EquipmentModule | Custom")
		TMap<FName, FModMultProperty> nNamedProperties;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ENDamageType"))
		int32 DamageShieldAbsorption;

	/**
	*	Revoke Damage Types from the Suit's EnergyShield that were granted by other modules.
	*   Use this to remove suit damage types when a certain module is installed.
	*   Always takes precedence over DamageShieldAbsorption.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ENDamageType"))
		int32 RemoveDamageShieldAbsorption;

	/**
	* Boolean flags for general suit abilities such as being able to enter flight mode.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ESuitFlag"))
		int32 SuitFlags;

	/**
	* Unset the values that could have been set in SuitFlags by other modules.
	* Use this to remove suit abilities when a certain module is installed.
	* Always takes precedence over SuitFlags.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ESuitFlag"))
		int32 RemoveSuitFlags;

	/**
	* Boolean flags for general suit abilities such as being able to enter flight mode.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ESuitFlagAdvanced"))
		int32 SuitFlagsAdvanced;

	/**
	* Boolean flags for general suit abilities such as being able to enter flight mode.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ESuitFlagAdvanced"))
		int32 RemoveSuitFlagsAdvanced;

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


	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Inventory")
		TSubclassOf<class UFGItemDescriptor> mCachedDescriptor = nullptr;

	/**
	* Allow the suit to refill its Fuel by consuming these items from the inventory.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Inventory")
	TArray<TSubclassOf<class UFGItemDescriptor>> nUnlockedAllowedFuels;
	/**
	* Allow the suit to refill its Fuel by consuming these items from the inventory.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Inventory")
		TArray<TSubclassOf<class UFGItemDescriptor>> nSkippedAllowedFuels;


	~FEquipmentStats() = default;

};