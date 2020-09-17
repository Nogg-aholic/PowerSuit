// ILikeBanas


#include "EquipmentModuleComponent.h"
#include "PowerSuit.h"
#include "FGCharacterMovementComponent.h"
#include "util/Logging.h"

// Sets default values for this component's properties
UEquipmentModuleComponent::UEquipmentModuleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEquipmentModuleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UEquipmentModuleComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEquipmentModuleComponent, CurrentPower);
	DOREPLIFETIME(UEquipmentModuleComponent, Active);
	DOREPLIFETIME(UEquipmentModuleComponent, Producing);
	DOREPLIFETIME(UEquipmentModuleComponent, CurrentShield);
	DOREPLIFETIME(UEquipmentModuleComponent, AddedDeltaPower);
	DOREPLIFETIME(UEquipmentModuleComponent, ShieldDmg);
	DOREPLIFETIME(UEquipmentModuleComponent, ActiveTimer);
	DOREPLIFETIME(UEquipmentModuleComponent, FuseBreak);
	DOREPLIFETIME(UEquipmentModuleComponent, FuseBreakOverDraw);

}

// Called every frame
void UEquipmentModuleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!EquipmentParent)
		return;
	if (!EquipmentParent->IsEquipped())
		return;

	if (!EquipmentParent->HasAuthority())
		return;



	FTimespan timesinceFuseBreak = FDateTime::Now() - FuseBreak;
	FTimespan timesinceActiveTimer = FDateTime::Now() - ActiveTimer;
	FTimespan timesinceShieldDmg = FDateTime::Now() - ShieldDmg;

	/*
	 Order is important here 
	 first we check if the Active Timer Should be Triggered
	 then we check if the FuseTimer has run out so we can restart Power Production 
	*/


	if (timesinceActiveTimer.GetTotalSeconds() > GetActiveTimerDuration())
		Active = false;
	else
		Active = true;

	APowerSuit * Parent = Cast<APowerSuit>(EquipmentParent);
	Parent->HandlePowerConsumption(DeltaTime);
	Parent->HandleFuelConsumption(DeltaTime);
	if (!Producing)
	{
		if (timesinceFuseBreak.GetTotalSeconds() > GetFuseTimerDuration())
		{
			if (Stats.nSuitProperties[ESuitProperty::nPowerProduction].result() > 0)
			{
				//CurrentPower = FMath::Clamp((Stats.nSuitProperties[ESuitProperty::nPowerProduction].result()* GetFuseTimerDuration() / Stats.nSuitProperties[ESuitProperty::nPowerCapacity].result()),0.01f,.5f);
				CurrentPower = 0.01f;
				Producing = true;
				UFGCharacterMovementComponent  * MovementComponent = Cast< AFGCharacterPlayer>(EquipmentParent->Instigator)->GetFGMovementComponent();
				MovementComponent->Velocity  = FVector(MovementComponent->Velocity.X, MovementComponent->Velocity.Y, 0);
				if (Active)
				{
					// this will reactivate itself on next frame if we dont offset the Timer
					// we do this to prevent to trigger the Fuse instantly again 
					// maybe should solve this better
					ActiveTimer = ActiveTimer - FTimespan(0, 0, GetActiveTimerDuration());
					Active = false;
					
				}
				UpdateStats(nInventory);
			}
		}
	}

	if (Producing)
	{
		
		if (ShouldRegenShield(timesinceShieldDmg))
			RegenShield(DeltaTime);
		else if (CurrentShield == GetMaxShield())
			ShieldBroke = false;
		else if(GetMaxShield() != 0)
			ShieldBroke = true;
		else 
			ShieldBroke = false;


		if (CurrentPower <= 1.f)
			RegenPower(DeltaTime);
		

		ConsumeFuel(DeltaTime);
			
		
		IsProducing();

		RegenHealth(DeltaTime);
	}

	
}

