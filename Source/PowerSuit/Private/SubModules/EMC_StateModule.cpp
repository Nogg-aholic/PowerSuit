#include "SubModules/EMC_StateModule.h"
#include "SubModules/EMC_PowerModule.h"
#include "SubModules/EMC_HyperTubeModule.h"

#include "Replication/PowerSuitRCO.h"


// Server and Client
void UEMC_StateModule::PreTick()
{
	// this should not be here but fine for now
	if (nIsSliding)
	{
		// set slide time states
		if (Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasInfiniteSlide))
		{
#ifdef FOR_MODSHIPPING
			Parent->MoveC->mSlideTime = 0.f;
#else
			Parent->MoveC->*get(steal_mSlideTime()) = 0.f;
#endif
		}
#ifdef FOR_MODSHIPPING
		Parent->MoveC->mMaxSlideAngle = 1.64999997615814f + Parent->GetMovementPropertySafe(ESMC_mMaxSlideAngle).value();
#else
		Parent->MoveC->*get(steal_mMaxSlideAngle()) = 1.64999997615814f + Parent->GetMovementPropertySafe(ESMC_mMaxSlideAngle).value();
#endif
	}
#ifdef MODDING_SHIPPING
	EHoverPackMode MM = Component->EquipmentParent->mCurrentHoverMode;
	float & Friction = Parent->EquipmentParent->mHoverFriction;

#else
	const EHoverPackMode MM = Parent->EquipmentParent->*get(steal_mCurrentHoverMode());
	float & Friction = Parent->EquipmentParent->*get(steal_mHoverFriction());
#endif
	if (MM == EHoverPackMode::HPM_Hover || MM == EHoverPackMode::HPM_HoverSlowFall)
	{
		if (!Parent->TKey_NoGravity || Parent->Stats.HasFlag(ESuitFlag::SuitFlag_ForceGravityMode))
		{
			Parent->MoveC->GravityScale = 1.f;
		}
	}

	if (Parent->TKey_NoFriction)
	{
		Friction = 0;
	}
	else
	{
		Friction = Parent->GetFlightPropertySafe(ESuitFlightProperty::EFP_mHoverFriction).value();
	}
}

void UEMC_StateModule::Tick()
{
	CheckHotkeys();
	UpdateSuitState();
	ReplicateStates();
}

void UEMC_StateModule::PostTick()
{

	
}



