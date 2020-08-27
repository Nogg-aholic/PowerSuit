// ILikeBanas


#include "PowerSuit.h"
#include "FGGameState.h"
#include "util/Logging.h"
#include "SML/mod/hooking.h"
#include "FGBuildableConveyorBelt.h"
#include "FGBuildablePipePart.h"
#include "FGBuildablePipeHyper.h"
#include "FGCharacterMovementComponent.h"
#include "FactoryGame.h"

APowerSuit::APowerSuit()
{
	Hovering = false;
	HotKeyUpAccelration = false;
	HotKeyDownAccelration = false;
	HotKeyDirectionalAccelration = false;
	FuelAmount = 1.f;
}


void APowerSuit::ReplicateStates()
{
	OnCheckHotkeys();
	if (!Instigator->HasAuthority())
	{
		if (nMovementMode == EMovementMode::MOVE_Walking && Hovering)
		{
			Hovering = false;
			OnHoverChanged(false);
		}
		if (mCachedMovementComponent->mIsSprinting != nIsSprinting)
		{
			nIsSprinting = mCachedMovementComponent->mIsSprinting;
			OnIsSprintingChanged(nIsSprinting);
		}
		if (mCachedMovementComponent->mIsSliding != nIsSliding)
		{
			nIsSliding = mCachedMovementComponent->mIsSliding;
			OnIsSlidingChanged(nIsSliding);
		}
		if (mCachedMovementComponent->MovementMode != nMovementMode)
		{
			nMovementMode = mCachedMovementComponent->MovementMode;
			uint8 out=  nMovementMode;
			OnMoveRep(out);
		}
		if (mCachedMovementComponent->CustomMovementMode != nCustomMovementMode)
		{
			nCustomMovementMode = mCachedMovementComponent->CustomMovementMode;
			OnCustomMovementMode(nCustomMovementMode);
		}
	}
	else if(!nRemoteControlled)
	{
		if (nMovementMode == EMovementMode::MOVE_Walking && Hovering)
		{
			Hovering = false;
		}
		if (mCachedMovementComponent->mIsSprinting != nIsSprinting)
		{
			nIsSprinting = mCachedMovementComponent->mIsSprinting;
		}
		if (mCachedMovementComponent->mIsSliding != nIsSliding)
		{
			nIsSliding = mCachedMovementComponent->mIsSliding;
		}
		if (mCachedMovementComponent->MovementMode != nMovementMode)
		{
			nMovementMode = mCachedMovementComponent->MovementMode;
		}
		if (mCachedMovementComponent->CustomMovementMode != nCustomMovementMode)
		{
			nCustomMovementMode = mCachedMovementComponent->CustomMovementMode;
			OnCustomMovementMode(nCustomMovementMode);

		}
	}

}

void APowerSuit::CheckFuel()
{
	AFGCharacterPlayer * pawn = Cast< AFGCharacterPlayer>(Instigator);
	if (pawn)
	{
		for (int32 i = 0; i < mCostToUse.Num(); i++)
		{
			if (pawn->GetInventory()->HasItems(mCostToUse[i].ItemClass, mCostToUse[i].Amount))
			{
				pawn->GetInventory()->Remove(mCostToUse[i].ItemClass, mCostToUse[i].Amount);
				// item Removed -> Reset Fuel and Power of our Component
				FuelAmount = 1;
				Module->CurrentPower = 1;
				// Trigger event setting the Icon and inventory Slot we now take from
				if (pawn->GetInventory()->HasItems(mCostToUse[i].ItemClass, mCostToUse[i].Amount))
				{
					TArray< TSubclassOf<  UFGItemDescriptor > > relevantClasses;
					relevantClasses.Add(mCostToUse[i].ItemClass);
					TArray <int32> indexes = pawn->GetInventory()->GetRelevantStackIndexes(relevantClasses, 10);
					if (indexes.IsValidIndex(0))
						OnFuelConsumption.Broadcast(mCostToUse[i].ItemClass, indexes[0], 1);
					else
						OnFuelConsumption.Broadcast(mCostToUse[i].ItemClass, 0, 1);
				}
				else
				{
					OnFuelConsumption.Broadcast(mCostToUse[i].ItemClass, 0, 1);
				}
				break;
			}
		}
	}
}

