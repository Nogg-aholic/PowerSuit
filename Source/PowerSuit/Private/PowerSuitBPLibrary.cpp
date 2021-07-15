// Copyright Epic Games, Inc. All Rights Reserved.

#include "PowerSuitBPLibrary.h"
#include "Equipment/Equipment_PowerSuit.h"
#include "EquipmentModuleComponent.h"
#include "SubModules/EMC_HealthModule.h"
#include "SubModules/EMC_PowerModule.h"

#include "FGCharacterMovementComponent.h"

#include "PowerSuit.h"
#include "Patching/BlueprintHookHelper.h"
#include "Patching/BlueprintHookManager.h"
#include "Patching/NativeHookManager.h"

UPowerSuitBPLibrary::UPowerSuitBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool UPowerSuitBPLibrary::EnableHover(UEquipmentModuleComponent* Component)
{
	if (!Component)
		return false;

	if (Component->TKey_Fly && Component->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked) && Component->nProducing)
	{
		Component->MoveC->CustomMovementMode = uint8(ECustomMovementMode::CMM_Hover);
		Component->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Hover);
	}
	return Component->TKey_Fly;
};


// Reset momvement component to player defaults
void UPowerSuitBPLibrary::ResetMovementComponent(UFGCharacterMovementComponent* MovementComponent, bool Notify)
{
	// this was added because of vehicle logic etc , TODO: Confirm this is actually needed & add vehicle movement stats etc
	if (!MovementComponent)
		return;

	UFGCharacterMovementComponent* MovementCDO = Cast< UFGCharacterMovementComponent>(MovementComponent->GetClass()->GetDefaultObject());

	MovementComponent->JumpZVelocity = MovementCDO->JumpZVelocity;//500.f;
	MovementComponent->JumpOffJumpZFactor = MovementCDO->JumpOffJumpZFactor;//0.5f;
	MovementComponent->GroundFriction = MovementCDO->GroundFriction;//8.0f;
	MovementComponent->MaxWalkSpeed = MovementCDO->MaxWalkSpeed;//500.0f;
	MovementComponent->MaxWalkSpeedCrouched = MovementCDO->MaxWalkSpeedCrouched;//240.0f;
	MovementComponent->MaxSwimSpeed = MovementCDO->MaxSwimSpeed;//300.0f;
	MovementComponent->BrakingFrictionFactor = MovementCDO->BrakingFrictionFactor;//2.f;
	MovementComponent->BrakingFriction = MovementCDO->BrakingFriction;
	MovementComponent->BrakingDecelerationWalking = MovementCDO->BrakingDecelerationWalking;// 2048.0;
	MovementComponent->BrakingDecelerationSwimming = MovementCDO->BrakingDecelerationSwimming;//100.0;
	MovementComponent->BrakingDecelerationFalling = MovementCDO->BrakingDecelerationFalling;//0;
	MovementComponent->FallingLateralFriction = MovementCDO->FallingLateralFriction;//0;
	MovementComponent->GravityScale = MovementCDO->GravityScale;// 1.2f;
	MovementComponent->mClimbSpeed = MovementCDO->mClimbSpeed;// 500.f;
	MovementComponent->mMaxSprintSpeed = MovementCDO->mMaxSprintSpeed;//900.f;
#ifdef FOR_MODSHIPPING
	MovementComponent->mMaxSlideAngle = MovementCDO->mMaxSlideAngle;//1.64999997615814f;
	MovementComponent->mBoostJumpZMultiplier = MovementCDO->mBoostJumpZMultiplier;// 1.5f;
	MovementComponent->mBoostJumpVelocityMultiplier = MovementCDO->mBoostJumpVelocityMultiplier;// 1.29999995231628f;
	MovementComponent->mSprintMinDotResult = MovementCDO->mSprintMinDotResult;// 0.75;
	MovementComponent->mJumpOffLadderVelocity = MovementCDO->mJumpOffLadderVelocity;// 300;
	MovementComponent->mBoostJumpTimeWindow = MovementCDO->mBoostJumpTimeWindow;// 0.150000005960464;
	MovementComponent->mZiplineSpeed = MovementCDO->mZiplineSpeed;// 1200;
	MovementComponent->mZiplineCorrectionSpeedMultiplier = MovementCDO->mZiplineCorrectionSpeedMultiplier; //ESMC_mZiplineCorrectionSpeedMultiplier
	MovementComponent->mZiplineVelocityInterpolationSpeed = MovementCDO->mZiplineVelocityInterpolationSpeed;// 1
	MovementComponent->mZiplineSpeedMultiplierUp = MovementCDO->mZiplineSpeedMultiplierUp;// 0.5;
	MovementComponent->mZiplineSpeedMultiplierDown = MovementCDO->mZiplineSpeedMultiplierDown;//1.29999995231628;
#else

	MovementComponent->*get(steal_mMaxSlideAngle()) = MovementCDO->*get(steal_mMaxSlideAngle());//1.64999997615814f;
	MovementComponent->*get(steal_mBoostJumpZMultiplier()) = MovementCDO->*get(steal_mBoostJumpZMultiplier());// 1.5f;
	MovementComponent->*get(steal_mBoostJumpVelocityMultiplier()) = MovementCDO->*get(steal_mBoostJumpVelocityMultiplier());// 1.29999995231628f;
	MovementComponent->*get(steal_mBoostJumpTimeWindow()) = MovementCDO->*get(steal_mBoostJumpTimeWindow());// 0.150000005960464;
	MovementComponent->*get(steal_mZiplineSpeed()) = MovementCDO->*get(steal_mZiplineSpeed());// 1200;
	MovementComponent->*get(steal_mZiplineCorrectionSpeedMultiplier()) = MovementCDO->*get(steal_mZiplineCorrectionSpeedMultiplier()); //ESMC_mZiplineCorrectionSpeedMultiplier
	MovementComponent->*get(steal_mZiplineVelocityInterpolationSpeed()) = MovementCDO->*get(steal_mZiplineVelocityInterpolationSpeed());// 1
	MovementComponent->*get(steal_mZiplineSpeedMultiplierUp()) = MovementCDO->*get(steal_mZiplineSpeedMultiplierUp());// 0.5;
	MovementComponent->*get(steal_mZiplineSpeedMultiplierDown()) = MovementCDO->*get(steal_mZiplineSpeedMultiplierDown());//1.29999995231628;

#endif

}


