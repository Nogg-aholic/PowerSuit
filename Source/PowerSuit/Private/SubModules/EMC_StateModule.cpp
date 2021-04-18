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

	if (Parent->EquipmentParent->mCurrentHoverMode == EHoverPackMode::HPM_Hover || Parent->EquipmentParent->mCurrentHoverMode == EHoverPackMode::HPM_HoverSlowFall)
	{
		if (Parent->TKey_NoFriction && Parent->EquipmentParent->mHoverFriction != 0.0f)
		{
			Parent->EquipmentParent->mHoverFriction = 0.0f;
		}
		else if (!Parent->TKey_NoFriction)
		{
			Parent->EquipmentParent->mHoverFriction = Parent->GetFlightPropertySafe(ESuitFlightProperty::EFP_mHoverFriction).value();
		}
		if(Parent->EquipmentParent->mHoverFriction == 0.0f)
		{
			Parent->MoveC->GravityScale = 1.f;
			Parent->MoveC->Velocity.Z += Parent->MoveC->GetGravityZ() * Parent->Delta;
		}
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

void UEMC_StateModule::UpdateHotkeys()
{
	if (!Parent->EquipmentParent->GetInstigator())
		return;
	if (!Parent->EquipmentParent->GetInstigator()->GetController())
		return;

	if (Parent->EquipmentParent->GetInstigator()->IsLocallyControlled())
	{
		UE_LOG(LogTemp,Error,TEXT("KeyBinds Bound !"))
		FInputActionKeyMapping JumpKey = UFGInputLibrary::GetKeyMappingForAction(Cast<AFGPlayerController>(Parent->EquipmentParent->GetInstigator()->GetController()), "Jump_Drift", false);
		FInputActionKeyMapping AccelKey = UFGInputLibrary::GetKeyMappingForAction(Cast<AFGPlayerController>(Parent->EquipmentParent->GetInstigator()->GetController()), "ToggleSprint", false);
		FInputActionKeyMapping crouchKey = UFGInputLibrary::GetKeyMappingForAction(Cast<AFGPlayerController>(Parent->EquipmentParent->GetInstigator()->GetController()), "Crouch", false);
		KB_Up = JumpKey.Key;
		KB_Accel = AccelKey.Key;
		KB_Down = crouchKey.Key;
	}
};


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

		// Toggle Hotkey, controls Frictionif allowed
		if (Controller->WasInputKeyJustPressed(KB_Toggle))
		{
			if (Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasNoFrictionMode))
			{
				Parent->TKey_NoFriction = !Parent->TKey_NoFriction;
				if (!Controller->HasAuthority())
					Parent->RCO->ServerSetHovering(Parent, Parent->TKey_NoFriction);
			};
		}

		// Uphotkey mostly mapped to Space
		if (Controller->IsInputKeyDown(KB_Up) != HKey_Up)
		{


			HKey_Up = Controller->IsInputKeyDown(KB_Up);
			if (!Controller->HasAuthority())
				Parent->RCO->ServerSetHotKeyUpAccel(Parent, HKey_Up);

			if (Parent->EquipmentParent)
			{
				Parent->EquipmentParent->OnPlayAirBreakSound();
			}
		}


		if (Controller->IsInputKeyDown(KB_Up))
		{
			if (Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked))
			{
				if (!Parent->TKey_Fly && (Parent->TKey_Fly_Pressed > Parent->GetFlightPropertySafe(ESuitFlightProperty::EFP_mJumpKeyHoldActivationTime).value() || Controller->IsInputKeyDown(EKeys::LeftControl)))
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
		if (Controller->IsInputKeyDown(KB_Down) != HKey_Down)
		{
			HKey_Down = Controller->IsInputKeyDown(KB_Down);
			if (!Controller->HasAuthority())
				Parent->RCO->ServerSetHotKeyDownAccel(Parent, HKey_Down);
		}
		// Accel Hotkey , this is Sprinting key mostly shift
		if (Controller->IsInputKeyDown(KB_Accel) != HKey_Accel)
		{
			HKey_Accel = Controller->IsInputKeyDown(KB_Accel);
			if (!Controller->HasAuthority())
				Parent->RCO->ServerSetHotKeyDirectionalAccel(Parent, HKey_Accel);
		}
		// DeAccel Hotkey
		if (Controller->IsInputKeyDown(KB_Breaks) != HKey_Breaks)
		{
			HKey_Breaks = Controller->IsInputKeyDown(KB_Breaks);
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

	if (!Parent->PowerModule->Producing)
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
			if ((HKey_Accel || HKey_Down) && Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasZiplineAccel)) // Is pipe accelerating or decelerating
				Parent->SuitState = EPowerSuitState::PS_ZIPLINE_SPRINT;
			else
				Parent->SuitState = EPowerSuitState::PS_ZIPLINE;
		}
		else if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_Hover || Parent->nCustomMovementMode == ECustomMovementMode::CMM_HoverSlowFall)
		{
			bool canFly = Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked);
			// if WantsToFly is disabled we assume we aborted flight 
			if (Parent->TKey_Fly && canFly)
			{
				if (HKey_Accel)
					Parent->SuitState = EPowerSuitState::PS_HOVERSPRINT;
				else if (HKey_Up)
					Parent->SuitState = EPowerSuitState::PS_FLYUP;
				else if (HKey_Down)
					Parent->SuitState = EPowerSuitState::PS_FLYDOWN;
				else if (Parent->MoveC->Velocity.Size2D() > 0.1f)
					Parent->SuitState = EPowerSuitState::PS_HOVERMOVE;
				else
					Parent->SuitState = EPowerSuitState::PS_HOVER;
			}
			else if (canFly)
			{
				Parent->MoveC->CustomMovementMode = uint8(ECustomMovementMode::CMM_HoverSlowFall);
				Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_HoverSlowFall);
				Parent->SuitState = EPowerSuitState::PS_SLOWFALL;
			}
			else
			{
				Parent->MoveC->CustomMovementMode = uint8(ECustomMovementMode::CMM_None);
				Parent->MoveC->MovementMode = EMovementMode::MOVE_Falling;
				Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Inactive);
				Parent->SuitState = EPowerSuitState::PS_FALLING;
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
				if (HKey_Down && Parent->PowerModule->CurrentPower > 0.2f)
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
	else if (Parent->nMovementMode == EMovementMode::MOVE_Falling || Parent->nMovementMode == EMovementMode::MOVE_Flying)
	{
		// if we want to fly and are in the Air we allow if Unlocked
		if (Parent->TKey_Fly && Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked) && Parent->PowerModule->CurrentPower > 0.1f)
		{
			Parent->MoveC->CustomMovementMode = uint8(ECustomMovementMode::CMM_Hover);
			Parent->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Hover);
			Parent->SuitState = EPowerSuitState::PS_HOVER;
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
		UE_LOG(LogTemp, Verbose, TEXT("What State is this ? %s"), *FString::FromInt(int32(Parent->nMovementMode)));
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
			Parent->EquipmentParent->OnMoveRep(uint8(Parent->nMovementMode));
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
	Parent->EquipmentParent->mCurrentPowerLevel = FMath::Clamp(Parent->PowerModule->CurrentPower, 0.f, Parent->EquipmentParent->mPowerCapacity);
	if (!Parent->EquipmentParent->HasAuthority())
	{
		int32 Value = int32(Parent->EquipmentParent->mCurrentHoverMode);
		UE_LOG(LogTemp, Error, TEXT("Remote ! Hover modo changu ? %i"), Value);
		Parent->RCO->ServerUpdateCurrentHoverMode(Parent,Parent->EquipmentParent->mCurrentHoverMode);
	}
}
