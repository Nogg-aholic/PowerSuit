#pragma once
#include "PowerSuit.h"
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

	// Used to determine if an item can be put into the suit equip slots. Bound in cpp by function name.
	UFUNCTION()
		bool VerifyItem(TSubclassOf<UFGItemDescriptor> ItemClass, int32 Amount) const;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnConsumeFuelItem, TSubclassOf<class UFGItemDescriptor>, Item, int32, Slotid, int32, AmountLeft);
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnConsumeFuelItem OnConsumeFuelItem;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBufferRefuel);
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnBufferRefuel OnBufferRefuel;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventoryDropFail, TSubclassOf<class UFGItemDescriptor>, Item, int32, Amount, int32, Type);
	// 0 == Unique excluded; 1 == Not Equipment Descriptor; 2 == Rebooting
	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnInventoryDropFail OnInventoryDropFail;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFuseTriggered, bool, FuseState, FDateTime, Datefloat);
	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnFuseTriggered OnFuseTriggered;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFuelFuseTriggered, bool, FuelFuseState, FDateTime, Datefloat);
	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnFuelFuseTriggered OnFuelFuseTriggered;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFuseOverDrawTriggered, bool, TrueForRecovered, FDateTime, Datefloat);
	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnFuseOverDrawTriggered OnFuseOverDrawTriggered;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnShieldDamageTaken, float, ShieldDamage, FDateTime, Date, bool, empty,FHitResult, Impact);
	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnShieldDamageTaken OnShieldDamageTaken;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnModuleInstalled, TSubclassOf<class UFGItemDescriptor>, Item, APowerSuitModuleAttachment *, Attachment);
	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnModuleInstalled OnModuleInstalled;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnModuleUnInstalled, TSubclassOf<class UFGItemDescriptor>, Item, APowerSuitModuleAttachment*, Attachment);
	UPROPERTY(BlueprintAssignable, Category = "EquipmentModule")
		FOnModuleUnInstalled OnModuleUnInstalled;
protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	friend class UEMC_PowerModule;
	friend class APowerSuit;
	friend class UEMC_InventoryModule;
	virtual void EquippedTick(float DeltaTime);


	void ResetStats(bool Notify = true);
	void SetupSubModules();


