


#include "EquipmentModuleComponent.h"
#include "Equipment/Equipment_PowerSuit.h"
#include "FGHealthComponent.h"
#include "FGPlayerController.h"
#include "FGCharacterPlayer.h"
#include "PowerSuitBPLibrary.h"
#include "SubModules/EMC_PowerModule.h"
#include "SubModules/EMC_FuelModule.h"
#include "SubModules/EMC_ShieldModule.h"
#include "SubModules/EMC_HealthModule.h"
#include "SubModules/EMC_HyperTubeModule.h"
#include "SubModules/EMC_InventoryModule.h"
#include "SubModules/EMC_AttachmentModule.h"
#include "SubModules/EMC_StateModule.h"
#include "SubModules/EMC_ZiplineModule.h"

#include "Replication/PowerSuitRCO.h"
#include "Attachment/PowerSuitModuleAttachment.h"
#include "Buildables/FGBuildablePipeHyper.h"


#include "FGCharacterMovementComponent.h"




// Sets default values for this component's properties
UEquipmentModuleComponent::UEquipmentModuleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	nInventory = nullptr;
	SuitState = EPowerSuitState::PS_REBOOTING;
	PowerConsumption = 0;
	nProducing = false;
	nCurrentPower = 0;
	nFuseBreak = FDateTime::Now();
	nFuseBreakOverDraw = FDateTime::Now();
	nFuelFuseStatus = true;
	nFuelFuseBreak = FDateTime::FromUnixTimestamp(0); // assume intact on construct
	nShieldRegenCooldown = false;
	nCurrentShield = 0;
	nFuelConsumption = 0;
	nFuelAmount = 0;
}

void UEquipmentModuleComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEquipmentModuleComponent, nInventory);
	DOREPLIFETIME(UEquipmentModuleComponent, SuitState);
	DOREPLIFETIME(UEquipmentModuleComponent, PowerConsumption);
	DOREPLIFETIME(UEquipmentModuleComponent, PowerProduction);
	
	DOREPLIFETIME(UEquipmentModuleComponent, nProducing);
	DOREPLIFETIME(UEquipmentModuleComponent, nCurrentPower);

	DOREPLIFETIME(UEquipmentModuleComponent, nShieldRegenCooldown);
	DOREPLIFETIME(UEquipmentModuleComponent, nCurrentShield);

	DOREPLIFETIME(UEquipmentModuleComponent, nFuelConsumption);
	DOREPLIFETIME(UEquipmentModuleComponent, nFuelAmount);
	DOREPLIFETIME(UEquipmentModuleComponent, nFuelFuseStatus);
	DOREPLIFETIME(UEquipmentModuleComponent, TKey_Fly);
	DOREPLIFETIME(UEquipmentModuleComponent, TKey_NoGravity);
	DOREPLIFETIME(UEquipmentModuleComponent, TKey_NoFriction);
}

// Called every frame
// Handle suit Fuse and Active timers, power and fuel consumption, shields, health regen, etc.
void UEquipmentModuleComponent::EquippedTick(float DeltaTime)
{
	if (!EquipmentParent)
		return;
	if (!EquipmentParent->IsEquipped()|| !PowerModule || !RCO)
		return;

	LastDeltaTime = DeltaTime;

	// Important order ! 
	StateModule->PreTick();  // 1
	AttachmentModule->PreTick(); // 2 
	PowerModule->PreTick(); // 3 
	FuelModule->PreTick(); // 4 <- we only reload when we produce
	HyperTubeModule->PreTick(); // 5
	InventoryModule->PreTick(); // 6 
	ShieldModule->PreTick();  // 7 
	ZiplineModule->PreTick(); // 8
	HealthModule->PreTick(); // 9

	// Update the State depending on our Power and Fuel access
	StateModule->Tick(); // 1
	AttachmentModule->Tick(); // 2 
	FuelModule->Tick(); // 4 <- order swapped 
	PowerModule->Tick(); // 3  we may not have fuel
	HyperTubeModule->Tick(); // 5
	InventoryModule->Tick(); // 6 
	ShieldModule->Tick();  // 7 
	ZiplineModule->Tick(); // 8
	HealthModule->Tick(); // 9


	// Fuel & Power was used items were consumed  
	StateModule->PostTick();  // 1
	AttachmentModule->PostTick(); // 2 
	PowerModule->PostTick(); // 3 
	FuelModule->PostTick(); // 4
	HyperTubeModule->PostTick(); // 5
	InventoryModule->PostTick(); // 6 
	ShieldModule->PostTick();  // 7 
	ZiplineModule->PostTick(); // 8
	HealthModule->PostTick(); // 9


}


