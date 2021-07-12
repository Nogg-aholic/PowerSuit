#pragma once
#include "Components/ActorComponent.h"
#include "Equipment/EquipmentModuleDescriptor.h"
#include "Equipment/FGHoverPack.h"
#include "Stats/PowerSuitData.h"
#include "EquipmentModuleComponent.generated.h"


class UEMC_PowerModule;
class UEMC_ShieldModule;
class UEMC_FuelModule;
class UEMC_HealthModule;
class UEMC_HyperTubeModule;
class UEMC_InventoryModule;
class UEMC_StateModule;
class UEMC_ZiplineModule;
class UPowerSuitRCO;
class UPowerSuitModuleAttachment;
class UEMC_AttachmentModule;
class APowerSuit;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POWERSUIT_API UEquipmentModuleComponent : public USceneComponent, public IFGSaveInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEquipmentModuleComponent();


	virtual void PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion) override;


	virtual void PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion) override;


	virtual void PreLoadGame_Implementation(int32 saveVersion, int32 gameVersion) override;


	virtual void PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion) override;


	virtual void GatherDependencies_Implementation(TArray< UObject* >& out_dependentObjects) override;


	virtual bool ShouldSave_Implementation() const override;


	virtual bool NeedTransform_Implementation() override;

	UFUNCTION()
	bool VerifyItem(TSubclassOf<UFGItemDescriptor> ItemClass, int32 Amount) const;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnConsumeFuelItem, TSubclassOf<class UFGItemDescriptor>, Item, int32, Slotid, int32, AmountLeft);
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnConsumeFuelItem OnConsumeFuelItem;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBufferRefuel);
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnBufferRefuel OnBufferRefuel;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryDropFail, TSubclassOf<class UFGItemDescriptor>, Item, int32, Amount);
	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnInventoryDropFail OnInventoryDropFail;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFuseTriggered, bool, FuseState, FDateTime, Datefloat);
	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnFuseTriggered OnFuseTriggered;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnShieldDamageTaken, float, ShieldDamage, FDateTime, Date, bool, empty);
	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnShieldDamageTaken OnShieldDamageTaken;

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



	// Called every frame


private:

	friend class UEMC_PowerModule;
	friend class APowerSuit;
	friend class UEMC_InventoryModule;
	virtual void EquippedTick(float DeltaTime);



	void ResetStats();
	void SetupSubModules();


