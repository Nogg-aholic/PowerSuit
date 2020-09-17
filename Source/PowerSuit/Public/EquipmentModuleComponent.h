// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FGInventoryComponent.h"
#include "EquipmentModuleDescriptor.h"
#include "FactoryGame.h"
#include "FGEquipment.h"
#include "FGHealthComponent.h"
#include "EquipmentModuleComponent.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFuseTriggered, bool, FuseState, FDateTime, Datefloat );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnShieldDamageTaken, float, ShieldDamge, FDateTime, Date, bool, empty);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POWERSUIT_API UEquipmentModuleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEquipmentModuleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void Init(UFGInventoryComponent * Inventory, AFGEquipment  *Parent);

private:

	void RegenShield(float DeltaTime);
	void RegenPower(float DeltaTime);
	void RegenHealth(float DeltaTime);

	void ConsumeFuel(float DeltaTime);

	bool IsProducing();

	bool ShouldRegenShield(FTimespan timesinceShieldDmg);
public:
	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void OnInventoryItemAdd(TSubclassOf<UFGItemDescriptor> itemClass, int32 numAdded);
	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void OnInventoryItemRemove(TSubclassOf<UFGItemDescriptor> itemClass, int32 numAdded);
private:
	void UpdateStats(UFGInventoryComponent * Inventory);

	float ApplyShieldDamage(float DmgIn);

public:

	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		float CalculateDamage(UFGInventoryComponent * Inventory, float DmgIn, ENDamageType Type, TSubclassOf<class UFGDamageType> BpType);

	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void AddDeltaPower(float PowerConsumption);


	/*
				COSMETIC
	*/

	UFUNCTION(BlueprintImplementableEvent)
		void ShieldDamageTaken(UEquipmentModuleComponent* selfref, float ShieldDamge, FDateTime Date);

	UFUNCTION(BlueprintImplementableEvent)
		void FuseTriggered(UEquipmentModuleComponent* selfref, bool FuseState, FDateTime Date);



	/*
			** UProps **
	*/

	/*
			BASIC
	*/

	// Reference of the Component we are Attached to"
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Components")
		AFGEquipment * EquipmentParent;
	//  Internal Inventory Ref"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
		UFGInventoryComponent * nInventory;

	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Stats")
		FEquipmentStats DefaultStats;
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Stats")
		FEquipmentStats Stats;

	/*
			STATE
	*/

	// Are we producing power ? -> !IsFuseTriggered"
	UPROPERTY(BlueprintReadonly, Replicated, Category = "EquipmentModule")
		bool Producing;
	// Are we consuming extra power?"
	UPROPERTY(BlueprintReadonly, Replicated, Category = "EquipmentModule")
		bool Active;
	// Took ShieldDamage within the DelayTimer ran out"
	UPROPERTY(BlueprintReadonly, Replicated, Category = "EquipmentModule")
		bool ShieldBroke;

	/*
			POWER
	*/

	//  Power in Percent 
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float CurrentPower;
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetPowerDraw();
	//  External Power Draw in MWs
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float AddedDeltaPower;
	//	External Fuel Draw in MWs	( Power overdraw is negativly reducing ChargeDuration on which we base the amount to remove per frame)
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float AddedDeltaFuel;

	/*
			SHIELD
	*/

	// Amount of Shield Total"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float CurrentShield;

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetShieldRechargeRate() { return FMath::Clamp(Stats.nSuitProperties[ESuitProperty::nShieldRegen].result(), 0.f, 10000.f); };
	// Max Shield is Mod *Mult 
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetMaxShield() { if (Producing)return (Stats.nSuitProperties[ESuitProperty::nShield].result()); else return 0; };


	/*
			HEALTH
	*/
	// We use this to subtract Delta from
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Stats")
		float HealthBuffer;

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetNewMaxHealth() { return FMath::Clamp(100.f + Stats.nSuitProperties[ESuitProperty::nHealth].result(), 1.f, 1000000.f); };

	/*
			TIMER
	*/

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetActiveTimerDuration() { return  FMath::Clamp(Stats.nSuitProperties[ESuitProperty::nActiveTime].result(), 0.f, 10000.f); };
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetFuseTimerDuration() { return FMath::Clamp(Stats.nSuitProperties[ESuitProperty::nFuseTime].result(), 0.f, 10000.f); };
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetShieldRechargeDelayDuration() { return FMath::Clamp(Stats.nSuitProperties[ESuitProperty::nShieldRechargeDelay].result(), 0.f, 10000.f); };
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetOverDrawDuration() { return FMath::Clamp(Stats.nSuitProperties[ESuitProperty::nFuseTimeOverDraw].result(), 0.f, 10000.f); };

	// Timer for when Fuse was Triggered"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
		FDateTime FuseBreak;
	// Timer for when Fuse was Triggered"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
		FDateTime FuseBreakOverDraw;
	// Timer for when Active was Triggered"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
		FDateTime ActiveTimer;
	// Timer for when we took Shield Damage"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
		FDateTime ShieldDmg;

	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnFuseTriggered OnFuseTriggered;
	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnShieldDamageTaken OnShieldDamageTaken;


	UFUNCTION(BlueprintPure)
		float GetPropertyResult(FModMultProperty prop) { return prop.result(); };


	/*
			MISC STUFF
	*/

	TArray<TSubclassOf<class UEquipmentModuleDescriptor>> UniquesActive;

	UFUNCTION(BlueprintCallable)
		FString GetStatsAsString();

	TArray<AActor *> Attachments;
	
};