void UEquipmentModuleComponent::SetupSubModules()
{

	AFGPlayerController* Controller = Cast< AFGPlayerController>(EquipmentParent->GetInstigatorController());
	if (!Controller)
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("Controller cast failed! fix this "));
		return;
	}

	if (!PowerModule)
	{

		UE_LOG(PowerSuit_Log, Display, TEXT("Creating SubModules "));

		StateModule = NewObject<UEMC_StateModule>(); // 1
		AttachmentModule = NewObject<UEMC_AttachmentModule>(); // 2
		PowerModule = NewObject<UEMC_PowerModule>(); // 3 
		FuelModule = NewObject<UEMC_FuelModule>(); // 4
		HyperTubeModule = NewObject<UEMC_HyperTubeModule>(); // 5
		InventoryModule = NewObject<UEMC_InventoryModule>(); // 6
		ShieldModule = NewObject<UEMC_ShieldModule>(); // 7 
		ZiplineModule = NewObject<UEMC_ZiplineModule>(); // 8
		HealthModule = NewObject<UEMC_HealthModule>(); // 9
	}

	StateModule->Parent = this; // 1
	AttachmentModule->Parent = this; // 2 
	PowerModule->Parent = this; // 3 
	FuelModule->Parent = this; // 4
	HyperTubeModule->Parent = this; // 5
	InventoryModule->Parent = this; // 6 
	ShieldModule->Parent = this;  // 7 
	ZiplineModule->Parent = this; // 8
	HealthModule->Parent = this; // 9

	RCO = Cast< UPowerSuitRCO>(Controller->GetRemoteCallObjectOfClass(UPowerSuitRCO::StaticClass()));

	if (!RCO)
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("Registered RCO"));
		RCO = Cast< UPowerSuitRCO>(Controller->RegisterRemoteCallObjectClass(UPowerSuitRCO::StaticClass()));
		if (!RCO)
		{
			UE_LOG(PowerSuit_Log, Error, TEXT("No RCO ! Fix this "));
			return;
		}
	}

};

void UEquipmentModuleComponent::Init( APowerSuit * Parent)
{

	UE_LOG(PowerSuit_Log, Display, TEXT("InitCall"));
	if (!Parent)
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("No Parent on InitCall"));

		return;
	}


	if (!Parent->GetInstigator())
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("No Instigator on InitCall"));
		return;
	}

	EquipmentParent = Parent;
	AFGCharacterPlayer* Character = Cast< AFGCharacterPlayer>(EquipmentParent->GetInstigator());
	MoveC = Character->GetFGMovementComponent();

	SetupSubModules();

	InventoryModule->InitInventory();

}



void UEquipmentModuleComponent::Client_FuseBreak_Implementation()
{
	nFuseBreak = FDateTime::Now();
	// TODO : Test if this is needed or clients & if this double executes on Server
	//if(!EquipmentParent->HasAuthority())
		//OnFuseTriggered.Broadcast(nProducing, nFuseBreak);
}
void UEquipmentModuleComponent::Client_FuseBreakOverDraw_Implementation()
{
	nFuseBreakOverDraw = FDateTime::Now();
	//if (!EquipmentParent->HasAuthority())
		//OnFuseOverDrawTriggered.BroadCast(false, nFuseBreakOverDraw);
}
//void UEquipmentModuleComponent::Client_FuseOverDrawRecover_Implementation()
//{
//	// Nothing needs to happen here
//}
void UEquipmentModuleComponent::Client_FuelFuseBreak_Implementation()
{
	nFuelFuseBreak = FDateTime::Now();
	nFuelFuseStatus = false;
	// TODO : Test if this is needed or clients & if this double executes on Server just like FuseBreak
}
void UEquipmentModuleComponent::Client_FuelFuseRecover_Implementation()
{
	nFuelFuseStatus = true;
	// TODO : Test if this is needed or clients & if this double executes on Server just like FuseBreak
}