void UEquipmentModuleComponent::Init(UFGInventoryComponent * Inventory, AFGEquipment * Parent)
{
	if (!Parent || !Inventory)
	{
		return;
	}
	if (Parent->HasAuthority())
	{
		EquipmentParent = Parent;
		nInventory = Inventory;
		UpdateStats(Inventory);
		if (Stats.InventorySlots == 0)
			return;

		if (Inventory->GetSizeLinear() != Stats.InventorySlots)
			Inventory->Resize(Stats.InventorySlots);
		if (!Inventory->GetIsReplicated())
			Inventory->SetIsReplicated(true);

		for (int32 i = 0; i < Stats.InventorySlots; i++)
		{
			Inventory->AddArbitrarySlotSize(i, 1);
			// ? this does not work .why?
			Inventory->SetAllowedItemOnIndex(i, TSubclassOf<class UEquipmentModuleDescriptor>());
		}
	}
	else
	{
		nInventory = Inventory;
		EquipmentParent = Parent;
		UpdateStats(Inventory);

	}

	Inventory->OnItemAddedDelegate.AddDynamic(this, &UEquipmentModuleComponent::OnInventoryItemAdd);
	Inventory->OnItemRemovedDelegate.AddDynamic(this, &UEquipmentModuleComponent::OnInventoryItemRemove);

	if (Parent->IsEquipped())
		SetComponentTickEnabled(true);
	else
		SetComponentTickEnabled(false);

}

void UEquipmentModuleComponent::RegenShield(float DeltaTime) {
	CurrentShield = FMath::Clamp(CurrentShield + (GetShieldRechargeRate() * DeltaTime),0.f,GetMaxShield());
	if (!Active)
		ActiveTimer = FDateTime::Now();
	ShieldBroke = false;
}

void UEquipmentModuleComponent::RegenPower(float DeltaTime) {
	
	CurrentPower = FMath::Clamp(CurrentPower + ((GetPowerDraw() / FMath::Clamp(Stats.nSuitProperties[ESuitProperty::nPowerCapacity].result(),1.f,100000000.f))*DeltaTime), 0.f, 1.f);
	if(CurrentPower == 0.f)
		if (FTimespan(FDateTime::Now() - FuseBreakOverDraw).GetTotalSeconds() > Stats.nSuitProperties[ESuitProperty::nFuseTimeOverDraw].result() +1)
			FuseBreakOverDraw = FDateTime::Now();
		
}

void UEquipmentModuleComponent::RegenHealth(float DeltaTime) {
	class UFGHealthComponent* heatlhcomp = Cast< AFGCharacterPlayer>(EquipmentParent->Instigator)->GetHealthComponent();
	if (heatlhcomp)
	{
		if (heatlhcomp->GetCurrentHealth() < heatlhcomp->GetMaxHealth())
		{
			if (HealthBuffer > 1.f)
			{
				heatlhcomp->Heal(HealthBuffer);
				HealthBuffer = 0.f;
			}
			else
			{
				HealthBuffer = HealthBuffer + (Stats.nSuitProperties[ESuitProperty::nHealthRegen].result() * DeltaTime);
			}

		}
	}
}

void UEquipmentModuleComponent::ConsumeFuel(float DeltaTime) {

	if (!EquipmentParent)
		return;
	APowerSuit * Parent = Cast<APowerSuit>(EquipmentParent);
	if (!Parent)
		return;
	float MJ = 0.f; float MJcurrent = 0.f; float pen = 0.f; float FuelAmount = 0.f;
	FuelAmount = FMath::Clamp(Parent->FuelAmount,0.f,1.f);
	MJ = FMath::Clamp(Parent->nCurrentFuelItem.GetDefaultObject()->GetEnergyValue(Parent->nCurrentFuelItem),600.f,100000000.f);
	MJcurrent = MJ * FuelAmount;
	if (MJcurrent > 0)
		Cast<APowerSuit>(EquipmentParent)->FuelAmount = FMath::Clamp(((MJcurrent - (Stats.nSuitProperties[ESuitProperty::nBaseFuelConsumption].result() + AddedDeltaFuel) *DeltaTime) / MJ), 0.f, 1.f);
}

