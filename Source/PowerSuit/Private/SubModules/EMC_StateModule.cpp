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
#ifdef FOR_MODSHIPPING
	float & Friction = Parent->EquipmentParent->mHoverFriction;
#else
	float & Friction = Parent->EquipmentParent->*get(steal_mHoverFriction());
#endif

	if (!Parent->TKey_NoGravity || Parent->Stats.HasFlag(ESuitFlag::SuitFlag_ForceGravityMode))
	{
		Parent->MoveC->GravityScale = 1.f;
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

	if (!Parent || !Parent->EquipmentParent || !Parent->EquipmentParent->GetInstigator())
		return;

	APlayerController* Controller = Cast< APlayerController >(Parent->EquipmentParent->GetInstigatorController());
	
	if (!Controller || !UFGBlueprintFunctionLibrary::IsLocallyHumanControlled(Parent->EquipmentParent->GetInstigator()) && Controller->HasAuthority())
	{	
		// server and not humanly controlled doesnt need to update Hotkeys
		return;
	}

	if (Controller)
	{

		// Toggle Hotkey, controls Gravity if allowed
		if (Controller->WasInputKeyJustPressed(Parent->KB_Toggle))
		{
			if (Parent->Stats.HasFlag(ESuitFlag::SuitFlag_AllowsToggleGravityMode))
			{
				Parent->TKey_NoGravity = !Parent->TKey_NoGravity;
				if (!Controller->HasAuthority())
					Parent->RCO->ServerSetGravityMode(Parent, Parent->TKey_NoGravity);
			};
		}

		// Toggle Hotkey, controls Flight Friction if allowed
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
		//if (Controller->IsInputKeyDown(Parent->KB_Up) != HKey_Up)
		if(Parent->MoveC->mIsPressingJump != HKey_Up)
		{
			HKey_Up = Parent->MoveC->mIsPressingJump;
			if (!Controller->HasAuthority())
				Parent->RCO->ServerSetHotKeyUpAccel(Parent, HKey_Up);

			if (Parent->EquipmentParent)
			{
				Parent->EquipmentParent->OnPlayAirBreakSound();
			}
		}


		if (Parent->MoveC->mIsPressingJump)
		{
			if (Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked))
			{
#ifdef FOR_MODSHIPPING
				const float& JumpHoldTime = Parent->EquipmentParent->mJumpKeyHoldActivationTime;
#else
				const float& JumpHoldTime = Parent->EquipmentParent->*get(steal_mJumpKeyHoldActivationTime());
#endif
				if (!Parent->TKey_Fly && (Parent->FallingTime > JumpHoldTime) ||
					(Controller->IsInputKeyDown(EKeys::LeftControl) && Parent->EquipmentParent->EnableCtrlSpaceStartFlight) )
				{

					if (Parent->EquipmentParent->GetInstigator()->HasAuthority())
					{
						Parent->FallingTime = 0;
						Parent->TKey_Fly = true;
					}
					else
					{
						Parent->FallingTime = 0;
						Parent->TKey_Fly = true;
						Parent->RCO->ServerSetFlying(Parent, true);
					}
				}
				else
				{
				}
			}
		}
		else {
			//if (Parent->TKey_Fly_Pressed != 0)
		}

		// DownHotkey mostly mapped to C (Crouch)
		if (Controller->IsInputKeyDown(Parent->KB_Down) != HKey_Down)
		{
			HKey_Down = Controller->IsInputKeyDown(Parent->KB_Down);
			LastDownPress = FDateTime::Now();
			if (!Controller->HasAuthority())
				Parent->RCO->ServerSetHotKeyDownAccel(Parent, HKey_Down);
		}
		// Accel Hotkey , this is Sprinting key mostly shift

		bool WantsSprint = false;
		if (Parent->MoveC->mIsSprinting || Parent->MoveC->mWantsToSprint)
			WantsSprint = true;
		bool AccelKeyDown = false;
		if (Controller->IsInputKeyDown(Parent->KB_Accel))
			AccelKeyDown = true;
		bool WantsAccel = false;
		if (WantsSprint || AccelKeyDown)
		{
			WantsAccel = true;
		}
		else
		{
			WantsAccel = false;
		}
		if (HKey_Accel != WantsAccel)
		{
			HKey_Accel = WantsAccel;
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
	if (!Parent || !Parent->EquipmentParent || !Parent->EquipmentParent->GetInstigator())
		return;
	const bool Controlled = UFGBlueprintFunctionLibrary::IsLocallyHumanControlled(Parent->EquipmentParent->GetInstigator());
	if (!Parent->EquipmentParent->HasAuthority())
	{
		if (!Controlled)
		{
			// Remote and not humanly controlled doesnt need to update Hotkeys
			return;
		}
	}
	const auto Before = Parent->SuitState;
	if (!Parent->nProducing)
	{
		if(Parent->EquipmentParent->HasAuthority())
		{
			Parent->TKey_Fly = false;
			Parent->TKey_NoGravity = false;
			Parent->TKey_NoFriction = false;	
		}
		if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_Hover || Parent->nCustomMovementMode == ECustomMovementMode::CMM_HoverSlowFall)
		{
			Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Inactive);
			Parent->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_None);
			Parent->MoveC->MovementMode = EMovementMode::MOVE_Falling;
			UE_LOG(PowerSuit_Log, Display, TEXT("Disabled Flight about to Reboot!"));
		}
	}
	else if (Parent->nMovementMode == EMovementMode::MOVE_Custom)
	{
		if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_PipeHyper || Parent->MoveC->IsInHyperPipe())
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
			if ((HKey_Accel || HKey_Down) && Parent->Stats.HasAdvancedFlag(ESuitFlagAdvanced::SuitFlagAdvanced_ZipLineAccel)) // Is pipe accelerating or decelerating
				Parent->SuitState = EPowerSuitState::PS_ZIPLINE_SPRINT;
			else
				Parent->SuitState = EPowerSuitState::PS_ZIPLINE;
		}
		else if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_HoverSlowFall && !Parent->TKey_NoGravity)
		{
			Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Inactive);
			if(Parent->EquipmentParent->HasAuthority())
			{
				Parent->SuitState = EPowerSuitState::PS_FALLING;
				Parent->TKey_Fly = false;
			}
			Parent->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_None);
			Parent->MoveC->MovementMode = EMovementMode::MOVE_Falling;
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
				
					if(Parent->EquipmentParent->HasAuthority())
					{
						Parent->SuitState = EPowerSuitState::PS_FALLING;
						Parent->TKey_Fly = false;
					}
					Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Inactive);
					Parent->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_None);
                    Parent->MoveC->MovementMode = EMovementMode::MOVE_Falling;
					UE_LOG(PowerSuit_Log, Display, TEXT("Disabled Flight - FlyUpKey Released"));
				}
				else if(Parent->EquipmentParent->HasAuthority())
				{
					
					if (HKey_Up && HKey_Accel)
					{
						if (Parent->MoveC->Velocity.Size2D() > 0.1f)
						{
							Parent->SuitState = EPowerSuitState::PS_FLYUPDIAGONALSPRINT;
						}
						else
							Parent->SuitState = EPowerSuitState::PS_FLYUPSPRINT;
					}
					else if (HKey_Up)
					{
						if (Parent->MoveC->Velocity.Size2D() > 0.1f)
						{
							Parent->SuitState = EPowerSuitState::PS_FLYUPDIAGONAL;
						}
						else
						Parent->SuitState = EPowerSuitState::PS_FLYUP;
					}
					else if (HKey_Down && HKey_Accel)
					{
						if (Parent->MoveC->Velocity.Size2D() > 0.1f)
						{
							Parent->SuitState = EPowerSuitState::PS_FLYDOWNDIAGONALSPRINT;
						}
						else
							Parent->SuitState = EPowerSuitState::PS_FLYDOWNSPRINT;
					}
					else if (HKey_Down)
					{
						if (Parent->MoveC->Velocity.Size2D() > 0.1f)
						{
							Parent->SuitState = EPowerSuitState::PS_FLYDOWNDIAGONAL;
						}
						else
							Parent->SuitState = EPowerSuitState::PS_FLYDOWN;
					}
					else if (HKey_Accel && Parent->MoveC->Velocity.Size2D() > 0.1f)
						Parent->SuitState = EPowerSuitState::PS_HOVERSPRINT;
					else if (Parent->MoveC->Velocity.Size2D() > 0.1f)
						Parent->SuitState = EPowerSuitState::PS_HOVERMOVE;
					else
						Parent->SuitState = EPowerSuitState::PS_HOVER;
				}
			}
			else
			{
				if(Parent->EquipmentParent->HasAuthority())
				{
					Parent->SuitState = EPowerSuitState::PS_FALLING;
					Parent->TKey_Fly = false;
				}
				Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Inactive);
				Parent->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_None);
				Parent->MoveC->MovementMode = EMovementMode::MOVE_Falling;
				UE_LOG(PowerSuit_Log, Display, TEXT("Disabled Flight, cannot fly or doesnt want to"));
			}
		}
		else if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_Ladder)
		{
			Parent->SuitState = EPowerSuitState::PS_WALKING;
		}
	}
	else if (Parent->nMovementMode == EMovementMode::MOVE_Walking)
	{
		// deactivate wanting to fly if we landed, cannot fly on ground
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
		if(Controlled)
			Parent->FallingTime = Parent->FallingTime + Parent->LastDeltaTime;

		// if we want to fly and are in the Air we allow if Unlocked
		if (Parent->TKey_Fly && Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked) && Parent->nCurrentPower > 0.1f &&  !Parent->MoveC->IsInHyperPipe())
		{
			if(!(!Parent->TKey_NoGravity && !HKey_Up))
			{
				if (FTimespan(FDateTime::Now() - LastDownPress).GetTotalSeconds() > 0.15f)
				{
					if(Parent->EquipmentParent->HasAuthority())
					{
						Parent->SuitState = EPowerSuitState::PS_HOVER;
					}
					Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Hover);
					Parent->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_Hover);
					Parent->MoveC->MovementMode = EMovementMode::MOVE_Custom;
					UE_LOG(PowerSuit_Log, Display, TEXT("Enabled Hover mode"));
				}
				else
				{
					UE_LOG(PowerSuit_Log, Display, TEXT("Ignored Enabled Hover mode, assuming Touble Tap"));
					if (Parent->EquipmentParent->GetInstigator()->HasAuthority())
					{
						Parent->TKey_Fly = false;
					}
					else
					{
						Parent->TKey_Fly = false;
						Parent->RCO->ServerSetFlying(Parent, false);
					}
				}
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
		//UE_LOG(PowerSuit_Log, Error, TEXT("What State is this ? %s"), *FString::FromInt(static_cast<int32>(Parent->nMovementMode)));
	}

	if (Before != Parent->SuitState)
	{
		Parent->EquipmentParent->OnSuitStateUpdate.Broadcast(Before,Parent->SuitState);
		if (
			Parent->SuitState == EPowerSuitState::PS_FLYUP 
			|| Parent->SuitState == EPowerSuitState::PS_FLYDOWN 
			|| Parent->SuitState == EPowerSuitState::PS_FLYDOWNSPRINT 
			|| Parent->SuitState == EPowerSuitState::PS_FLYUPSPRINT 
			|| Parent->SuitState == EPowerSuitState::PS_HOVER 
			|| Parent->SuitState == EPowerSuitState::PS_HOVERMOVE 
			|| Parent->SuitState == EPowerSuitState::PS_HOVERSPRINT
			|| Parent->SuitState == EPowerSuitState::PS_FLYDOWNDIAGONALSPRINT
			|| Parent->SuitState == EPowerSuitState::PS_FLYUPDIAGONALSPRINT
			|| Parent->SuitState == EPowerSuitState::PS_FLYUPDIAGONAL
			|| Parent->SuitState == EPowerSuitState::PS_FLYDOWNDIAGONAL
			)
		{
			Parent->EquipmentParent->OnCharacterMovementModeChanged(Parent->MoveC->MovementMode, Parent->MoveC->CustomMovementMode, Parent->MoveC->MovementMode, Parent->MoveC->CustomMovementMode);
		}
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
	if (!Parent || !Parent->EquipmentParent || !Parent->EquipmentParent->GetInstigator())
		return;
	if(!Parent->PowerModule)
		return;
	Parent->EquipmentParent->OnCheckHotkeys();

	const bool Controlled = UFGBlueprintFunctionLibrary::IsLocallyHumanControlled(Parent->EquipmentParent->GetInstigator());
	
	// return here if we dont have a valid component 
	// can happen when using Trains maybe
	if (!Parent->MoveC)
		return;

	// deactivate wanting to fly if we landed, cannot fly on ground
	if (Parent->nMovementMode == EMovementMode::MOVE_Walking && Parent->TKey_Fly)
	{
		if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_Hover || Parent->nCustomMovementMode == ECustomMovementMode::CMM_HoverSlowFall)
		{
			UE_LOG(PowerSuit_Log, Display, TEXT("Ground Flight Disable"));
			Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Inactive,false);
			Parent->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_None);
			Parent->nMovementMode = EMovementMode::MOVE_Walking;
			//if (Controlled)
			//	Parent->EquipmentParent->OnFlyingChanged(false);
		}
		Parent->TKey_Fly = false;
	}

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