void UPowerSuitBPLibrary::UpdateMovementComponent(APowerSuit * EquipmentParent, bool Notify)
{
	if (!EquipmentParent)
		return;
	// this was added because of vehicle logic etc , TODO: Confirm this is actually needed & add vehicle movement stats etc
	UFGCharacterMovementComponent* MovementComponent = EquipmentParent->Module->MoveC;

	if (!MovementComponent)
		return;

	ResetMovementComponent(MovementComponent);

	EquipmentParent->Module->nPowerCapacity = FMath::Clamp(EquipmentParent->Module->GetSuitPropertySafe(ESuitProperty::nPowerCapacity).value(),0.0001f,999999999.f);

	// Setup component properties factoring in the defaults for the player character
	MovementComponent->JumpZVelocity += EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_JumpZVelocity).value(); // ESMC_JumpZVelocity
	MovementComponent->JumpOffJumpZFactor += EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_JumpZVelocity).value(); // ESMC_JumpOffJumpZFactor
	MovementComponent->GravityScale +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_GravityScale).value(); // ESMC_GravityScale
#ifdef FOR_MODSHIPPING

	MovementComponent->mBoostJumpZMultiplier +=  EquipmentParent->Module->GetMovementPropertySafe(ESMC_mBoostJumpZMultiplier).value(); // ESMC_mBoostJumpZMultiplier
	MovementComponent->mBoostJumpVelocityMultiplier +=  EquipmentParent->Module->GetMovementPropertySafe(ESMC_mBoostJumpVelocityMultiplier).value(); // ESMC_mBoostJumpVelocityMultiplier
#else
	MovementComponent->*get(steal_mBoostJumpZMultiplier()) +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mBoostJumpZMultiplier).value(); // ESMC_mBoostJumpZMultiplier 
	MovementComponent->*get(steal_mBoostJumpVelocityMultiplier()) +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mBoostJumpVelocityMultiplier).value(); // ESMC_mBoostJumpVelocityMultiplier
#endif
	MovementComponent->GroundFriction +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_GroundFriction).value(); // ESMC_GroundFriction
	MovementComponent->MaxWalkSpeed +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_MaxWalkSpeed).value(); //ESMC_MaxWalkSpeed
	MovementComponent->MaxWalkSpeedCrouched +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_MaxWalkSpeedCrouched).value(); // ESMC_MaxWalkSpeedCrouched
	MovementComponent->MaxSwimSpeed +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_MaxSwimSpeed).value(); //ESMC_MaxSwimSpeed
	MovementComponent->BrakingFrictionFactor +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_BrakingFrictionFactor).value(); //	ESMC_BrakingFrictionFactor 
	MovementComponent->BrakingFriction +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_BrakingFriction).value(); //ESMC_BrakingFriction
	MovementComponent->BrakingDecelerationWalking +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_BrakingDecelerationWalking).value(); //ESMC_BrakingDecelerationWalking
	MovementComponent->BrakingDecelerationFalling +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_BrakingDecelerationFalling).value(); //ESMC_BrakingDecelerationSwimming
	MovementComponent->BrakingDecelerationSwimming +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_BrakingDecelerationSwimming).value(); //ESMC_BrakingDecelerationFalling
	MovementComponent->FallingLateralFriction +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_FallingLateralFriction).value(); // ESMC_FallingLateralFriction


	MovementComponent->mClimbSpeed +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mClimbSpeed).value(); // ESMC_mClimbSpeed
	MovementComponent->mMaxSprintSpeed +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mMaxSprintSpeed).value(); //ESMC_mMaxSprintSpeed
	//  Tick overrides ref->mMaxSlideAngle = Stats.mMaxSlideAngle + 1.64999997615814f; // ESMC_mMaxSlideAngle