void APowerSuit::Tick(float DeltaSeconds)
{
	if (!IsEquipped())
		return;
	// Thrust when ever possible
	if (!mIsThrusting && CanThrust())
		SetIsThrusting(true);
	else if (mIsThrusting && !CanThrust())
	{
		SetIsThrusting(false);
		JetPackStopThrust();
	}

	// check on Fuel if emtpy
	if (Instigator->HasAuthority())
		if (FuelAmount <= 0)
			CheckFuel();

	// Power
	HandlePowerConsumption(DeltaSeconds);

	// replicate Buttons and Movement States
	ReplicateStates();

	// this should not be here but fine for now
	if (nIsSliding && mIsThrusting && Module)
	{
		mCachedMovementComponent->mSlideTime = 0;
		mCachedMovementComponent->mMaxSlideAngle = 1.64999997615814f + Module->Stats.mMaxSlideAngle;
	}
}

void APowerSuit::ResetMovementComponent(UFGCharacterMovementComponent  * ref)
{
	ref->JumpZVelocity = 500.f;
	ref->JumpOffJumpZFactor =  0.5f;
	ref->GroundFriction =  8.0f;
	ref->MaxWalkSpeed = 500.0f;
	ref->MaxWalkSpeedCrouched =  240.0f;
	ref->MaxSwimSpeed = 300.0f;
	ref->BrakingFrictionFactor =  2.f;
	ref->BrakingFriction = 0;
	ref->BrakingDecelerationWalking =  2048.0;
	ref->BrakingDecelerationSwimming =  100.0;
	ref->BrakingDecelerationFalling =0;
	ref->FallingLateralFriction = 0;
	ref->GravityScale =  1.2f;
	ref->mClimbSpeed =  500.f;
	ref->mMaxSprintSpeed = 900.f;
	ref->mMaxSlideAngle = 1.64999997615814f;
	ref->mBoostJumpZMultiplier =  1.5f;
	ref->mBoostJumpVelocityMultiplier =  1.29999995231628f;
}

void APowerSuit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APowerSuit, FuelAmount);
}

float APowerSuit::CalculateZVelocity(FVector VelInput,FVector PlayerVelocity, float dt)
{
	float out = 0;
	if (HotKeyUpAccelration)
	{
		// Not really clamping dt here, why im not sure
		double prec = (1/((1.f / 60.f) / FMath::Clamp(dt, 0.000001f, 1.f)));
		out = Module->Stats.nAirSpeedModifierUp*prec;
	}
	else
	{
		if (HotKeyDownAccelration)
			 out = Module->Stats.nAirSpeedModifierDown;
		else
		{
			if (PlayerVelocity.Z > 0)
				out = 0;
			else
			{
				// here we need to clamp deltatime..
				double prec = (-2.0)*(1/((1. / 60.) / FMath::Clamp(dt, 0.000001f, 0.016667f)));
				out = VelInput.Z + (PlayerVelocity.Z)*prec;
			}
		}
	}
	return out;
}

FVector APowerSuit::CalculateDampening(FVector PlayerVelocity, UCurveFloat * FloatTable)
{
	FVector out = FVector();
	float absvelocity = (FMath::Abs(PlayerVelocity.X) + FMath::Abs(PlayerVelocity.Y)) / 2;
	float dampval = (1/ Module->Stats.nDampeningMultiplier) * FMath::Abs(FloatTable->GetFloatValue(absvelocity));
	float mult = 1.0f;

	// urgh .. works tho...

	if (PlayerVelocity.X >= 0)
		mult = 1.0f;
	else
		mult = -1.0f;

	if (FMath::Abs(PlayerVelocity.X) > 1.0f)
		out.X = mult * FMath::Abs(PlayerVelocity.X / dampval);
	else
		out.X = PlayerVelocity.X;

	if (PlayerVelocity.Y >= 0)
		mult = 1.0f;
	else
		mult = -1.0f;

	if (FMath::Abs(PlayerVelocity.Y) > 1.0f)
		out.Y = mult * FMath::Abs(PlayerVelocity.Y / dampval);
	else
		out.Y = PlayerVelocity.Y;

	return out;
}