public:


	UFUNCTION(Client, Reliable)
		void RemoteInventoryRefresh(bool IsAdd, TSubclassOf<UFGItemDescriptor> Class, int32 Amount);

	UFUNCTION(Client, Reliable)
		void RemoteUpdateMovementGround(float Speed);

	UFUNCTION(Client, Reliable)
		void RemoteUpdateMovementAir(float Speed,float Accel);

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

	// Calculate damage while factoring in damage resistances, will send the damage to shield first if the module stats has shield damage flag for that type
	UFUNCTION(BlueprintCallable, Category = "EquipmentModule")
		float CalculateDamage(float DmgIn, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/PowerSuit.ENDamageType")) int32 Type, TSubclassOf<class UFGDamageType> BpType, FHitResult Impact);


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
	
	// Reference of the Component we are Attached to
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule", DisplayName="Parent Suit Equipment")
	APowerSuit* EquipmentParent;

	/** A cached instance of the instigator's movement component */
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

	// Replicated Stuff
	
	UPROPERTY(BlueprintReadWrite, Category = "EquipmentModule")
	UPowerSuitRCO* RCO;


	UPROPERTY(Category = "EquipmentModule", SaveGame, BlueprintReadWrite)
		TEnumAsByte<enum EMovementMode> nMovementMode;

	UPROPERTY(Category = "EquipmentModule", SaveGame,BlueprintReadWrite)
		uint8 nCustomMovementMode;

	UPROPERTY(Category = "EquipmentModule", BlueprintReadWrite,SaveGame, Replicated)
		TEnumAsByte<EPowerSuitState> SuitState;



	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "EquipmentModule")
		float FallingTime;
	
	// key is also KB_Up but with delay Timer
	UPROPERTY(BlueprintReadWrite, Replicated, SaveGame, Category = "EquipmentModule")
		bool TKey_Fly;

	// Replicated State of suit keybind PowerSuit.GravityToggle
	UPROPERTY(BlueprintReadWrite, Replicated,SaveGame, Category = "EquipmentModule")
		bool TKey_NoGravity;

	// Replicated State of suit keybind PowerSuit.FrictionToggle
	UPROPERTY(BlueprintReadWrite, Replicated, SaveGame, Category = "EquipmentModule")
		bool TKey_NoFriction;



	//  External Power Draw in MWs
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "EquipmentModule")
		float PowerConsumption;

	//  External Power Production in MWs
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "EquipmentModule")
		float PowerProduction;

	// Are we producing power? -> Should modules be functioning
	UPROPERTY(BlueprintReadonly, Replicated, SaveGame, Category = "EquipmentModule")
		bool nProducing;

	// Timer for when Fuse was Triggered
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "EquipmentModule", DisplayName="N Fuse Break Time")
		FDateTime nFuseBreak;
	UFUNCTION(Client, Reliable)
		void Client_FuseBreak();

	// Timer for when Overdraw was Triggered
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "EquipmentModule", DisplayName = "N Fuse Break Overdraw Time")
		FDateTime nFuseBreakOverDraw;
	UFUNCTION(Client, Reliable)
		void Client_FuseBreakOverDraw();

	// Timer for when FuelFuse was Triggered
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "EquipmentModule", DisplayName = "N Fuel Fuse Break Time")
		FDateTime nFuelFuseBreak;
	// Are we currently able to supply fuel to modules? True if yes, false if in recovery period
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		bool nFuelFuseStatus;
	UFUNCTION(Client, Reliable)
		void Client_FuelFuseBreak();
	UFUNCTION(Client, Reliable)
		void Client_FuelFuseRecover();

	// Timer for when we took Shield Damage
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "EquipmentModule", DisplayName = "N Shield Damage Time")
		FDateTime nShieldDmg;
	UFUNCTION(Client, Reliable)
		void Client_ShieldDmg();


	// Are we on cooldown for shield regen because we took ShieldDamage within the DelayTimer?
	UPROPERTY(BlueprintReadonly, Replicated, Category = "EquipmentModule")
		bool nShieldRegenCooldown;

	// Amount of Shield Total
	UPROPERTY(BlueprintReadOnly, Replicated, SaveGame, Category = "EquipmentModule")
		float nCurrentShield;

	// Current Power in MJ
	UPROPERTY(replicated, BlueprintReadWrite, SaveGame)
		float nCurrentPower;

	// Maximum Power in MJ the suit can hold
	UPROPERTY(SaveGame)
		float nPowerCapacity;

	//	Constant Fuel Draw in MW, use Attachments for varying fuel consumption
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float nFuelConsumption;

	/** Fractional fill level of the fuel tank, 0.0 empty to 1.0 full */
	UPROPERTY(BlueprintReadWrite, Replicated, SaveGame, Category = "EquipmentModule")
		float nFuelAmount;


	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_ToggleFlightGravity;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_ToggleFlightFriction;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Up;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Down;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_Accel;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_DeAccel;
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		FKey KB_UI;

	// The last frame delta time, in seconds, set by EquippedTick
	float LastDeltaTime;

	// Prop to help with changing flight speeds with less lag
	UPROPERTY(BlueprintReadWrite, Category = "EquipmentModule")
		float HoverMovementSpeedAdded;

	// Prop to help with changing flight speeds with less lag
	UPROPERTY(BlueprintReadWrite, Category = "EquipmentModule")
		float HoverMovementSpeedAccelAdded;

	// Used by EquipmentModuleComponent to delay the update of remote suit inventories. Intentionally allows new executions to cancel the old ones.
	FTimerDelegate DelayedRemoteInventoryRefresh_TimerDel;
	// Used by EquipmentModuleComponent to delay the update of remote suit inventories. Intentionally allows new executions to cancel the old ones.
	FTimerHandle DelayedRemoteInventoryRefresh_TimerHandle;
};