#pragma once
#include "PowerSuit.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FGCheatManager.h"
#include "Buildables/FGBuildable.h"
#include "Replication/FGReplicationDetailInventoryComponent.h"

#include "EquipmentModuleComponent.h"
#include "Replication/PowerSuitRCO.h"
#include "SubModules/EMC_StateModule.h"
#include "Attachment/PowerSuitModuleAttachment.h"
#include "PowerSuitBPLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnObjectConstruct,AActor* , Actor);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnWidgetConstruct,UUserWidget* , Widget);

UCLASS()
class UPowerSuitBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()


public:
	UFUNCTION(BlueprintPure)
		static float GetPropertyResult(FModMultProperty prop) { return prop.value();	};

	UFUNCTION(BlueprintPure)
		static TArray<TSubclassOf<UFGItemDescriptor>> GetAllItemsFiltered(TArray<class TSubclassOf<UFGItemDescriptor>> AllItems, const FString FilterString, const int32 ResutLimit);


	UFUNCTION(BlueprintCallable)
	static void UpdateInnerConnectionRange(APowerSuit* Suit);

	UFUNCTION(BlueprintCallable)
		static void ToggleCameraMode(UFGCheatManager * manager, APlayerController* PlayerController) {
		if (manager)
			manager->ToggleCameraMode();
		else if (PlayerController)
		{
			PlayerController->AddCheats(true);
			if (manager)
				manager->ToggleCameraMode();
		}
	}

	UFUNCTION(BlueprintPure)
	static bool ShouldPlayThrustLoop(UEquipmentModuleComponent* Component)
	{
		if (!Component)
			return false;
		if (FMath::Clamp(Component->MoveC->Velocity.Size2D(), 0.f, 1.f) > 0 || Component->TKey_NoGravity || Component->StateModule->HKey_Up || Component->StateModule->HKey_Down)
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
		static void ResetMovementComponent(UFGCharacterMovementComponent * MovementComponent, bool Notify = false);

	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void UpdateMovementComponent(APowerSuit * EquipmentParent, bool Notify = false);

	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void ResetFlightStats(APowerSuit * EquipmentParent, bool Notify = false);


	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void UpdateFlightStats(APowerSuit * EquipmentParent, bool Notify = false);
	

	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void UpdateFlags(APowerSuit* EquipmentParent, bool Notify = false);
	

	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void UpdateInventorySize(APowerSuit * EquipmentParent, bool Notify = false);
	
	// NOT REPLICATED ! and rarely should be used manually!
	// this is used by the Inventory Component
	// Only use this if you know what you are doing
	UFUNCTION(BlueprintCallable)
		static void UpdateAllNoRefresh(APowerSuit * EquipmentParent, bool Notify = true);

	UFUNCTION(BlueprintPure, Category = "EquipmentStats")
		static bool HasSuitFlag(ESuitFlag Flag, FEquipmentStats Stats)  { return Stats.HasFlag(Flag); };
	UFUNCTION(BlueprintPure, Category = "EquipmentStats")
		static bool HasSuitFlagAdvanced(ESuitFlagAdvanced Flag, FEquipmentStats Stats) { return Stats.HasAdvancedFlag(Flag); };
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
		static void SetSuitPropertyNamed(FEquipmentStats& Stats, FName Prop, FModMultProperty Property);
	// NOT REPLICATED ! see SetProperty for Instructions;
	UFUNCTION(BlueprintCallable)
		static void SetPropertyGeneral(UPARAM(ref) FEquipmentStats& Stats, EEquipmentStatsProperty PropertyType, uint8 Index, FModMultProperty Property);
	// NOT REPLICATED ! see SetProperty for Instructions;
	UFUNCTION(BlueprintCallable)
		static void SetSuitFlag(UPARAM(ref) FEquipmentStats& Stats, ESuitFlag Flag, bool Enabled);
	// NOT REPLICATED ! see SetProperty for Instructions;
	UFUNCTION(BlueprintCallable)
		static void SetSuitFlagAdvanced(UPARAM(ref) FEquipmentStats& Stats, ESuitFlagAdvanced Flag, bool Enabled);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PowerSuit Bind on Actor Construct"))
	static void BindOnActorConstruct(TSubclassOf<AActor> ActorClass, FOnObjectConstruct Binding);

	static FOnObjectConstruct OnObjectConstruct;
	
	UFUNCTION(BlueprintCallable)
	static void SetUseDistanceOnPlayer(AFGCharacterPlayer* Player, float Distance);

	UFUNCTION(BlueprintCallable)
		static void SetInnerConnection(APowerSuit* Suit, UFGPowerConnectionComponent* Connection);
	UFUNCTION(BlueprintCallable)
		static void InitBattery(UFGPowerConnectionComponent* Connection, float Capacity, float inputCapacity);

	UFUNCTION(BlueprintCallable)
		static float GetBatteryState(UFGPowerConnectionComponent* Connection);
	UFUNCTION(BlueprintCallable)
		static float GetBatteryInput(UFGPowerConnectionComponent* Connection);
	UFUNCTION(BlueprintCallable)
		static float AddPowerStore(UFGPowerConnectionComponent* Connection, float PowerStore);
	UFUNCTION(BlueprintCallable)
		static float RemovePowerStore(UFGPowerConnectionComponent* Connection, float PowerStore);
	
	// Approximation !! not guaranteed to fit !!! 
	UFUNCTION(BlueprintCallable)
		static int32 GetAvailableSpaceForItem(UFGInventoryComponent* Inventory, TSubclassOf<class UFGItemDescriptor> ItemClass, int32 MaxStacks);

	// Likely to break; probably nullptr
	UFUNCTION(BlueprintCallable)
		static TSubclassOf<UFGRecipe> GetBuiltWithRecipe(AFGBuildable* Building);

	// Struct is not blueprint compatible so must handle this part all in cpp
	static class UFGInventoryComponent* GetReplicationDetailActiveInventoryComponent(FReplicationDetailData* ReplicationDetailData);
	// Struct is not blueprint compatible so must handle this part all in cpp
	static void GetReplicationDetailData(AFGBuildable* buildable, TArray<FReplicationDetailData*>& out_repDetailData);

	// Use the replication detail data system to get the real inventory component used by a buildable
	UFUNCTION(BlueprintCallable)
		static TArray<UFGInventoryComponent*> GetRealInventoryComponents(AFGBuildable* buildable);

};