void UEquipmentModuleComponent::Client_ShieldDmg_Implementation()
{
	nShieldDmg = FDateTime::Now();
}

float UEquipmentModuleComponent::CalculateDamage(float DmgIn, int32 Type, TSubclassOf<UFGDamageType> BpType, FHitResult Impact)
{
	float Dmg = DmgIn;
	if (nInventory)
	{

		int32 Hits = 0;
		int32 Reduction = 0; 
		int32 ReductionPercent = 0; 

		for (uint8 flag = ENDamageType::DamageTypeNULL; flag != ENDamageType::DamageTypeMAX; flag++)
		{
			
			if (flag & Type)
			{
				if(Stats.nDamageTypeResistance.Contains(static_cast<ENDamageType>(flag)))
				{
					Reduction =+ Stats.nDamageTypeResistance[static_cast<ENDamageType>(flag)].Modifier;
					ReductionPercent =+ Stats.nDamageTypeResistance[static_cast<ENDamageType>(flag)].Multiplier;
					Hits++;
				}
			}
		}


		if (BpType)
		{
			if (Stats.nDamageResistance.Contains(*BpType))
			{
				Dmg = FMath::Clamp((Dmg - Stats.nDamageResistance[*BpType].Modifier) - (FMath::Clamp(Dmg - Stats.nDamageResistance[*BpType].Modifier, 0.f, 1000.f)* Stats.nDamageResistance[*BpType].Multiplier), 0.f, 1000.f);
			}
		}
		float AdjustedDmg = Dmg;

		if (Hits > 0)
		{
			Reduction = FMath::Clamp(Reduction / Hits, 0, 1000000);
			ReductionPercent = FMath::Clamp(ReductionPercent / Hits, 0, 10000000);
			AdjustedDmg = FMath::Clamp((Dmg - Reduction) - (FMath::Clamp(Dmg - Reduction, 0.f, 1000.f)* ReductionPercent), 0.f, 1000.f);
		}
		
		if (nCurrentShield > 0)
		{
			if (Stats.HasDamageMask(Type))
			{
				return ShieldModule->ApplyShieldDamage(AdjustedDmg,Impact);
			}
		}
		return AdjustedDmg;
	}
	return Dmg;
}





void UEquipmentModuleComponent::RemoteInventoryRefresh_Implementation(bool IsAdd, TSubclassOf<UFGItemDescriptor> Class, int32 Amount)
{
	if (!EquipmentParent || !EquipmentParent->GetInstigator() || EquipmentParent->GetInstigator()->HasAuthority())
		return;

	if(Class)
	{
		if (IsAdd)
		{
			OnModuleInstalled.Broadcast(Class, nullptr);
			UE_LOG(PowerSuit_Log, Display, TEXT("Remote with RefreshInventoryAdd"))
		}
		else
		{
			OnModuleUnInstalled.Broadcast(Class, nullptr);
			UE_LOG(PowerSuit_Log, Display, TEXT("Remote with RefreshInventoryRemove"));
		}
	}
	
	UE_LOG(PowerSuit_Log, Display, TEXT("Remote Refresh Scheduled for 0.2 sec"));

	// Used by EquipmentModuleComponent to delay the update of remote suit inventories. Intentionally allows new executions to cancel the old ones.

	DelayedRemoteInventoryRefresh_TimerDel.BindUFunction(InventoryModule, FName("RefreshInventory"));

	GetWorld()->GetTimerManager().SetTimer(DelayedRemoteInventoryRefresh_TimerHandle, DelayedRemoteInventoryRefresh_TimerDel, .2f, false);

}


void UEquipmentModuleComponent::RemoteUpdateMovementGround_Implementation(const float Speed)
{
	if(!MoveC || !EquipmentParent)
		return;
	

	EquipmentParent->OnMovementModeChanged.Broadcast(EMovementMode::MOVE_Custom, 0, EMovementMode::MOVE_Custom, 0);

	MoveC->mMaxSprintSpeed = Speed;
}