bool UEquipmentModuleComponent::IsProducing() {
	if (CurrentPower <= 0.f )
	{
		if (Cast<APowerSuit>(EquipmentParent)->FuelAmount > 0)
		{
			if (!Stats.nFuseNeverBreaksWhenFueled)
			{
				if (FTimespan(FDateTime::Now() - FuseBreakOverDraw).GetTotalSeconds() > Stats.nSuitProperties[ESuitProperty::nFuseTimeOverDraw].result())
					Producing = false;
				else
					Producing = true;
			}
			else
				Producing = true;
		}	
		else
			Producing = false;
		if (!Producing)
		{
			if (FTimespan(FDateTime::Now() - FuseBreak).GetTotalSeconds() > GetFuseTimerDuration())
			{
				Cast<APowerSuit>(EquipmentParent)->SuitState = EPowerSuitState::STANDING;
				UFGCharacterMovementComponent  * MovementComponent = Cast< AFGCharacterPlayer>(EquipmentParent->Instigator)->GetFGMovementComponent();
				Cast<APowerSuit>(EquipmentParent)->ResetMovementComponent(MovementComponent);
				FuseBreak = FDateTime::Now();
				FuseTriggered(this, Producing, FuseBreak);
				OnFuseTriggered.Broadcast(Producing, FuseBreak);
				ShieldDmg = FDateTime::Now();
			}
		}
	}
	return Producing;
}

bool UEquipmentModuleComponent::ShouldRegenShield(FTimespan timesinceShieldDmg)
{
	if (!EquipmentParent)
		return false;
	if (timesinceShieldDmg.GetTotalSeconds() > GetShieldRechargeDelayDuration())
	{
		if (CurrentShield < GetMaxShield())
			if (Active || CurrentPower > 0.2f)
				if (CurrentPower != 0)
					return true;
				else if (Cast<APowerSuit>(EquipmentParent)->FuelAmount > 0)
					return true;
	}
	return false;
}


void UEquipmentModuleComponent::OnInventoryItemAdd(TSubclassOf< UFGItemDescriptor > itemClass, int32 numAdded)
{
	if (EquipmentParent && nInventory)
		UpdateStats(nInventory);
}

void UEquipmentModuleComponent::OnInventoryItemRemove(TSubclassOf< UFGItemDescriptor > itemClass, int32 numAdded)
{
	if (EquipmentParent && nInventory)
		UpdateStats(nInventory);
}