#ifdef FOR_MODSHIPPING

	MovementComponent->mBoostJumpTimeWindow +=  EquipmentParent->Module->GetMovementPropertySafe(ESMC_mBoostJumpTimeWindow).value(); //ESMC_mBoostJumpTimeWindow

	MovementComponent->mZiplineSpeed +=  EquipmentParent->Module->GetMovementPropertySafe(ESMC_mZiplineSpeed).value(); // ESMC_mZiplineSpeed
	MovementComponent->mZiplineCorrectionSpeedMultiplier +=  EquipmentParent->Module->GetMovementPropertySafe(ESMC_mZiplineCorrectionSpeedMultiplier).value(); //ESMC_mZiplineCorrectionSpeedMultiplier
	MovementComponent->mZiplineVelocityInterpolationSpeed +=  EquipmentParent->Module->GetMovementPropertySafe(ESMC_mZiplineVelocityInterpolationSpeed).value();
	MovementComponent->mZiplineSpeedMultiplierUp +=  EquipmentParent->Module->GetMovementPropertySafe(ESMC_mZiplineSpeedMultiplierUp).value();
	MovementComponent->mZiplineSpeedMultiplierDown +=  EquipmentParent->Module->GetMovementPropertySafe(ESMC_mZiplineSpeedMultiplierDown).value();
#else
	MovementComponent->*get(steal_mBoostJumpTimeWindow()) +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mBoostJumpTimeWindow).value(); //ESMC_mBoostJumpTimeWindow
	MovementComponent->*get(steal_mZiplineSpeed()) +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mZiplineSpeed).value(); // ESMC_mZiplineSpeed
	MovementComponent->*get(steal_mZiplineCorrectionSpeedMultiplier()) +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mZiplineCorrectionSpeedMultiplier).value(); //ESMC_mZiplineCorrectionSpeedMultiplier
	MovementComponent->*get(steal_mZiplineVelocityInterpolationSpeed()) +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mZiplineVelocityInterpolationSpeed).value();
	MovementComponent->*get(steal_mZiplineSpeedMultiplierUp()) +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mZiplineSpeedMultiplierUp).value();
	MovementComponent->*get(steal_mZiplineSpeedMultiplierDown()) +=  EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mZiplineSpeedMultiplierDown).value();
#endif


	// Setup component properties factoring in the defaults for the player character
	MovementComponent->JumpZVelocity *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_JumpZVelocity).ClampMult(); // ESMC_JumpZVelocity
	MovementComponent->JumpOffJumpZFactor *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_JumpZVelocity).ClampMult(); // ESMC_JumpOffJumpZFactor
	MovementComponent->GravityScale *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_GravityScale).ClampMult(); // ESMC_GravityScale
#ifdef FOR_MODSHIPPING

	MovementComponent->mBoostJumpZMultiplier *= EquipmentParent->Module->GetMovementPropertySafe(ESMC_mBoostJumpZMultiplier).ClampMult(); // ESMC_mBoostJumpZMultiplier
	MovementComponent->mBoostJumpVelocityMultiplier *= EquipmentParent->Module->GetMovementPropertySafe(ESMC_mBoostJumpVelocityMultiplier).ClampMult(); // ESMC_mBoostJumpVelocityMultiplier
#else
	MovementComponent->*get(steal_mBoostJumpZMultiplier()) *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mBoostJumpZMultiplier).ClampMult(); // ESMC_mBoostJumpZMultiplier 
	MovementComponent->*get(steal_mBoostJumpVelocityMultiplier()) *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mBoostJumpVelocityMultiplier).ClampMult(); // ESMC_mBoostJumpVelocityMultiplier
#endif
	MovementComponent->GroundFriction *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_GroundFriction).ClampMult(); // ESMC_GroundFriction
	MovementComponent->MaxWalkSpeed *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_MaxWalkSpeed).ClampMult(); //ESMC_MaxWalkSpeed
	MovementComponent->MaxWalkSpeedCrouched *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_MaxWalkSpeedCrouched).ClampMult(); // ESMC_MaxWalkSpeedCrouched
	MovementComponent->MaxSwimSpeed *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_MaxSwimSpeed).ClampMult(); //ESMC_MaxSwimSpeed
	MovementComponent->BrakingFrictionFactor *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_BrakingFrictionFactor).ClampMult(); //	ESMC_BrakingFrictionFactor 
	MovementComponent->BrakingFriction *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_BrakingFriction).ClampMult(); //ESMC_BrakingFriction
	MovementComponent->BrakingDecelerationWalking *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_BrakingDecelerationWalking).ClampMult(); //ESMC_BrakingDecelerationWalking
	MovementComponent->BrakingDecelerationFalling *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_BrakingDecelerationFalling).ClampMult(); //ESMC_BrakingDecelerationSwimming
	MovementComponent->BrakingDecelerationSwimming *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_BrakingDecelerationSwimming).ClampMult(); //ESMC_BrakingDecelerationFalling
	MovementComponent->FallingLateralFriction *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_FallingLateralFriction).ClampMult(); // ESMC_FallingLateralFriction


	MovementComponent->mClimbSpeed *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mClimbSpeed).ClampMult(); // ESMC_mClimbSpeed
	MovementComponent->mMaxSprintSpeed *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mMaxSprintSpeed).ClampMult(); //ESMC_mMaxSprintSpeed
	//  Tick overrides ref->mMaxSlideAngle = Stats.mMaxSlideAngle + 1.64999997615814f; // ESMC_mMaxSlideAngle