void APowerSuit::HandlePipeVelocity(float dt)
{
	float prec = (1 / ((1.f / 60.f) / FMath::Clamp(dt, 0.000001f, 1.f)));
	if (Module->CurrentPower > 0.05f)
	{
		if (HotKeyDirectionalAccelration || HotKeyDownAccelration)
		{
			if (Instigator->HasAuthority())
			{
				if (nRemoteControlled)
				{
					float speed = nPipeVelocity;
					mCachedMovementComponent->mPipeData.mPipeVelocityReal = nPipeVelocity;
					mCachedMovementComponent->mPipeData.mPipeVelocity = nPipeVelocity;
				}
				else
				{

					float speed = mCachedMovementComponent->GetPipeVelocity();
					if (Cast<AFGBuildablePipeHyper>(mCachedMovementComponent->mPipeData.mTravelingPipeHyper))
					{
						float secc = Cast<AFGBuildablePipeHyper>(mCachedMovementComponent->mPipeData.mTravelingPipeHyper)->mLength - mCachedMovementComponent->mPipeData.mPipeProgressReal;
						float sec = mCachedMovementComponent->mPipeData.mPipeProgressReal;

						if (secc > 1000.f && FMath::Abs(sec) > 1000.f)
						{
							if (HotKeyDirectionalAccelration)
								mCachedMovementComponent->mPipeData.mPipeVelocityReal = FMath::Clamp((speed + ((speed * 0.016667f * prec + Module->Stats.nPipeAccel))), -10000.f, 10000.f);
							else
								mCachedMovementComponent->mPipeData.mPipeVelocityReal = FMath::Clamp((speed - ((speed * 0.016667f * prec))), -10000.f, 10000.f);
							lastPipeVelocity = mCachedMovementComponent->mPipeData.mPipeVelocityReal;
							mCachedMovementComponent->mPipeData.mPipeVelocity = mCachedMovementComponent->mPipeData.mPipeVelocityReal;
						}
					}
				}
			}
			else
			{
				float speed = mCachedMovementComponent->GetPipeVelocity();
				if (Cast<AFGBuildablePipeHyper>(mCachedMovementComponent->mPipeData.mTravelingPipeHyper))
				{
					float secc = Cast<AFGBuildablePipeHyper>(mCachedMovementComponent->mPipeData.mTravelingPipeHyper)->mLength - mCachedMovementComponent->mPipeData.mPipeProgressReal;
					float sec = mCachedMovementComponent->mPipeData.mPipeProgressReal;

					if (secc > 1000.f && FMath::Abs(sec) > 1000.f)
					{
						if (HotKeyDirectionalAccelration)
							mCachedMovementComponent->mPipeData.mPipeVelocityReal = FMath::Clamp((speed + ((speed * 0.016667f * prec))), -10000.f, 10000.f);
						else
							mCachedMovementComponent->mPipeData.mPipeVelocityReal = FMath::Clamp((speed - ((speed * 0.016667f * prec))), -10000.f, 10000.f);

						lastPipeVelocity = mCachedMovementComponent->mPipeData.mPipeVelocityReal;
						mCachedMovementComponent->mPipeData.mPipeVelocity = mCachedMovementComponent->mPipeData.mPipeVelocityReal;
					}
				}
				OnPipeVelocityChanged(mCachedMovementComponent->GetPipeVelocity());
			}
		}
	}
}

void APowerSuit::HandlePowerConsumption(float dt)
{
	if (!Module)
		return;

	if (nCustomMovementMode == ECustomMovementMode::CMM_PipeHyper)
	{
		if (Module->Stats.nHasPipeAccel && Module->CurrentPower > 0.05f)
		{
			
			if (HotKeyDirectionalAccelration)
			{
				if (Module->AddedDeltaPower != BasePowerUsage)
					OnPowerConsumption.Broadcast(dt, BasePowerUsage);
			}
			else
			{
				if (Module->AddedDeltaPower != BasePowerUsage*0.2f)
					OnPowerConsumption.Broadcast(dt, BasePowerUsage*0.2f);
			}
			
		}
		else
		{
			if (Module->AddedDeltaPower != 0)
				OnPowerConsumption.Broadcast(dt, 0);
		}
	}
	else if (nCustomMovementMode == ECustomMovementMode::CMM_Ladder)
	{
		if (Module->AddedDeltaPower != 0)
		{
			OnPowerConsumption.Broadcast(dt, 0);
		}
	}
	else
	{
		if (nMovementMode == EMovementMode::MOVE_Walking)
		{
			if (Module->CurrentPower > 0.05f)
			{
				if (HotKeyDirectionalAccelration)
				{
					if (Module->AddedDeltaPower != BasePowerUsage*0.7f)
						OnPowerConsumption.Broadcast(dt, BasePowerUsage*0.7f);
				}
				else
				{
					if (Module->AddedDeltaPower != BasePowerUsage*0.1f)
						OnPowerConsumption.Broadcast(dt, BasePowerUsage*0.1f);
				}
			}
		}
		else if (nMovementMode == EMovementMode::MOVE_Falling)
		{
			if (Hovering && Module->CurrentPower > 0.01f)
			{

				if (GetCurrentFuel() > 0.0f && HotKeyDirectionalAccelration)
				{
					if (Module->AddedDeltaPower != BasePowerUsage* 1.f)
						OnPowerConsumption.Broadcast(dt, BasePowerUsage*1.f);
				}
				else
				{
					if (Module->AddedDeltaPower != BasePowerUsage* 0.25f)
						OnPowerConsumption.Broadcast(dt, BasePowerUsage*0.25f);
				}
			}
		}
		else if (nMovementMode == EMovementMode::MOVE_Swimming)
		{
			if (Module->AddedDeltaPower != 0)
			{
				OnPowerConsumption.Broadcast(dt, 0);
			}
		}
		else if (nMovementMode == EMovementMode::MOVE_Flying)
		{
			if (Module->AddedDeltaPower != 0)
			{
				OnPowerConsumption.Broadcast(dt, 0);
			}
		}
		else if (nMovementMode == EMovementMode::MOVE_Custom)
		{
			if (Module->AddedDeltaPower != 0)
			{
				OnPowerConsumption.Broadcast(dt, 0);
			}
		}
		else if (nMovementMode == EMovementMode::MOVE_None)
		{
			if (Module->AddedDeltaPower != 0)
			{
				OnPowerConsumption.Broadcast(dt, 0);
			}
		}
	}
	return;
}

