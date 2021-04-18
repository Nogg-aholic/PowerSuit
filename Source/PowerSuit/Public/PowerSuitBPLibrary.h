#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "FGCheatManager.h"
#include "EquipmentModuleComponent.h"
#include "Replication/PowerSuitRCO.h"
#include "SubModules/EMC_StateModule.h"
#include "Attachment/PowerSuitModuleAttachment.h"
#include "PowerSuitBPLibrary.generated.h"

UCLASS()
class UPowerSuitBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()


public:
	UFUNCTION(BlueprintPure)
		static float GetPropertyResult(FModMultProperty prop) { return prop.value(); };

	UFUNCTION(BlueprintCallable)
		static void ToggleCameraMode(UFGCheatManager * manager) {
		if (manager)
			manager->ToggleCameraMode();
	}

	UFUNCTION(BlueprintPure)
	static bool ShouldPlayThrustLoop(UEquipmentModuleComponent* Component)
	{
		if (!Component)
			return false;
		if (FMath::Clamp(Component->MoveC->Velocity.Size2D(), 0.f, 1.f) > 0 || Component->TKey_NoFriction || Component->StateModule->HKey_Up || Component->StateModule->HKey_Down)
			return true;
		else
			return false;
	}

	UFUNCTION(BlueprintCallable)
		static bool EnableHover(UEquipmentModuleComponent* Component);

	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void ResetMovementComponent(UFGCharacterMovementComponent * MovementComponent);

	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void UpdateMovementComponent(APowerSuit * EquipmentParent);

	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void ResetFlightStats(APowerSuit * EquipmentParent);


	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void UpdateFlightStats(APowerSuit * EquipmentParent);
	

	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void UpdateFlags(APowerSuit* EquipmentParent);
	

	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void UpdateInentorySize(APowerSuit * EquipmentParent);
	
	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void UpdateAllNoRefresh(APowerSuit * EquipmentParent);

	UFUNCTION(BlueprintPure, Category = "EquipmentStats")
		static bool HasSuitFlag(ESuitFlag Flag, FEquipmentStats Stats)  { return Stats.HasFlag(Flag); };
	UFUNCTION(BlueprintPure, Category = "EquipmentStats")
		static bool HasDamageFlag(ENDamageType Type, FEquipmentStats Stats) { return Stats.HasDamageMask(Type); };


	// NOT REPLICATED ! see SetProperty for Instructions;
	UFUNCTION(BlueprintCallable)
		static void SetSuitMovementProperty(UPARAM(ref) FEquipmentStats& Stats,ESuitMovementProperty Enum, FModMultProperty Property);
	// NOT REPLICATED ! see SetProperty for Instructions;
	UFUNCTION(BlueprintCallable)
		static void SetSuitFlightProperty(UPARAM(ref) FEquipmentStats& Stats, ESuitFlightProperty Enum, FModMultProperty Property);
	// NOT REPLICATED!  use this on the Server and Replicate changes to Clients by replicating for example the source variables and make functions deterministic
	// or by replicating result values. If this is not important on the Client you may not Replicate this;
	UFUNCTION(BlueprintCallable)
		static void SetSuitProperty(UPARAM(ref) FEquipmentStats& Stats, ESuitProperty Enum, FModMultProperty Property);
	// NOT REPLICATED ! see SetProperty for Instructions;
	UFUNCTION(BlueprintCallable)
		static void SetPropertyGeneral(UPARAM(ref) FEquipmentStats& Stats, EEquipmentStatsProperty PropertyType, uint8 Index, FModMultProperty Property);
	// NOT REPLICATED ! see SetProperty for Instructions;
	UFUNCTION(BlueprintCallable)
		static void SetSuitFlag(UPARAM(ref) FEquipmentStats& Stats, ESuitFlag Flag, bool Enabled);

};