#ifdef FOR_MODSHIPPING

	MovementComponent->mBoostJumpTimeWindow *= EquipmentParent->Module->GetMovementPropertySafe(ESMC_mBoostJumpTimeWindow).ClampMult(); //ESMC_mBoostJumpTimeWindow

	MovementComponent->mZiplineSpeed *= EquipmentParent->Module->GetMovementPropertySafe(ESMC_mZiplineSpeed).ClampMult(); // ESMC_mZiplineSpeed
	MovementComponent->mZiplineCorrectionSpeedMultiplier *= EquipmentParent->Module->GetMovementPropertySafe(ESMC_mZiplineCorrectionSpeedMultiplier).ClampMult(); //ESMC_mZiplineCorrectionSpeedMultiplier
	MovementComponent->mZiplineVelocityInterpolationSpeed *= EquipmentParent->Module->GetMovementPropertySafe(ESMC_mZiplineVelocityInterpolationSpeed).ClampMult();
	MovementComponent->mZiplineSpeedMultiplierUp *= EquipmentParent->Module->GetMovementPropertySafe(ESMC_mZiplineSpeedMultiplierUp).ClampMult();
	MovementComponent->mZiplineSpeedMultiplierDown *= EquipmentParent->Module->GetMovementPropertySafe(ESMC_mZiplineSpeedMultiplierDown).ClampMult();
#else
	MovementComponent->*get(steal_mBoostJumpTimeWindow()) *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mBoostJumpTimeWindow).ClampMult(); //ESMC_mBoostJumpTimeWindow
	MovementComponent->*get(steal_mZiplineSpeed()) *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mZiplineSpeed).ClampMult(); // ESMC_mZiplineSpeed
	MovementComponent->*get(steal_mZiplineCorrectionSpeedMultiplier()) *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mZiplineCorrectionSpeedMultiplier).ClampMult(); //ESMC_mZiplineCorrectionSpeedMultiplier
	MovementComponent->*get(steal_mZiplineVelocityInterpolationSpeed()) *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mZiplineVelocityInterpolationSpeed).ClampMult();
	MovementComponent->*get(steal_mZiplineSpeedMultiplierUp()) *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mZiplineSpeedMultiplierUp).ClampMult();
	MovementComponent->*get(steal_mZiplineSpeedMultiplierDown()) *= EquipmentParent->Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mZiplineSpeedMultiplierDown).ClampMult();
#endif

	if(Notify)
		EquipmentParent->OnPowerSuitStatUpdate.Broadcast(0);

}