FVector APowerSuit::ApplyMultipleir(FVector Vel1, FVector PlayerVelocity, FVector Forward, UCurveFloat * FloatTable, float dt)
{
	FVector out = { 0,0,0 };
	FVector AddedVelocity = { 0,0,0 };
	float prec = (1 / ((1.f / 60.f) / FMath::Clamp(dt, 0.000001f, 1.f)));

	if (!Module)
		return out;

	if (nCustomMovementMode == ECustomMovementMode::CMM_PipeHyper && Module->Stats.nHasPipeAccel)
	{
		HandlePipeVelocity(dt);
	}
	else
	{
		if (nMovementMode == EMovementMode::MOVE_Walking)
		{
			if (Module->CurrentPower > 0.05f)
			{
				if (HotKeyDirectionalAccelration)
				{
					AddedVelocity = Forward * 2 * ((Module->Stats.nSpeedModifier + 1)*(Module->Stats.nGroundSpeedModifierSprint + 1))*prec;
				}
				else
				{
					AddedVelocity = Forward * (Module->Stats.nSpeedModifier + 1)*(Module->Stats.nGroundSpeedModifierNormal + 1) *prec;
				}
				// Add stuff up
				out.Z = 0;
				out.Z = out.Z + AddedVelocity.Z;
				out.X = Vel1.X;
				out.X = out.X + AddedVelocity.X;
				out.Y = Vel1.Y;
				out.Y = out.Y + AddedVelocity.Y;
				// apply smoothing clamp
				if (FMath::Abs(out.X) > 0.f && FMath::Abs(out.X) < 0.005f)
					out.X = 0;
				if (FMath::Abs(out.Y) > 0.f && FMath::Abs(out.Y) < 0.005f)
					out.Y = 0;

				if (Module->Stats.nHasBeltImmunity)
				{
					// TODO : either const hook or proper velocity cancelation, what i tried before never ended in full cancelation.
				}
			}
		}
		if (nMovementMode == EMovementMode::MOVE_Falling)
		{
			if (Hovering && Module->CurrentPower > 0.01f)
			{
				Forward.Z = 0;
				prec = (1 / ((1.f / 60.f) / FMath::Clamp(dt, 0.000001f, 0.016667f)));

				if (GetCurrentFuel() > 0.0f && HotKeyDirectionalAccelration)
				{

					AddedVelocity = Forward * 
						prec
							*(1 + Module->Stats.nAirGlobalSpeed)
							*(1 + Module->Stats.nSpeedModifier)
							*(1 + Module->Stats.nAirSpeedModifierSprint);
					if(HasAuthority())
						FuelAmount = GetCurrentFuel() - ((dt) * (1 / Module->Stats.nChargeDuration));
				}
				else
				{
					AddedVelocity = Forward 
						* prec
						* (1 + Module->Stats.nAirGlobalSpeed) 
						* (1 + Module->Stats.nSpeedModifier) 
						* (1 + Module->Stats.nAirSpeedModifierNormal);
				}

				out.Z = CalculateZVelocity(Vel1, PlayerVelocity, dt);
				out.Z = out.Z + AddedVelocity.Z;
				out.X = Vel1.X - CalculateDampening(PlayerVelocity, FloatTable).X;
				out.X = out.X + AddedVelocity.X;
				out.Y = Vel1.Y - CalculateDampening(PlayerVelocity, FloatTable).Y;
				out.Y = out.Y + AddedVelocity.Y;

				if (FMath::Abs(out.X) > 0.f && FMath::Abs(out.X) < 0.005f)
					out.X = 0;
				if (FMath::Abs(out.Y) > 0.f && FMath::Abs(out.Y) < 0.005f)
					out.Y = 0;
			}
		}
	}
	 
	
	return out;
}
