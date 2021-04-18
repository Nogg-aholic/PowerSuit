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




	void PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion);


	void PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion);


	void PreLoadGame_Implementation(int32 saveVersion, int32 gameVersion);


	void PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion);


	void GatherDependencies_Implementation(TArray< UObject* >& out_dependentObjects);

	
	bool ShouldSave_Implementation() const;


	bool NeedTransform_Implementation();


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnConsumeFuelItem, TSubclassOf<class UFGItemDescriptor>, Item, int32, Slotid, int32, AmountLeft);
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnConsumeFuelItem OnConsumeFuelItem;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBufferRefuel);
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnBufferRefuel OnBufferRefuel;


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
	FModMultProperty GetStatePropertySafe(TMap<TEnumAsByte< EPowerSuitState>, FModMultProperty> Map, EPowerSuitState property) const;




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
	UPROPERTY(EditDefaultsOnly, Category = "EquipmentModule | Stats")
		FEquipmentStats DefaultStats;

	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Stats")
		FEquipmentStats Stats;

	//  Internal Inventory Ref"
	UPROPERTY(BlueprintReadOnly, Replicated, SaveGame, Category = "EquipmentModule | Components")
		UFGInventoryComponent * nInventory;

	// Cached Stats
	
	// Reference of the Component we are Attached to"
	UPROPERTY(BlueprintReadOnly, Category = "EquipmentModule | Components")
	APowerSuit* EquipmentParent;

	/** A cached instance of the instigators movementcomponent */
	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
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
	UPROPERTY(BlueprintReadWrite, Category = "Equipment")
		bool PlayedRechargeSound = false;

	// Replicated Stuff
	
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	UPowerSuitRCO* RCO;


	UPROPERTY(Category = "Character Movement: MovementMode", BlueprintReadWrite)
		TEnumAsByte<enum EMovementMode> nMovementMode;
	// "
	UPROPERTY(Category = "Character Movement: MovementMode", BlueprintReadWrite)
		uint8 nCustomMovementMode;
	// "
	UPROPERTY(Category = "Character Movement: MovementMode", BlueprintReadWrite, Replicated)
		TEnumAsByte<EPowerSuitState> SuitState;



	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Equipment")
		float TKey_Fly_Pressed;
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Equipment")
		bool TKey_Fly;
	// key is also KB_Up but with delay Timer
	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Equipment")
		bool TKey_NoFriction;

	
	float Delta;
};