void UPowerSuitBPLibrary::ResetFlightStats(APowerSuit * EquipmentParent, bool Notify) {
	if (!EquipmentParent)
		return;
	// this was added because of vehicle logic etc , TODO: Confirm this is actually needed & add vehicle movement stats etc
		
	APowerSuit* MovementCDO = Cast< APowerSuit>(EquipmentParent->GetClass()->GetDefaultObject());
#ifdef FOR_MODSHIPPING

	EquipmentParent->mHoverSpeed = MovementCDO->mHoverSpeed;
	EquipmentParent->mHoverAccelerationSpeed = MovementCDO->mHoverAccelerationSpeed;
	EquipmentParent->mHoverSprintMultiplier = MovementCDO->mHoverSprintMultiplier;
	EquipmentParent->mRailRoadSurfSpeed = MovementCDO->mRailRoadSurfSpeed;
	EquipmentParent->mRailroadSurfSensitivity = MovementCDO->mRailroadSurfSensitivity;
	EquipmentParent->mHoverFriction = MovementCDO->mHoverFriction;
	EquipmentParent->mJumpKeyHoldActivationTime = MovementCDO->mJumpKeyHoldActivationTime;
	EquipmentParent->mFallSpeedLimitWhenPowered = MovementCDO->mFallSpeedLimitWhenPowered;
	EquipmentParent->mPowerConnectionSearchRadius = MovementCDO->mPowerConnectionSearchRadius;
	EquipmentParent->mPowerConnectionSearchTickRate = MovementCDO->mPowerConnectionSearchTickRate;
	EquipmentParent->mPowerConnectionDisconnectionTime = MovementCDO->mPowerConnectionDisconnectionTime;
	EquipmentParent->mCrouchHoverCancelTime = MovementCDO->mCrouchHoverCancelTime;
	EquipmentParent->mCharacterUseDistanceWhenActive = MovementCDO->mCharacterUseDistanceWhenActive;
	EquipmentParent->mPowerDrainRate = MovementCDO->mPowerDrainRate;
	EquipmentParent->mPowerConsumption = MovementCDO->mPowerConsumption;
	EquipmentParent->mPowerCapacity = MovementCDO->mPowerCapacity;
#else
	EquipmentParent->*get(steal_mHoverSpeed()) = MovementCDO->*get(steal_mHoverSpeed());
	EquipmentParent->*get(steal_mHoverAccelerationSpeed()) = MovementCDO->*get(steal_mHoverAccelerationSpeed());
	EquipmentParent->*get(steal_mHoverSprintMultiplier()) = MovementCDO->*get(steal_mHoverSprintMultiplier());
	EquipmentParent->*get(steal_mRailRoadSurfSpeed()) = MovementCDO->*get(steal_mRailRoadSurfSpeed());
	EquipmentParent->*get(steal_mRailroadSurfSensitivity()) = MovementCDO->*get(steal_mRailroadSurfSensitivity());
	EquipmentParent->*get(steal_mHoverFriction()) = MovementCDO->*get(steal_mHoverFriction());
	EquipmentParent->*get(steal_mJumpKeyHoldActivationTime()) = MovementCDO->*get(steal_mJumpKeyHoldActivationTime());
	EquipmentParent->*get(steal_mFallSpeedLimitWhenPowered()) = MovementCDO->*get(steal_mFallSpeedLimitWhenPowered());
	EquipmentParent->*get(steal_mPowerConnectionSearchRadius()) = MovementCDO->*get(steal_mPowerConnectionSearchRadius());
	EquipmentParent->*get(steal_mPowerConnectionSearchTickRate()) = MovementCDO->*get(steal_mPowerConnectionSearchTickRate());
	EquipmentParent->*get(steal_mPowerConnectionDisconnectionTime()) = MovementCDO->*get(steal_mPowerConnectionDisconnectionTime());
	EquipmentParent->*get(steal_mCrouchHoverCancelTime()) = MovementCDO->*get(steal_mCrouchHoverCancelTime());
	EquipmentParent->*get(steal_mCharacterUseDistanceWhenActive()) = MovementCDO->*get(steal_mCharacterUseDistanceWhenActive());
	EquipmentParent->*get(steal_mPowerDrainRate()) = MovementCDO->*get(steal_mPowerDrainRate());
	EquipmentParent->*get(steal_mPowerConsumption()) = MovementCDO->*get(steal_mPowerConsumption());
	EquipmentParent->*get(steal_mPowerCapacity()) = MovementCDO->*get(steal_mPowerCapacity());
#endif

	if (Notify)
		EquipmentParent->OnPowerSuitStatUpdate.Broadcast(1);
}


void UPowerSuitBPLibrary::UpdateFlightStats(APowerSuit * EquipmentParent, bool Notify) {
	if (!EquipmentParent)
		return;
	ResetFlightStats(EquipmentParent);
#ifdef FOR_MODSHIPPING

	EquipmentParent->mHoverSpeed +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverSpeed).value();
	EquipmentParent->mHoverAccelerationSpeed +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverAccelerationSpeed).value();
	EquipmentParent->mHoverSprintMultiplier +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverSprintMultiplier).value();
	EquipmentParent->mRailRoadSurfSpeed +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mRailRoadSurfSpeed).value();
	EquipmentParent->mRailroadSurfSensitivity +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mRailroadSurfSensitivity).value();
	EquipmentParent->mHoverFriction +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverFriction).value();
	EquipmentParent->mJumpKeyHoldActivationTime +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mJumpKeyHoldActivationTime).value();
	EquipmentParent->mFallSpeedLimitWhenPowered +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mFallSpeedLimitWhenPowered).value();
	EquipmentParent->mPowerConnectionSearchRadius +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionSearchRadius).value();
	EquipmentParent->mPowerConnectionSearchTickRate +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionSearchTickRate).value();
	EquipmentParent->mPowerConnectionDisconnectionTime +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionDisconnectionTime).value();
	EquipmentParent->mCrouchHoverCancelTime +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mCrouchHoverCancelTime).value();
	EquipmentParent->mCharacterUseDistanceWhenActive +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mCharacterUseDistanceWhenActive).value();
	EquipmentParent->mPowerDrainRate +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerDrainRate).value();
	EquipmentParent->mPowerConsumption +=  EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConsumption).value();
	EquipmentParent->mPowerCapacity += EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerCapacity).value();
