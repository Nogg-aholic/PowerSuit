#include "SubModules/EMC_HyperTubeModule.h"
#include "SubModules/EMC_PowerModule.h"
#include "SubModules/EMC_StateModule.h"
#include "FGBuildablePipePart.h"
#include "buildables/FGBuildablePipeHyper.h"

// Server Only? this was previous on both, something is not replicated here and this helped to prevent stutter and death
void UEMC_HyperTubeModule::ModifyPipeSpeed()
{
	const float speed = Parent->MoveC->GetPipeVelocity();
#ifdef FOR_MODSHIPPING
	FPlayerPipeHyperData & Data = Parent->MoveC->mPipeData;
#else
	FPlayerPipeHyperData & Data = Parent->MoveC->*get(steal_mPipeData());
#endif
	if (Cast<AFGBuildablePipeHyper>(Data.mTravelingPipeHyper))
	{
		const float DistanceToLink = Cast<AFGBuildablePipeHyper>(Data.mTravelingPipeHyper)->mLength - Data.mPipeProgressReal;
		const float DistanceFromLink = Data.mPipeProgressReal;
		const float DeltaMovement = Parent->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mMaxSpeedInPipes).value() * Parent->LastDeltaTime;
		if (DistanceToLink > 1000.f - DeltaMovement && FMath::Abs(DistanceFromLink) > 1000.f + DeltaMovement)
		{
			// need to create Speed variable for Pipes
			if (Parent->StateModule->HKey_Accel)
				Data.mPipeVelocityReal = FMath::Clamp((speed + ((speed * Parent->LastDeltaTime))), -5000.f - Parent->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mMaxSpeedInPipes).value(), 5000.f + Parent->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mMaxSpeedInPipes).value());
			else
				Data.mPipeVelocityReal = FMath::Clamp((speed - ((speed * Parent->LastDeltaTime))), -100000.f, 100000.f);
			LastPipeVelocity = Data.mPipeVelocityReal;
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
	if (Parent->SuitState == EPowerSuitState::PS_PIPEHYPER_SPRINT)
	{
		HandlePipeVelocity();
	}
}

void UEMC_HyperTubeModule::Tick()
{
	
}

void UEMC_HyperTubeModule::PostTick()
{
}