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

	if (nBasePowerUsage.Num() != int32(EPowerSuitState::PowerSuitStateMAX))
	{
		for (int32 i = 0; i < EPowerSuitState::PowerSuitStateMAX; ++i)
		{
			if (!nBasePowerUsage.Contains(EPowerSuitState(i)))
				nBasePowerUsage.Add(EPowerSuitState(i), 0.f);
		}
	}
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
				//if(Module->CurrentPower + .1f < 1.f)
				//	Module->CurrentPower += .1f;
				Module->Producing = true;
				
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
				nCurrentFuelItem = mCostToUse[i].ItemClass;
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

	// replicate Buttons and Movement States
	ReplicateStates();

	// this should not be here but fine for now
	if (nIsSliding && mIsThrusting && Module)
	{
		mCachedMovementComponent->mSlideTime = 0;
		mCachedMovementComponent->mMaxSlideAngle = 1.64999997615814f + Module->Stats.nMovementComponentStats.mMaxSlideAngle;
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

float APowerSuit::CalculateZVelocity(FVector VelInput,FVector PlayerVelocity, float dt, UCurveFloat * FloatTable)
{
	float out = 0;
	if (HotKeyUpAccelration)
	{
		// this part , we dont talk about .. ever
		double prec2 = (1 / ((1. / 60.) / FMath::Clamp(dt, 0.000001f, 0.016667f)));
		double prec = (-2.0)*(1 / ((1. / 60.) / FMath::Clamp(dt, 0.000001f, 0.016667f)));
		if (PlayerVelocity.Z <= 0)
		{
			out = VelInput.Z + (PlayerVelocity.Z)*prec;
		}
		else
		{
			out = (PlayerVelocity.Z);
		}
		float max = 10 * Module->Stats.nSpeedInState[EPowerSuitState::FLYUP].result() * prec2;
		out = FMath::Clamp(out + (max),2.f,max- FMath::Clamp(out /10,0.f,max));
	}
	else
	{
		if (HotKeyDownAccelration)
		{
			out = Module->Stats.nSpeedInState[EPowerSuitState::FLYDOWN].result();
		}
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
	float dampval = (1/ Module->Stats.nSuitProperties[ESuitProperty::nDampening].result()) * FMath::Abs(FloatTable->GetFloatValue(absvelocity));
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
								mCachedMovementComponent->mPipeData.mPipeVelocityReal = FMath::Clamp((speed + ((speed * 0.016667f * prec ))), -10000.f - Module->Stats.nSpeedInState[EPowerSuitState::PIPEHYPER].result(), 10000.f + Module->Stats.nSpeedInState[EPowerSuitState::PIPEHYPER].result());
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
	if (!Module->Stats.nSuitProperties.Contains(ESuitProperty::nExternalPower))
		return;


	if (nMovementMode == EMovementMode::MOVE_Walking)
	{
		if (Module->Producing)
		{
			if (FMath::Abs(Yaxis) == 0 && FMath::Abs(Xaxis) == 0)
				SuitState = EPowerSuitState::STANDING;
			else
			{
				if (HotKeyDirectionalAccelration)
					SuitState = EPowerSuitState::SPRINTING;
				else
					SuitState = EPowerSuitState::WALKING;
			}
		}
	}
	else
	{
		if (nMovementMode == EMovementMode::MOVE_Falling)
		{
			if (Hovering && Module->Producing  && Module->Stats.nHasFlightUnlock)
			{
				if (GetCurrentFuel() > 0.0f && HotKeyDirectionalAccelration)
					SuitState = EPowerSuitState::HOVERSPRINT;
				else if (HotKeyUpAccelration)
					SuitState = EPowerSuitState::FLYUP;
				else if (HotKeyDownAccelration)
					SuitState = EPowerSuitState::FLYDOWN;
				else if (FMath::Abs(Yaxis) != 0 || FMath::Abs(Xaxis) != 0)
					SuitState = EPowerSuitState::HOVERMOVE;
				else
					SuitState = EPowerSuitState::HOVER;
			}
			else
			{
				SuitState = EPowerSuitState::WALKING;
			}
		}
	}


	float Penalty = 0.f;
	float power = 0.f;
	power += Module->Stats.nSuitProperties[ESuitProperty::nExternalPower].Modifier;
	power -= Module->Stats.nPowerProductionInState[SuitState].result();
	power += nBasePowerUsage[SuitState];
	if (FuelAmount <= 0)
	{
		Penalty = Module->Stats.nEmptyFuelPowerPenaltyInState[SuitState].result();
	}

	power =  Module->Stats.nSuitProperties[ESuitProperty::nExternalPower] * (power + Penalty);
	if (Module->AddedDeltaPower != power)
	{
		OnPowerConsumption.Broadcast(dt, power);
		Module->AddDeltaPower(power);
	}
	
	return;	
}

void APowerSuit::HandleFuelConsumption(float dt)
{
	if (!Module)
		return;

	if (!Module->Stats.nSuitProperties.Contains(ESuitProperty::nBaseFuelConsumption))
		return;

	float Penalty = 0.f; float fuel;

	fuel = Module->Stats.nSuitProperties[ESuitProperty::nBaseFuelConsumption].Modifier;
	fuel += Module->Stats.nFuelConsumptionInState[SuitState].result();

	
	if (Module->CurrentPower <= 0.f && Module->Producing)
	{
		Penalty = Module->Stats.nEmptyPowerFuelPenaltyInState[SuitState].result();
	}

	fuel = Module->Stats.nSuitProperties[ESuitProperty::nBaseFuelConsumption] * (fuel + Penalty);

	if (Module->AddedDeltaFuel != fuel)
	{
		Module->AddedDeltaFuel = fuel;
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
		SuitState = EPowerSuitState::PIPEHYPER;
		HandlePipeVelocity(dt);
	}
	else
	{
		if (nMovementMode == EMovementMode::MOVE_Walking)
		{
			if (Module->Producing)
			{			
				AddedVelocity =
					Forward
					* 2
					* FMath::Clamp((1 + Module->Stats.nSuitProperties[ESuitProperty::nSpeed].Multiplier), 0.f, 100000000.f)
					* FMath::Clamp(Module->Stats.nSpeedInState[SuitState].resultOrMult(), 0.f, 100000000.f)
					* prec;
				
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
		if (nMovementMode == EMovementMode::MOVE_Falling )
		{
			if (Hovering && Module->Producing  && Module->Stats.nHasFlightUnlock)
			{
				Forward.Z = 0;
				prec = (1 / ((1.f / 60.f) / FMath::Clamp(dt, 0.000001f, 0.016667f)));

				out.Z = CalculateZVelocity(Vel1, PlayerVelocity, dt, FloatTable);		

				AddedVelocity = Forward
					* FMath::Clamp((1 + Module->Stats.nSuitProperties[ESuitProperty::nAirSpeed].Multiplier), 0.f, 100000000.f)
					* FMath::Clamp((1 + Module->Stats.nSuitProperties[ESuitProperty::nSpeed].Multiplier), 0.f, 100000000.f)
					* FMath::Clamp(Module->Stats.nSpeedInState[SuitState].resultOrMult() , 0.f, 100000000.f)
					* prec;

				if (GetCurrentFuel() > 0.0f && HotKeyDirectionalAccelration && HasAuthority())
				{
					FuelAmount = GetCurrentFuel() - ((dt) * (1 / FMath::Clamp( Module->Stats.nSuitProperties[ESuitProperty::nChargeDuration].result(),1.f,10000000.f)));
				}
			
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