#else


	EquipmentParent->*get(steal_mHoverSpeed()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverSpeed).value();
	EquipmentParent->*get(steal_mHoverAccelerationSpeed()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverAccelerationSpeed).value();
	EquipmentParent->*get(steal_mHoverSprintMultiplier()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverSprintMultiplier).value();
	EquipmentParent->*get(steal_mRailRoadSurfSpeed()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mRailRoadSurfSpeed).value();
	EquipmentParent->*get(steal_mRailroadSurfSensitivity()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mRailroadSurfSensitivity).value();
	EquipmentParent->*get(steal_mHoverFriction()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverFriction).value();
	EquipmentParent->*get(steal_mJumpKeyHoldActivationTime()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mJumpKeyHoldActivationTime).value();
	EquipmentParent->*get(steal_mFallSpeedLimitWhenPowered()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mFallSpeedLimitWhenPowered).value();
	EquipmentParent->*get(steal_mPowerConnectionSearchRadius()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionSearchRadius).value();
	EquipmentParent->*get(steal_mPowerConnectionSearchTickRate()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionSearchTickRate).value();
	EquipmentParent->*get(steal_mPowerConnectionDisconnectionTime()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionDisconnectionTime).value();
	EquipmentParent->*get(steal_mCrouchHoverCancelTime()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mCrouchHoverCancelTime).value();
	EquipmentParent->*get(steal_mCharacterUseDistanceWhenActive()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mCharacterUseDistanceWhenActive).value();
	EquipmentParent->*get(steal_mPowerDrainRate()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerDrainRate).value();
	EquipmentParent->*get(steal_mPowerConsumption()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConsumption).value();
	EquipmentParent->*get(steal_mPowerCapacity()) += EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerCapacity).value();
#endif



#ifdef FOR_MODSHIPPING

	EquipmentParent->mHoverSpeed *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverSpeed).ClampMult();
	EquipmentParent->mHoverAccelerationSpeed *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverAccelerationSpeed).ClampMult();
	EquipmentParent->mHoverSprintMultiplier *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverSprintMultiplier).ClampMult();
	EquipmentParent->mRailRoadSurfSpeed *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mRailRoadSurfSpeed).ClampMult();
	EquipmentParent->mRailroadSurfSensitivity *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mRailroadSurfSensitivity).ClampMult();
	EquipmentParent->mHoverFriction *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverFriction).ClampMult();
	EquipmentParent->mJumpKeyHoldActivationTime *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mJumpKeyHoldActivationTime).ClampMult();
	EquipmentParent->mFallSpeedLimitWhenPowered *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mFallSpeedLimitWhenPowered).ClampMult();
	EquipmentParent->mPowerConnectionSearchRadius *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionSearchRadius).ClampMult();
	EquipmentParent->mPowerConnectionSearchTickRate *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionSearchTickRate).ClampMult();
	EquipmentParent->mPowerConnectionDisconnectionTime *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionDisconnectionTime).ClampMult();
	EquipmentParent->mCrouchHoverCancelTime *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mCrouchHoverCancelTime).ClampMult();
	EquipmentParent->mCharacterUseDistanceWhenActive *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mCharacterUseDistanceWhenActive).ClampMult();
	EquipmentParent->mPowerDrainRate *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerDrainRate).ClampMult();
	EquipmentParent->mPowerConsumption *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConsumption).ClampMult();
	EquipmentParent->mPowerCapacity *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerCapacity).ClampMult();
#else


	EquipmentParent->*get(steal_mHoverSpeed()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverSpeed).ClampMult();
	EquipmentParent->*get(steal_mHoverAccelerationSpeed()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverAccelerationSpeed).ClampMult();
	EquipmentParent->*get(steal_mHoverSprintMultiplier()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverSprintMultiplier).ClampMult();
	EquipmentParent->*get(steal_mRailRoadSurfSpeed()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mRailRoadSurfSpeed).ClampMult();
	EquipmentParent->*get(steal_mRailroadSurfSensitivity()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mRailroadSurfSensitivity).ClampMult();
	EquipmentParent->*get(steal_mHoverFriction()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mHoverFriction).ClampMult();
	EquipmentParent->*get(steal_mJumpKeyHoldActivationTime()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mJumpKeyHoldActivationTime).ClampMult();
	EquipmentParent->*get(steal_mFallSpeedLimitWhenPowered()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mFallSpeedLimitWhenPowered).ClampMult();
	EquipmentParent->*get(steal_mPowerConnectionSearchRadius()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionSearchRadius).ClampMult();
	EquipmentParent->*get(steal_mPowerConnectionSearchTickRate()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionSearchTickRate).ClampMult();
	EquipmentParent->*get(steal_mPowerConnectionDisconnectionTime()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConnectionDisconnectionTime).ClampMult();
	EquipmentParent->*get(steal_mCrouchHoverCancelTime()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mCrouchHoverCancelTime).ClampMult();
	EquipmentParent->*get(steal_mCharacterUseDistanceWhenActive()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mCharacterUseDistanceWhenActive).ClampMult();
	EquipmentParent->*get(steal_mPowerDrainRate()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerDrainRate).ClampMult();
	EquipmentParent->*get(steal_mPowerConsumption()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerConsumption).ClampMult();
	EquipmentParent->*get(steal_mPowerCapacity()) *= EquipmentParent->Module->GetFlightPropertySafe(EFP_mPowerCapacity).ClampMult();