void UEquipmentModuleComponent::RemoteUpdateMovementAir_Implementation(const float Speed, const float Accel)
{
	if(!MoveC || !EquipmentParent)
		return;
	
	EquipmentParent->OnMovementModeChanged.Broadcast(EMovementMode::MOVE_Custom, 0, EMovementMode::MOVE_Custom, 4);
	EquipmentParent->mHoverSpeed = Speed;
	MoveC->mMaxSprintSpeed = Speed;
	EquipmentParent->mHoverAccelerationSpeed = Accel;
	UE_LOG(PowerSuit_Log, Display, TEXT("Remote updated flight props with HoverMovementSpeedAdded: %f HoverMovementSpeedAccelAdded: %f"), Speed, Accel);
}

FModMultProperty UEquipmentModuleComponent::GetSuitPropertySafe(ESuitProperty prop) const
{
	if(!Stats.nSuitProperties.Contains(prop))
		return FModMultProperty();
	else
		return *Stats.nSuitProperties.Find(prop);
	
}

FModMultProperty UEquipmentModuleComponent::GetMovementPropertySafe(ESuitMovementProperty prop) const
{
	if (!Stats.nMovementProperties.Contains(prop))
	{
		return FModMultProperty();
	}
	return *Stats.nMovementProperties.Find(prop);
}

FModMultProperty UEquipmentModuleComponent::GetFlightPropertySafe(ESuitFlightProperty prop) const
{
	if (!Stats.nFlightProperties.Contains(prop))
		return FModMultProperty();

	return *Stats.nFlightProperties.Find(prop);
}

FModMultProperty UEquipmentModuleComponent::GetNamedPropertySafe(FName prop) const
{
	if (Stats.nNamedProperties.Contains(prop))
		return *Stats.nNamedProperties.Find(prop);
	else
		return FModMultProperty();
}

FModMultProperty UEquipmentModuleComponent::GetStatePropertySafe(TMap<TEnumAsByte<EPowerSuitState>, FModMultProperty> Map, const EPowerSuitState Prop)
{
	if (Map.Contains(Prop))
		return *Map.Find(Prop);
	else
		return FModMultProperty();
}


void UEquipmentModuleComponent::ResetStats(bool Notify)
{
	if (!EquipmentParent ||EquipmentParent->IsPendingKill() || !EquipmentParent->GetInstigator() || EquipmentParent->GetInstigator()->IsPendingKill())
		return;
	UE_LOG(PowerSuit_Log, Display, TEXT("Resetting Stats"));
	AttachmentModule->ResetAttachments();

	// Reset everything
	Stats = DefaultStats;
	if(EquipmentParent->HasAuthority())
	{
		nCurrentShield = 0.f;
		HoverMovementSpeedAdded = 0.f;
		HoverMovementSpeedAccelAdded = 0.f;
	}

	InventoryModule->ResetInventoryStats();

	UPowerSuitBPLibrary::UpdateAllNoRefresh(EquipmentParent, Notify);
}

void UEquipmentModuleComponent::PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion)
{
}

void UEquipmentModuleComponent::PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion)
{

}

void UEquipmentModuleComponent::PreLoadGame_Implementation(int32 saveVersion, int32 gameVersion)
{

}

void UEquipmentModuleComponent::PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion)
{
}

void UEquipmentModuleComponent::GatherDependencies_Implementation(TArray<UObject*>& out_dependentObjects)
{
	
}

bool UEquipmentModuleComponent::ShouldSave_Implementation() const
{
	return true;
}

bool UEquipmentModuleComponent::NeedTransform_Implementation()
{
	return false;
}



bool UEquipmentModuleComponent::VerifyItem(TSubclassOf< UFGItemDescriptor > ItemClass, int32 Amount) const
{
	if (SuitState == EPowerSuitState::PS_REBOOTING)
	{
		OnInventoryDropFail.Broadcast(ItemClass, Amount, 2);
		return false;
	}
	
	if (ItemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()))
	{
		// ReSharper disable once CppMsExtDoubleUserConversionInCopyInit
		const TSubclassOf<class UEquipmentModuleDescriptor> Item = ItemClass;
		if (!UEquipmentModuleDescriptor::IsAllowedByUnique(Item,InventoryModule->UniquesActive))
		{
			OnInventoryDropFail.Broadcast(ItemClass, Amount,0);
			return false;
		}
		return true;
	}
	else
	{
		OnInventoryDropFail.Broadcast(ItemClass, Amount,1);
		return false;
	}
}