void UEquipmentModuleComponent::UpdateStats(UFGInventoryComponent * Inventory)
{

	Stats = DefaultStats + FEquipmentStats();
	UniquesActive = {};
	CurrentShield = 0.f;
	Stats.nHasPipeAccel = false;

	for (int32 i = 0; i< Attachments.Num(); i++)
	{
		if(Attachments[i])
			Attachments[i]->Destroy();
	}
	Attachments.Empty();

	if (Inventory)
	{
		TArray< FInventoryStack > out_stacks;
		Inventory->GetInventoryStacks(out_stacks);
		for (int32 i = 0; i < out_stacks.Num(); i++)
		{
			TSubclassOf< class UEquipmentModuleDescriptor> item = out_stacks[i].Item.ItemClass;
			if (item)
			{
				if (item.GetDefaultObject()->GetnUniqueUsage(item))
					if (UniquesActive.Contains(item))
						continue;
					else
						UniquesActive.Add(item);
				
				// Add Stas up 
				Stats = Stats + item.GetDefaultObject()->EquipmentStats;

				// Curve 
				if (EquipmentParent && item.GetDefaultObject()->EquipmentStats.nDampeningCurve)
				{
					if (EquipmentParent)
					{
						Cast<APowerSuit>(EquipmentParent)->OnChangeDampCurve.Broadcast(item.GetDefaultObject()->EquipmentStats.nDampeningCurve);
					}
					Stats.nDampeningCurve = item.GetDefaultObject()->EquipmentStats.nDampeningCurve;

				}

				// Attachment
				UClass* classref = item.GetDefaultObject()->GetnAttachment(item);
				if (EquipmentParent && classref && EquipmentParent->HasAuthority())
				{
					FVector  Loc = FVector();
					FRotator Rot = FRotator();
					AActor * spawned = GetWorld()->SpawnActor(classref, &Loc, &Rot);
					if (spawned)
					{
						spawned->AttachToActor(EquipmentParent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
						Attachments.Add(spawned);
					}
				}
			}
		}
	}
	if (EquipmentParent->HasAuthority() && EquipmentParent->Instigator)
	{
		class UFGHealthComponent* heatlhcomp = Cast< AFGCharacterPlayer>(EquipmentParent->Instigator)->GetHealthComponent();
		if (heatlhcomp)
		{
			// fine?
			heatlhcomp->mMaxHealth = GetNewMaxHealth();
		}
		UFGCharacterMovementComponent  * MovementComponent = Cast< AFGCharacterPlayer>(EquipmentParent->Instigator)->GetFGMovementComponent();


		MovementComponent->JumpZVelocity = Stats.nMovementComponentStats.JumpZVelocity + 500.f;
		MovementComponent->JumpOffJumpZFactor = Stats.nMovementComponentStats.JumpOffJumpZFactor + 0.5f;
		MovementComponent->GroundFriction = Stats.nMovementComponentStats.GroundFriction + 8.0f;
		MovementComponent->MaxWalkSpeed = (Stats.nMovementComponentStats.MaxWalkSpeed + 500.0f +Stats.nSuitProperties[ESuitProperty::nSpeed].Modifier)*(1 + Stats.nSuitProperties[ESuitProperty::nSpeed].Multiplier);
		MovementComponent->MaxWalkSpeedCrouched = (Stats.nMovementComponentStats.MaxWalkSpeedCrouched + 240.0f + Stats.nSuitProperties[ESuitProperty::nSpeed].Modifier)*(1 + Stats.nSuitProperties[ESuitProperty::nSpeed].Multiplier);
		MovementComponent->MaxSwimSpeed = (Stats.nMovementComponentStats.MaxSwimSpeed + 300.0f + Stats.nSuitProperties[ESuitProperty::nSpeed].Modifier)*(1 + Stats.nSuitProperties[ESuitProperty::nSpeed].Multiplier);
		MovementComponent->BrakingFrictionFactor = Stats.nMovementComponentStats.BrakingFrictionFactor + 2.f;
		MovementComponent->BrakingFriction = Stats.nMovementComponentStats.BrakingFriction;
		MovementComponent->BrakingDecelerationWalking = Stats.nMovementComponentStats.BrakingDecelerationWalking + 2048.0;
		MovementComponent->BrakingDecelerationSwimming = Stats.nMovementComponentStats.BrakingDecelerationSwimming + 100.0;
		MovementComponent->BrakingDecelerationFalling = Stats.nMovementComponentStats.BrakingDecelerationFalling;
		MovementComponent->FallingLateralFriction = Stats.nMovementComponentStats.FallingLateralFriction;

		MovementComponent->GravityScale = Stats.nSuitProperties[ESuitProperty::nGravity].result() + 1.2f;

		MovementComponent->mClimbSpeed = (Stats.nMovementComponentStats.mClimbSpeed + 500.f + Stats.nSuitProperties[ESuitProperty::nSpeed].Modifier)*(1 + Stats.nSuitProperties[ESuitProperty::nSpeed].Multiplier);
		MovementComponent->mMaxSprintSpeed = (Stats.nMovementComponentStats.mMaxSprintSpeed + 900.f + Stats.nSuitProperties[ESuitProperty::nSpeed].Modifier)*(1 + Stats.nSuitProperties[ESuitProperty::nSpeed].Multiplier);
		//  Tick overrides ref->mMaxSlideAngle = Stats.mMaxSlideAngle + 1.64999997615814f;
		MovementComponent->mBoostJumpZMultiplier = Stats.nMovementComponentStats.mBoostJumpZMult + 1.5f;
		MovementComponent->mBoostJumpVelocityMultiplier = Stats.nMovementComponentStats.mBoostJumpVelocityMult + 1.29999995231628f;

	}
}


float UEquipmentModuleComponent::ApplyShieldDamage(float DmgIn)
{
	
	if (DmgIn <= 0.f)
		return 0.f;

	ShieldDmg = FDateTime::Now();
	ActiveTimer = FDateTime::Now();
	if (CurrentShield - DmgIn > 0)
	{
		SML::Logging::log(SML::Logging::Error, CurrentShield);
		OnShieldDamageTaken.Broadcast(DmgIn-CurrentShield, ShieldDmg, true);
		CurrentShield = CurrentShield - DmgIn;
		SML::Logging::log(SML::Logging::Error, CurrentShield);
		SML::Logging::log(SML::Logging::Error, "Damage Absorbed fully");
		return 0;
	}
	else
	{
		float dmgout = DmgIn - CurrentShield;
		OnShieldDamageTaken.Broadcast(CurrentShield, ShieldDmg, true);
		CurrentShield = 0;
		SML::Logging::log(SML::Logging::Error, dmgout);
		return dmgout;
	}

}

float UEquipmentModuleComponent::CalculateDamage( UFGInventoryComponent * Inventory, float DmgIn, ENDamageType Type, TSubclassOf<class UFGDamageType>  BpType)
{
	float Dmg = DmgIn;
	if (Inventory)
	{
		//UpdateStats(selfref,Inventory);
		SML::Logging::log(SML::Logging::Error, Dmg);
		if (BpType)
		{
			if (Stats.nDamageResistance.Contains(*BpType))
			{
				Dmg = FMath::Clamp((Dmg - Stats.nDamageResistance[*BpType].Modifier) - (FMath::Clamp(Dmg - Stats.nDamageResistance[*BpType].Modifier, 0.f, 1000.f)* Stats.nDamageResistance[*BpType].Multiplier), 0.f, 1000.f);

			}
		}
		
		float AdjustedDmg = FMath::Clamp((Dmg - Stats.nDamageTypeResistance[Type].Modifier) - ( FMath::Clamp(Dmg - Stats.nDamageTypeResistance[Type].Modifier, 0.f, 1000.f)* Stats.nDamageTypeResistance[Type].Multiplier ), 0.f, 1000.f);
		if (CurrentShield > 0 && (Type != ENDamageType::DamageTypeGas || Type != ENDamageType::DamageTypeRadioActivity))
		{
			SML::Logging::log(SML::Logging::Error, Type);
			SML::Logging::log(SML::Logging::Error, AdjustedDmg);

			return ApplyShieldDamage(AdjustedDmg);
		}
		else
			return AdjustedDmg;

	}
	return Dmg;
}

void UEquipmentModuleComponent::AddDeltaPower(float PowerConsumption)
{
	if (AddedDeltaPower != PowerConsumption)
		AddedDeltaPower = PowerConsumption;
}

float UEquipmentModuleComponent::GetPowerDraw()
{
	if (!EquipmentParent)
		return 0;

	float PowerUsage = Stats.nSuitProperties[ESuitProperty::nPowerProduction].result();

	if (Active)
		PowerUsage += Stats.nSuitProperties[ESuitProperty::nPowerProductionActive].result();


	return PowerUsage - AddedDeltaPower;
}

FString UEquipmentModuleComponent::GetStatsAsString()
{
	
	UScriptStruct* Struct = Stats.StaticStruct();
	FString Output = TEXT("");
	Struct->ExportText(Output, &Stats, nullptr, this, (PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient || PPF_ParsingDefaultProperties), nullptr);

	
	return Output;
}