#endif

	if(Notify)
		EquipmentParent->OnPowerSuitStatUpdate.Broadcast(1);

}

void UPowerSuitBPLibrary::UpdateFlags(APowerSuit* EquipmentParent, bool Notify )
{
	for (int32 i = 1; i < ENDamageType::DamageTypeMAX; i = static_cast<ENDamageType>(i + i))
	{
		if (EquipmentParent->Module->Stats.HasRemoveDamageMask(i))
			if ((EquipmentParent->Module->Stats.HasDamageMask(i)))
				EquipmentParent->Module->Stats.RemoveDamageShieldAbsorption -= static_cast<ENDamageType>(i);
	}

	for (int32 i = 1; i < ESuitFlag::SuitFlag_END; i = static_cast<ESuitFlag>(i + i))
	{
		if (EquipmentParent->Module->Stats.HasRemoveFlag(i))
			if ((EquipmentParent->Module->Stats.HasFlag(i)))
				EquipmentParent->Module->Stats.RemoveSuitFlags -= static_cast<ESuitFlag>(i);
	}

	for (int32 i = 1; i < ESuitFlagAdvanced::SuitFlagAdvanced_END; i = static_cast<ESuitFlagAdvanced>(i + i))
	{
		if (EquipmentParent->Module->Stats.HasAdvancedRemoveFlag(i))
			if ((EquipmentParent->Module->Stats.HasAdvancedFlag(i)))
				EquipmentParent->Module->Stats.RemoveSuitFlagsAdvanced -= static_cast<ESuitFlagAdvanced>(i);
	}

	if(Notify)
		EquipmentParent->OnPowerSuitStatUpdate.Broadcast(3);

}


void UPowerSuitBPLibrary::UpdateInventorySize(APowerSuit* EquipmentParent, bool Notify)
{
	if (!EquipmentParent)
		return;
	if (!EquipmentParent->HasAuthority())
		return;

	// Size is loaded so skip setting it if Size is the same  
	if (EquipmentParent->Module->nInventory->GetSizeLinear() != EquipmentParent->Module->Stats.InventorySlots)
		EquipmentParent->Module->nInventory->Resize(EquipmentParent->Module->Stats.InventorySlots);

	// Limit Slots to SlotSize of 1
	// Modules may stack but they must be Single when accounted for
	for (int32 i = 0; i < EquipmentParent->Module->Stats.InventorySlots; i++)
		EquipmentParent->Module->nInventory->AddArbitrarySlotSize(i, 1);

	if(Notify)
		EquipmentParent->OnPowerSuitStatUpdate.Broadcast(2);

}





void UPowerSuitBPLibrary::UpdateAllNoRefresh(APowerSuit* EquipmentParent, bool Notify)
{
	UE_LOG(PowerSuit_Log, Error, TEXT("UpdateAllNoRefresh"));
	UpdateInventorySize(EquipmentParent);
	UpdateFlags(EquipmentParent);
	UpdateFlightStats(EquipmentParent);
	UpdateMovementComponent(EquipmentParent);
	EquipmentParent->Module->HealthModule->SetMaxHealth();
	EquipmentParent->OnPowerSuitStatUpdate.Broadcast(0);
}



void UPowerSuitBPLibrary::SetSuitMovementProperty(FEquipmentStats& Stats, ESuitMovementProperty Enum, FModMultProperty Property)
{
	if (Enum >= ESuitMovementProperty::ESMC_JumpZVelocity && Enum <= ESuitMovementProperty::ESMC_MAX)
	{
		Stats.nMovementProperties.Add(Enum, Property);
	}
}

void UPowerSuitBPLibrary::SetSuitFlightProperty(FEquipmentStats& Stats, ESuitFlightProperty Enum, FModMultProperty Property)
{
	if (Enum >= ESuitFlightProperty::EFP_mHoverSpeed && Enum <= ESuitFlightProperty::EFP_MAX)
	{
		Stats.nFlightProperties.Add(Enum, Property);
	}
}

void UPowerSuitBPLibrary::SetSuitProperty(FEquipmentStats& Stats, ESuitProperty Enum, FModMultProperty Property)
{
	if (Enum >= ESuitProperty::nPowerCapacity && Enum <= ESuitProperty::SuitPropertyMAX)
	{
		Stats.nSuitProperties.Add(Enum, Property);
	}
}

void UPowerSuitBPLibrary::SetSuitPropertyNamed(FEquipmentStats& Stats, FName Prop, FModMultProperty Property)
{
	if (Prop != FName())
	{
		Stats.nNamedProperties.Add(Prop, Property);
	}
}

