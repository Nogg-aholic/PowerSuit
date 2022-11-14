// Copyright Epic Games, Inc. All Rights Reserved.

#include "PowerSuit.h"


#include "FGCharacterPlayer.h"
#include "Buildables/FGBuildableConveyorBelt.h"
#include "Equipment/Equipment_PowerSuit.h"
#include "Patching/NativeHookManager.h"
#include "SubModules/EMC_StateModule.h"
#define LOCTEXT_NAMESPACE "FPowerSuitModule"

DEFINE_LOG_CATEGORY(LogGame);

class APowerSuit;

void FPowerSuitModule::StartupModule()
{
#if WITH_EDITOR 
#else

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	SUBSCRIBE_METHOD(AFGBuildableConveyorBelt::GetVelocityForBase, [](auto& Scope, const AFGBuildableConveyorBelt* Obj, class AActor* basedActor, class UPrimitiveComponent* baseComponent) {
		if (basedActor)
		{
			if (Cast<AFGCharacterPlayer>(basedActor))
			{
				for (auto i : Cast<AFGCharacterPlayer>(basedActor)->GetActiveEquipments())
				{
					APowerSuit * Suit = Cast<APowerSuit>(i);
					if (Suit)
					{
						if (Suit->Module->Stats.HasAdvancedFlag(ESuitFlagAdvanced::SuitFlagAdvanced_IgnoreBelts))
						{
							FVector A = basedActor->GetRootComponent()->GetForwardVector();
		                    FVector B = Scope(Obj,basedActor,baseComponent);
		                    const float Angle = FMath::Abs(FQuat::FindBetween(A, B).Rotator().Yaw);
							const bool LookingWithinAcceptedAngle = FMath::Abs(Angle) <= Cast<APowerSuit>(i)->BeltVelocityAcceptanceAngle;
							const bool PressingAccel = Suit->Module->StateModule->HKey_Accel;
		                    if (!LookingWithinAcceptedAngle || !PressingAccel)
		                    {
								// Prevent movement by belt
		                        FVector NewValue = FVector(0,0,0);
		                        Scope.Override(NewValue);
		                    }
							else if (LookingWithinAcceptedAngle && PressingAccel)
							{
								// Increase the applied belt speed
								// TODO consider cleaning up later with a real property instead of a named property
								if (Suit->Module->Stats.nNamedProperties.Contains("nBeltSpeedMultiplier")) {
									FModMultProperty& speedProp = *Suit->Module->Stats.nNamedProperties.Find("nBeltSpeedMultiplier");
									FVector NewValue = speedProp.ClampAdd(1) * B;
									Scope.Override(NewValue);
								}
							}
							
						}
					}
				}
			}
		}
		});
#endif
}

void FPowerSuitModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPowerSuitModule, PowerSuit)