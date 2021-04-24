#include "SubModules/EMC_HyperTubeModule.h"
#include "SubModules/EMC_PowerModule.h"
#include "SubModules/EMC_StateModule.h"
#include "FGBuildablePipePart.h"
#include "buildables/FGBuildablePipeHyper.h"

// Server Only? this was previous on both, something is not replicated here and this helped to prevent stutter and death
void UEMC_HyperTubeModule::ModifyPipeSpeed()
{
	float speed = Parent->MoveC->GetPipeVelocity();
#ifdef FOR_MODSHIPPING
	FPlayerPipeHyperData & Data = Parent->MoveC->mPipeData;
#else
	FPlayerPipeHyperData & Data = Parent->MoveC->*get(steal_mPipeData());
#endif
	if (Cast<AFGBuildablePipeHyper>(Data.mTravelingPipeHyper))
	{
		float DistanceToLink = Cast<AFGBuildablePipeHyper>(Data.mTravelingPipeHyper)->mLength - Data.mPipeProgressReal;
		float DistanceFromLink = Data.mPipeProgressReal;

		if (DistanceToLink > 1000.f && FMath::Abs(DistanceFromLink) > 1000.f)
		{
			// need to create Speed variable for Pipes
			if (Parent->StateModule->HKey_Accel)
				Data.mPipeVelocityReal = FMath::Clamp((speed + ((speed * Parent->Delta))), -10000.f - Parent->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mMaxSprintSpeed).value(), 10000.f);
			else
				Data.mPipeVelocityReal = FMath::Clamp((speed - ((speed * Parent->Delta))), -10000.f, 10000.f);
			lastPipeVelocity = Data.mPipeVelocityReal;
			Data.mPipeVelocity = Data.mPipeVelocityReal;
		}
	}
}

// Hypertube velocity calculation
void UEMC_HyperTubeModule::HandlePipeVelocity()
{
	if (Parent->nCurrentPower > 0.05f)
	{
#ifdef FOR_MODSHIPPING
		FPlayerPipeHyperData & Data = Parent->MoveC->mPipeData;
#else
		FPlayerPipeHyperData & Data = Parent->MoveC->*get(steal_mPipeData());
#endif
		// We already know either HotKeyDirectionalAcceleration || HotKeyDownAcceleration are being pressed; handled in caller
		if (Parent->EquipmentParent->HasAuthority())
		{ // MP server side
			if (!UFGBlueprintFunctionLibrary::IsLocallyHumanControlled(Parent->EquipmentParent->GetInstigator()))
			{
				float speed = nPipeVelocity;
				Data.mPipeVelocityReal = nPipeVelocity;
				Data.mPipeVelocity = nPipeVelocity;
			}
			else
			{
				if (Parent->StateModule->HKey_Accel || Parent->StateModule->HKey_Down)
				{
					ModifyPipeSpeed();
				}
			}
		}
		else
		{ // MP Client side
			if (Parent->StateModule->HKey_Accel || Parent->StateModule->HKey_Down)
			{
				ModifyPipeSpeed();
			}
			//OnPipeVelocityChanged(mCachedMovementComponent->GetPipeVelocity());
		}
	}
}

void UEMC_HyperTubeModule::PreTick()
{
	if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_PipeHyper && Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasPipeAccel))
	{
		Parent->SuitState = EPowerSuitState::PS_PIPEHYPER_SPRINT;
		HandlePipeVelocity();
	}
}

void UEMC_HyperTubeModule::Tick()
{
	if (Parent->nCustomMovementMode == ECustomMovementMode::CMM_PipeHyper && Parent->Stats.HasFlag(ESuitFlag::SuitFlag_HasPipeAccel))
	{
		HandlePipeVelocity();
	}
}

void UEMC_HyperTubeModule::PostTick()
{
}