void UEMC_StateModule::CheckHotkeys()
{
	APlayerController* Controller = Cast< APlayerController >(Parent->EquipmentParent->GetInstigatorController());
	Parent->EquipmentParent;

	if (!UFGBlueprintFunctionLibrary::IsLocallyHumanControlled(Parent->EquipmentParent->GetInstigator()) && Controller->HasAuthority())
	{	
		// server and not humanly controlled doesnt need to update Hotkeys
		return;
	}

	if (Controller)
	{

		// Toggle Hotkey, controls Gravity if allowed
		if (Controller->WasInputKeyJustPressed(Parent->KB_Toggle))
		{
			if (Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasNoGravityMode))
			{
				Parent->TKey_NoGravity = !Parent->TKey_NoGravity;
				if (!Controller->HasAuthority())
					Parent->RCO->ServerSetGravityMode(Parent, Parent->TKey_NoGravity);
			};
		}

		// Toggle Hotkey, controls Gravity if allowed
		if (Controller->WasInputKeyJustPressed(Parent->KB_Toggle2))
		{
			if (Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasNoFrictionMode))
			{
				Parent->TKey_NoFriction = !Parent->TKey_NoFriction;
				if (!Controller->HasAuthority())
					Parent->RCO->ServerSetFrictionMode(Parent, Parent->TKey_NoFriction);
			}
		}

		// Toggle Hotkey, controls Gravity if allowed
		if (Controller->WasInputKeyJustPressed(Parent->KB_UI))
		{
			Parent->EquipmentParent->OnPowerSuitUIToggle.Broadcast(Parent->KB_UI);
		}
		// Uphotkey mostly mapped to Space
		if (Controller->IsInputKeyDown(Parent->KB_Up) != HKey_Up)
		{


			HKey_Up = Controller->IsInputKeyDown(Parent->KB_Up);
			if (!Controller->HasAuthority())
				Parent->RCO->ServerSetHotKeyUpAccel(Parent, HKey_Up);

			if (Parent->EquipmentParent)
			{
				Parent->EquipmentParent->OnPlayAirBreakSound();
			}
		}


		if (Controller->IsInputKeyDown(Parent->KB_Up))
		{
			if (Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked))
			{
#ifdef MODDING_SHIPPING
				const float& JumpHoldTime = Component->EquipmentParent->mJumpKeyHoldActivationTime;
#else
				const float& JumpHoldTime = Parent->EquipmentParent->*get(steal_mJumpKeyHoldActivationTime());
#endif
				if (!Parent->TKey_Fly && (Parent->TKey_Fly_Pressed > JumpHoldTime) || Controller->IsInputKeyDown(EKeys::LeftControl))
				{

					if (Parent->EquipmentParent->GetInstigator()->HasAuthority())
					{
						Parent->TKey_Fly = true;
					}
					else
					{
						Parent->TKey_Fly = true;
						Parent->RCO->ServerSetFlying(Parent, true);
					}
				}
				else
				{
					Parent->TKey_Fly_Pressed = Parent->TKey_Fly_Pressed + Parent->Delta;
				}
			}
		}
		else {
			if (Parent->TKey_Fly_Pressed != 0)
				Parent->TKey_Fly_Pressed = 0;
		}

		// DownHotkey mostly mapped to C (Crouch)
		if (Controller->IsInputKeyDown(Parent->KB_Down) != HKey_Down)
		{
			HKey_Down = Controller->IsInputKeyDown(Parent->KB_Down);
			if (!Controller->HasAuthority())
				Parent->RCO->ServerSetHotKeyDownAccel(Parent, HKey_Down);
		}
		// Accel Hotkey , this is Sprinting key mostly shift
		if (Controller->IsInputKeyDown(Parent->KB_Accel) != HKey_Accel)
		{
			HKey_Accel = Controller->IsInputKeyDown(Parent->KB_Accel);
			if (!Controller->HasAuthority())
				Parent->RCO->ServerSetHotKeyDirectionalAccel(Parent, HKey_Accel);
		}
		// DeAccel Hotkey
		if (Controller->IsInputKeyDown(Parent->KB_Breaks) != HKey_Breaks)
		{
			HKey_Breaks = Controller->IsInputKeyDown(Parent->KB_Breaks);
			if (!Controller->HasAuthority())
				Parent->RCO->ServerSetHotKeyDirectionalDeAccel(Parent, HKey_Breaks);
		}
	}
}


