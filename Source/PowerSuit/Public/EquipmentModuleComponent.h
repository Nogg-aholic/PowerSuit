// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FGInventoryComponent.h"
#include "EquipmentModuleDescriptor.h"
#include "FactoryGame.h"
#include "FGHealthComponent.h"
#include "EquipmentModuleComponent.generated.h"
UENUM(Blueprinttype)
enum ENDamageType
{
	DamageTypeNULL,
	DamageTypePhysical,
	DamageTypeGas,
	DamageTypeRadioActivity,
	DamageTypeFallDamage
};



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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//
	void GatherEquipmentModInventory();

	float ApplyShieldDamage(float DmgIn);

	void UpdateStats(UFGInventoryComponent * Inventory);
	
	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void OnInventoryItemAdd(TSubclassOf<UFGItemDescriptor> itemClass, int32 numAdded);
	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void OnInventoryItemRemove(TSubclassOf<UFGItemDescriptor> itemClass, int32 numAdded);
	
	
	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void Init(UFGInventoryComponent * Inventory , AFGEquipment  *Parent );
	
	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		float CalculateDamage(UFGInventoryComponent * Inventory, float DmgIn, ENDamageType Type, TSubclassOf<class UFGDamageType> BpType);
	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		void AddDeltaPower( float delta, float PowerConsumption);
	
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Stats")
		FEquipmentStats BaseStats;

	// Description = "Are we producing power ? -> !IsFuseTriggered"
	UPROPERTY(BlueprintReadonly, Replicated, Category = "EquipmentModule")
		bool Producing;
	// Description = "Are we consuming extra power?"
	UPROPERTY(BlueprintReadonly, Replicated, Category = "EquipmentModule")
		bool Active;
	// Description = "Took ShieldDamage within the DelayTimer ran out"
	UPROPERTY(BlueprintReadonly, Replicated, Category = "EquipmentModule")
		bool ShieldBroke;

	//Timer Durations
	//  Description = "Min Time Active after Multiplier"
	UPROPERTY(BlueprintReadonly, BlueprintReadonly, Category = "EquipmentModule")
		float ActiveTimerDuration;
	// Description = "Min Fuse Time after Multiplier"
	UPROPERTY(BlueprintReadonly, BlueprintReadonly, Category = "EquipmentModule")
		float FuseTimeOutTime;
	// Description = "Min Shield Recharche Delay after Multiplier"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule" )
		float ShieldRechargeDelay;


	//  Description = "0-1 Power State"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule" )
		float CurrentPower;
	//  Description = "External Power Draw in MWs"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float AddedDeltaPower;
	// Description = "Amount of Shield"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float CurrentShield;
	//  Description = "Current Shield Recharge"s
	UPROPERTY(BlueprintReadonly, Category = "EquipmentModule")
		float ShieldRecharge;

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetMaxShield() { if (Producing)return (BaseStats.nShieldModifier +Stats.nShieldModifier)*(1+Stats.nShieldMultiplier); else return 0; };

	// Description = "Current Power Production after calculations"
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule")
		float PowerProduction;
	//  Description = "Power Capacity after Multipliers"
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule")
		float PowerCapacity;
	// Description = "Timer for when Fuse was Triggered"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
	FDateTime FuseBreak;
	
	// Description = "Timer for when Active was Triggered"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
	FDateTime ActiveTimer;
	
	// Description = "Timer for when we took Shield Damage"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
	FDateTime ShieldDmg;

	TArray<TSubclassOf<class UEquipmentModuleDescriptor>> UniquesActive;

	// Description = "Reference of the Component we are Attached to"
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Components")
	AFGEquipment * EquipmentParent;
	
	//  Description = "Internal Inventory Ref"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
	UFGInventoryComponent * nInventory; 

	UFUNCTION(BlueprintImplementableEvent)
		void ShieldDamageTaken(UEquipmentModuleComponent* selfref, float ShieldDamge, FDateTime Date);

	UFUNCTION(BlueprintImplementableEvent)
		void FuseTriggered(UEquipmentModuleComponent* selfref, bool FuseState, FDateTime Date);



	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnFuseTriggered OnFuseTriggered;

	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnShieldDamageTaken OnShieldDamageTaken;

	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Stats")
		FEquipmentStats Stats;

	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Stats")
		float HealthBuffer;

};