void UPowerSuitBPLibrary::SetPropertyGeneral(FEquipmentStats& Stats, EEquipmentStatsProperty PropertyType, uint8 Index, FModMultProperty Property)
{

	switch (PropertyType)
	{
	case EEquipmentStatsProperty::ESSP_nSuitProperties:
	{
		SetSuitProperty(Stats,static_cast<ESuitProperty>(Index), Property);
		break;
	}
	case EEquipmentStatsProperty::ESSP_nMovementProperties:
	{
		SetSuitMovementProperty(Stats, static_cast<ESuitMovementProperty>(Index), Property);
		break;
	}
	case EEquipmentStatsProperty::ESSP_nFlightProperties:
	{
		SetSuitFlightProperty(Stats, static_cast<ESuitFlightProperty>(Index), Property);
		break;
	}
	default:
		break;
	}

}

void UPowerSuitBPLibrary::SetSuitFlag(FEquipmentStats& Stats, ESuitFlag Flag, bool Enabled)
{
	const bool Has = Stats.HasFlag(Flag);
	if (Has && Enabled)
		return;
	else if (!Has && Enabled)
	{
		Stats.SuitFlags += Flag;
	}
	else if (Has && !Enabled)
	{
		Stats.SuitFlags -= Flag;
	}
}



void UPowerSuitBPLibrary::BindOnWidgetConstruct(const TSubclassOf<UUserWidget> WidgetClass,FOnWidgetConstruct Binding) {
	if(!WidgetClass)
		return;
	UFunction* ConstructFunction = WidgetClass->FindFunctionByName(TEXT("Construct"));
	if (!ConstructFunction || ConstructFunction->IsNative())
	{
		return;
	}
	UBlueprintHookManager* HookManager = GEngine->GetEngineSubsystem<UBlueprintHookManager>();
	HookManager->HookBlueprintFunction(ConstructFunction, [Binding](FBlueprintHookHelper& HookHelper) {
		Binding.ExecuteIfBound(Cast<UUserWidget>(HookHelper.GetContext()));
	}, EPredefinedHookOffset::Return);
}

void UPowerSuitBPLibrary::BindOnActorConstruct(const TSubclassOf<AActor> ActorClass,FOnObjectConstruct Binding) {
	if(!ActorClass)
		return;
	UFunction* ConstructFunction = ActorClass->FindFunctionByName(TEXT("Construct"));
	if(!ConstructFunction)
		return;
	UBlueprintHookManager* HookManager = GEngine->GetEngineSubsystem<UBlueprintHookManager>();
	HookManager->HookBlueprintFunction(ConstructFunction, [Binding](FBlueprintHookHelper& HookHelper) {
        Binding.ExecuteIfBound(Cast<AActor>(HookHelper.GetContext()));
    }, EPredefinedHookOffset::Return);
}

void UPowerSuitBPLibrary::SetUseDistanceOnPlayer(AFGCharacterPlayer* Player, float Distance)
{
	if(Player)
	{
#ifdef FOR_MODSHIPPING
		Player->mUseDistance = Distance;
#else
		float & Mod = Player->*get(steal_mUseDistance());
		Mod = Distance;
#endif
	}
}

void UPowerSuitBPLibrary::SetInnerConnection(APowerSuit * Suit, UFGPowerConnectionComponent* Connection)
{
	if (!Suit)
		return;
#ifdef FOR_MODSHIPPING
	Suit->mCurrentPowerConnection() = Connection;
	if (Suit->mCurrentPowerConnection)
		Suit->mHasConnection = true;
	else
		Suit->mHasConnection = false; 
#else
	Suit->*get(steal_mCurrentPowerConnection()) = Connection;

	if (Suit->*get(steal_mCurrentPowerConnection()))
		Suit->*get(steal_mHasConnection()) = true;
	else
		Suit->*get(steal_mHasConnection()) = false;
#endif


}

void UPowerSuitBPLibrary::UpdateInnerConnectionRange(APowerSuit* Suit)
{
#ifdef FOR_MODSHIPPING
	if (Suit->Module->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked))
	{
		Suit->mCurrentConnectionLocation = Suit->GetActorLocation();
		if (Suit->InnerBattery && !Suit->mHasConnection())
		{
			SetInnerConnection(Suit, Suit->InnerBattery);
		}
	}
	else
	{
		if (Suit->mHasConnection() && Suit->mCurrentPowerConnection == Suit->InnerBattery)
		{
			SetInnerConnection(Suit, nullptr);
		}
	}
#else
	if (Suit->Module->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked))
	{
		FVector& Mod = Suit->*get(steal_mCurrentConnectionLocation());
		Mod = Suit->GetInstigator()->GetActorLocation();
		bool& State = Suit->*get(steal_mHasConnection());
		if (Suit->InnerBattery && !State)
		{
			SetInnerConnection(Suit, Suit->InnerBattery);
		}
	}
	else
	{
		bool& State = Suit->*get(steal_mHasConnection());

		if (State && Suit->*get(steal_mCurrentPowerConnection()) == Suit->InnerBattery)
		{
			SetInnerConnection(Suit, nullptr);
		}
	}

#endif
}