public:


	UFUNCTION(Client, Reliable)
		void RemoteInventoryRefresh(bool IsAdd, TSubclassOf<UFGItemDescriptor> Class, int32 Amount);

	UFUNCTION(BlueprintPure)
	FModMultProperty GetSuitPropertySafe(ESuitProperty property) const;
	UFUNCTION(BlueprintPure)
	FModMultProperty GetMovementPropertySafe(ESuitMovementProperty property) const;
	UFUNCTION(BlueprintPure)
	FModMultProperty GetFlightPropertySafe(ESuitFlightProperty property) const;
	UFUNCTION(BlueprintPure)
	FModMultProperty GetNamedPropertySafe(FName property) const;
	UFUNCTION(BlueprintPure)
	static FModMultProperty GetStatePropertySafe(TMap<TEnumAsByte< EPowerSuitState>, FModMultProperty> Map, EPowerSuitState property);




	// Blueprint Called on Equipped
	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
	void Init(APowerSuit* Parent);


	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
	float CalculateDamage(float DmgIn, UPARAM(meta = (Bitmask, BitmaskEnum = ENDamageType)) int32 Type, TSubclassOf<class UFGDamageType> BpType);


	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
	FORCEINLINE  bool HasSuitFlag(ESuitFlag Flag) const { return Stats.HasFlag(Flag); };
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
	FORCEINLINE  bool HasDamageFlag(ENDamageType Type) const { return Stats.HasDamageMask(Type); };


	/*
			** UProps **
	*/




	// Defaults
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule")
		FEquipmentStats DefaultStats;

	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule")
		FEquipmentStats Stats;

	//  Internal Inventory Ref"
	UPROPERTY(BlueprintReadOnly, Replicated, SaveGame, Category = "EquipmentModule")
		UFGInventoryComponent * nInventory;

	// Cached Stats
	
	// Reference of the Component we are Attached to"
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule")
	APowerSuit* EquipmentParent;

	/** A cached instance of the instigators movementcomponent */
	UPROPERTY(BlueprintReadWrite, Category = "EquipmentModule")
	class UFGCharacterMovementComponent* MoveC;


	// Replicated through RCO 

	// SubModules
	UPROPERTY(BlueprintReadOnly)
	UEMC_PowerModule* PowerModule;
	UPROPERTY(BlueprintReadOnly)
	UEMC_ShieldModule * ShieldModule;
	UPROPERTY(BlueprintReadOnly)
	UEMC_FuelModule * FuelModule;
	UPROPERTY(BlueprintReadOnly)
	UEMC_HealthModule* HealthModule;
	UPROPERTY(BlueprintReadOnly)
	UEMC_HyperTubeModule* HyperTubeModule;
	UPROPERTY(BlueprintReadOnly)
	UEMC_InventoryModule* InventoryModule;
	UPROPERTY(BlueprintReadOnly)
	UEMC_AttachmentModule* AttachmentModule;
	UPROPERTY(BlueprintReadOnly)
	UEMC_StateModule* StateModule;
	UPROPERTY(BlueprintReadOnly)
	UEMC_ZiplineModule* ZiplineModule;

	// TODO: remove this
	UPROPERTY(BlueprintReadWrite, Category = "EquipmentModule")
		bool PlayedRechargeSound = false;

	// Replicated Stuff
	
	UPROPERTY(BlueprintReadWrite, Category = "EquipmentModule")
	UPowerSuitRCO* RCO;


	UPROPERTY(Category = "EquipmentModule", SaveGame, BlueprintReadWrite)
		TEnumAsByte<enum EMovementMode> nMovementMode;
	// "
	UPROPERTY(Category = "EquipmentModule", SaveGame,BlueprintReadWrite)
		uint8 nCustomMovementMode;
	// "
	UPROPERTY(Category = "EquipmentModule", BlueprintReadWrite,SaveGame, Replicated)
		TEnumAsByte<EPowerSuitState> SuitState;



	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "EquipmentModule")
		float FallingTime;
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "EquipmentModule")
		bool TKey_Fly;
	// key is also KB_Up but with delay Timer
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "EquipmentModule")
		bool TKey_NoGravity;
	// key is also KB_Up but with delay Timer
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "EquipmentModule")
		bool TKey_NoFriction;
	


	//  External Power Draw in MWs
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float PowerConsumption;

	// Are we producing power ? -> !IsFuseTriggered"
	UPROPERTY(BlueprintReadonly, Replicated, SaveGame, Category = "EquipmentModule")
		bool nProducing;

	// Timer for when Fuse was Triggered"
	UPROPERTY(BlueprintReadOnly, Replicated, SaveGame, Category = "EquipmentModule")
		FDateTime nFuseBreak;
	// Timer for when Fuse was Triggered"
	UPROPERTY(BlueprintReadOnly, Replicated, SaveGame, Category = "EquipmentModule")
		FDateTime nFuseBreakOverDraw;
	// Timer for when we took Shield Damage"
	UPROPERTY(BlueprintReadOnly, Replicated, SaveGame, Category = "EquipmentModule")
		FDateTime nShieldDmg;


	// Took ShieldDamage within the DelayTimer ran out"
	UPROPERTY(BlueprintReadonly, Replicated, Category = "EquipmentModule")
		bool nShieldRegenCooldown;

	// Amount of Shield Total
	UPROPERTY(BlueprintReadOnly, Replicated, SaveGame, Category = "EquipmentModule")
		float nCurrentShield;


	UPROPERTY(replicated, SaveGame)
		float nCurrentPower;

	UPROPERTY(SaveGame)
		float nPowerCapacity;

	//	External Fuel Draw in MWs	( Power overdraw is negativly reducing ChargeDuration on which we base the amount to remove per frame)
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float nFuelConsumption;

	/** Fractional fill level of the fuel tank, 0.0 empty to 1.0 full */
	UPROPERTY(BlueprintReadWrite, Replicated, SaveGame, Category = "EquipmentModule")
		float nFuelAmount;


	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Toggle;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Toggle2;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Up;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Down;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Accel;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Breaks;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_UI;
	float Delta;
};