void UEMC_StateModule::UpdateSuitState()
{
	/*
		I know i hate this too, its confusing
		resolve Parent->SuitState by respecting if we are Producing
		not all state changes are directly replicated by the MovementComponent
		to fix this we use RCOs to replicate from Remote and use Parent->SuitState for all
		out Consumption/Production Logic
	*/
	// Deterministic solveable through RCO replicated Key inputs and serverside replicated properties
	if (!Parent->EquipmentParent->HasAuthority())
	{
		return;
	}

	if (!Parent->nProducing)
	{
		Parent->SuitState = EPowerSuitState::PS_REBOOTING;
	}
	else if (Parent->nMovementMode == EMovementMode::MOVE_Custom)
	{
		if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_PipeHyper)
		{
			// Accel and De-Accel both need Power and we use only 1 State 
			if ((HKey_Accel || HKey_Down) && Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasPipeAccel)) // Is pipe accelerating or decelerating
				Parent->SuitState = EPowerSuitState::PS_PIPEHYPER_SPRINT;
			else
				Parent->SuitState = EPowerSuitState::PS_PIPEHYPER_RIDE;
		}
		else if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_Zipline)
		{
			// Accel and De-Accel both need Power and we use only 1 State 
			if ((HKey_Accel || HKey_Down) ) // && Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasZiplineAccel)) // Is pipe accelerating or decelerating
				Parent->SuitState = EPowerSuitState::PS_ZIPLINE_SPRINT;
			else
				Parent->SuitState = EPowerSuitState::PS_ZIPLINE;
		}
		else if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_HoverSlowFall && !Parent->TKey_NoGravity)
		{
			Parent->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_None);
			Parent->MoveC->MovementMode = EMovementMode::MOVE_Falling;
			Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Inactive);
			Parent->SuitState = EPowerSuitState::PS_FALLING;
			UE_LOG(PowerSuit_Log, Display, TEXT("Disabled SlowFall"));
		}
		else if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_Hover || Parent->nCustomMovementMode == ECustomMovementMode::CMM_HoverSlowFall)
		{
			const bool bCanFly = Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked);
			// if WantsToFly is disabled we assume we aborted flight 
			if (Parent->TKey_Fly && bCanFly)
			{
				if (!Parent->TKey_NoGravity && !HKey_Up)
				{
					Parent->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_None);
					Parent->MoveC->MovementMode = EMovementMode::MOVE_Falling;
					Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Inactive);
					Parent->SuitState = EPowerSuitState::PS_FALLING;
					UE_LOG(PowerSuit_Log, Display, TEXT("Disabled Flight"));
				}
				else
				{
					if (HKey_Up)
						Parent->SuitState = EPowerSuitState::PS_FLYUP;
					else if (HKey_Down)
						Parent->SuitState = EPowerSuitState::PS_FLYDOWN;
					else if (HKey_Accel)
						Parent->SuitState = EPowerSuitState::PS_HOVERSPRINT;
					else if (Parent->MoveC->Velocity.Size2D() > 0.1f)
						Parent->SuitState = EPowerSuitState::PS_HOVERMOVE;
					else
						Parent->SuitState = EPowerSuitState::PS_HOVER;
				}
			}
			else if (bCanFly && Parent->TKey_NoGravity && Parent->nCustomMovementMode == ECustomMovementMode::CMM_Hover)
			{
				Parent->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_HoverSlowFall);
				Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_HoverSlowFall);
				Parent->SuitState = EPowerSuitState::PS_SLOWFALL;
				UE_LOG(PowerSuit_Log, Display, TEXT("Disabled SlowFall"));
			}
			else
			{
				Parent->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_None);
				Parent->MoveC->MovementMode = EMovementMode::MOVE_Falling;
				Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Inactive);
				Parent->SuitState = EPowerSuitState::PS_FALLING;
				UE_LOG(PowerSuit_Log, Display, TEXT("Disabled Flight"));
			}
		}
	}
	else if (Parent->nMovementMode == EMovementMode::MOVE_Walking)
	{
		if (Parent->MoveC->Velocity.Size2D() == 0)
			Parent->SuitState = EPowerSuitState::PS_STANDING;
		else
		{
			// this controls which speeds we will Pick 
			if (nIsSliding)
			{
				if (HKey_Down && Parent->nCurrentPower > 0.2f)
					Parent->SuitState = EPowerSuitState::PS_POWERSLIDE;
				else
					Parent->SuitState = EPowerSuitState::PS_SLIDING;
			}
			else
			{
				if (nIsSprinting)
					Parent->SuitState = EPowerSuitState::PS_SPRINTING;
				else
					Parent->SuitState = EPowerSuitState::PS_WALKING;
			}
		}
	}
	else if (Parent->nMovementMode == EMovementMode::MOVE_Falling)
	{
		// if we want to fly and are in the Air we allow if Unlocked
		if (Parent->TKey_Fly && Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked) && Parent->nCurrentPower > 0.1f)
		{
			if(!(!Parent->TKey_NoGravity && !HKey_Up))
			{
				Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Hover);
				Parent->SuitState = EPowerSuitState::PS_HOVER;
				Parent->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_Hover);
				Parent->MoveC->MovementMode = EMovementMode::MOVE_Custom;
				UE_LOG(PowerSuit_Log, Display, TEXT("Enabled Hover mode"));
			}
		}
		else
		{
			Parent->SuitState = EPowerSuitState::PS_FALLING;
		}
	}
	else if (Parent->nMovementMode == EMovementMode::MOVE_Swimming)
	{
		Parent->SuitState = EPowerSuitState::PS_WALKING;
	}
	else
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("What State is this ? %s"), *FString::FromInt(static_cast<int32>(Parent->nMovementMode)));
	}
}


void UEMC_StateModule::ReplicateStates()
{
	/*
		The purpose of this Function is to Notify the BP and EquipmentComponent about changes
		that effect the Components Beheavior.
		Since we Trust the Client with all his State Variables and some of them arent replicated
		we do this here manually from client -> Host
	*/
	if(!Parent->PowerModule)
		return;
	Parent->EquipmentParent->OnCheckHotkeys();

	const bool Controlled = UFGBlueprintFunctionLibrary::IsLocallyHumanControlled(Parent->EquipmentParent->GetInstigator());


	// deactivate wanting to fly if we landed, cannot fly on ground
	if (Parent->nMovementMode == EMovementMode::MOVE_Walking && Parent->TKey_Fly)
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("Ground Flight Disable"));
		Parent->TKey_Fly = false;
		if (Controlled)
			Parent->EquipmentParent->OnFlyingChanged(false);
	}

	// return here if we dont have a valid component 
	// can happen when using Trains maybe
	if (!Parent->MoveC)
		return;

	// we either aren on the Server or a Remote Client wanting to replicate here
	// Replicate mIsSprinting
	if (Parent->MoveC->mIsSprinting != nIsSprinting)
	{
		nIsSprinting = Parent->MoveC->mIsSprinting;
		if (Controlled)
			Parent->EquipmentParent->OnIsSprintingChanged(nIsSprinting);
	}
	// Replicate mIsSliding
	if (Parent->MoveC->mIsSliding != nIsSliding)
	{
		nIsSliding = Parent->MoveC->mIsSliding;
		if (Controlled)
			Parent->EquipmentParent->OnIsSlidingChanged(nIsSliding);
	}
	// Replicate MovementMode
	if (Parent->MoveC->MovementMode != Parent->nMovementMode)
	{
		Parent->nMovementMode = Parent->MoveC->MovementMode;
		HoverModeChange();
		if (Controlled)
			Parent->EquipmentParent->OnMoveRep(static_cast<uint8>(Parent->nMovementMode));
	}
	// Replicate CustomMovementMode
	if (Parent->MoveC->CustomMovementMode != Parent->nCustomMovementMode)
	{
		Parent->nCustomMovementMode = Parent->MoveC->CustomMovementMode;
		if (Controlled)
			Parent->EquipmentParent->OnCustomMovementMode(Parent->nCustomMovementMode);
	}
	
}


void UEMC_StateModule::HoverModeChange()
{
	if (!Parent->EquipmentParent)
	{
		// called too early
		return;
	}
	// when ever this changed The last FGHoverpack::Tick() did Reset PowerLevel to 1 or 0 
	// we dont want that so we overwrite it whenever we have this scenario 
#ifdef MODDING_SHIPPING
	EHoverPackMode & mCurrentHoverMode = Parent->EquipmentParent->mCurrentHoverMode;
	float & mCurrentPowerLevel_ = Parent->EquipmentParent->mCurrentPowerLevel;
	float & mPowerCapacity_ = Parent->EquipmentParent->mPowerCapacity;
#else
	EHoverPackMode & mCurrentHoverMode = Parent->EquipmentParent->*get(steal_mCurrentHoverMode());
	float & mCurrentPowerLevel_ = Parent->EquipmentParent->*get(steal_mCurrentPowerLevel());
	float & mPowerCapacity_ = Parent->EquipmentParent->*get(steal_mPowerCapacity());
#endif
	mCurrentPowerLevel_ = FMath::Clamp(Parent->nCurrentPower, 0.f, mPowerCapacity_);
	if (!Parent->EquipmentParent->HasAuthority())
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("Remote ! Hover modo changed  %i"), static_cast<int32>(mCurrentHoverMode));
		Parent->RCO->ServerUpdateCurrentHoverMode(Parent,mCurrentHoverMode);
	}
}
