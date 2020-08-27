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


// Called every frame
void UEquipmentModuleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!EquipmentParent)
		return;
	if (!EquipmentParent->IsEquipped())
		return;

	//if (EquipmentParent->HasAuthority())
	//{
	if (EquipmentParent->HasAuthority())
	{

		FTimespan timesinceFuseBreak = FDateTime::Now() - FuseBreak;
		FTimespan timesinceActiveTimer = FDateTime::Now() - ActiveTimer;
		FTimespan timesinceShieldDmg = FDateTime::Now() - ShieldDmg;

		if (!Producing)
		{
			if (timesinceFuseBreak.GetTotalSeconds() > FuseTimeOutTime)
			{
				if (Stats.nPowerProduction <= PowerProduction)
				{
					CurrentPower = 0.1f;
					Producing = true;
				}
			}
		}

		if (timesinceActiveTimer.GetTotalSeconds() > ActiveTimerDuration)
			Active = false;
		else
			Active = true;

		if (Producing)
		{
			if (CurrentShield < GetMaxShield())
			{
				if (Active || CurrentPower > 0.2f)
				{
					float valsec = FMath::Clamp(ShieldRechargeDelay, 0.f, ShieldRechargeDelay);
					if (timesinceShieldDmg.GetTotalSeconds() > valsec)
					{
						CurrentShield = CurrentShield + (ShieldRecharge * DeltaTime);
						if (!Active)
							ActiveTimer = FDateTime::Now();
						ShieldBroke = false;
					}
					else
					{
						ShieldBroke = true;
					}
				}
				
			}


			if (CurrentPower > 0.f && CurrentPower <= 1.f)
			{
				float PowerUsage = 0.f;
				if (Active)
					PowerUsage = (Stats.nPowerProduction + Stats.nPowerProductionActive);
				else
					PowerUsage = Stats.nPowerProduction ;

				CurrentPower = FMath::Clamp(CurrentPower + (((PowerUsage - AddedDeltaPower) / PowerCapacity)*DeltaTime), 0.f, 1.f);
			}

			if (CurrentPower <= 0.f)
			{
				FuseBreak = FDateTime::Now();
				ShieldDmg = FDateTime::Now();

				Producing = false;
				FuseTriggered(this, Producing, FuseBreak);
				OnFuseTriggered.Broadcast(Producing, FuseBreak);
				return;
			}


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
						HealthBuffer = HealthBuffer + (Stats.nHealthRegen * DeltaTime);
					}

				}
			}
		}

	}
}


void UEquipmentModuleComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEquipmentModuleComponent, CurrentPower);
	DOREPLIFETIME(UEquipmentModuleComponent, Active);
	DOREPLIFETIME(UEquipmentModuleComponent, Producing);
	DOREPLIFETIME(UEquipmentModuleComponent, CurrentShield);
	DOREPLIFETIME(UEquipmentModuleComponent, AddedDeltaPower);
	DOREPLIFETIME(UEquipmentModuleComponent, ShieldRechargeDelay);
	DOREPLIFETIME(UEquipmentModuleComponent, ShieldDmg);
	DOREPLIFETIME(UEquipmentModuleComponent, ActiveTimer);
	DOREPLIFETIME(UEquipmentModuleComponent, FuseBreak);

	
	
}


void UEquipmentModuleComponent::UpdateStats( UFGInventoryComponent * Inventory)
{

	Stats = BaseStats;
	UniquesActive = {};
	CurrentShield = 0.01f;
	Stats.nHasPipeAccel = false;


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
				Stats = Stats + item.GetDefaultObject()->EquipmentStats;

				if (EquipmentParent && item.GetDefaultObject()->EquipmentStats.nDampeningCurve)
				{
					if (Cast<APowerSuit>(EquipmentParent))
					{
						APowerSuit * Parentref = Cast<APowerSuit>(EquipmentParent);
						Parentref->OnChangeDampCurve.Broadcast(item.GetDefaultObject()->EquipmentStats.nDampeningCurve);
					}
					Stats.nDampeningCurve = item.GetDefaultObject()->EquipmentStats.nDampeningCurve;

				}
			}
		}
	}
	if (EquipmentParent->HasAuthority() && EquipmentParent->Instigator)
	{
		class UFGHealthComponent* heatlhcomp = Cast< AFGCharacterPlayer>(EquipmentParent->Instigator)->GetHealthComponent();
		if (heatlhcomp)
		{
			// Does this even work ?  we maybe have to keep track of maxhealth default value set
			heatlhcomp->ResetHealth();
			if (heatlhcomp->mMaxHealth > 0)
				heatlhcomp->mMaxHealth = (heatlhcomp->mMaxHealth + Stats.nHealthModifier)*(1+Stats.nHealthMultiplier);
		}
		UFGCharacterMovementComponent  * ref = Cast< AFGCharacterPlayer>(EquipmentParent->Instigator)->GetFGMovementComponent();
		
		
		ref->JumpZVelocity = Stats.JumpZVelocity + 500.f;
		ref->JumpOffJumpZFactor = Stats.JumpOffJumpZFactor + 0.5f;
		ref->GroundFriction = Stats.GroundFriction + 8.0f;
		ref->MaxWalkSpeed = Stats.MaxWalkSpeed + 500.0f;
		ref->MaxWalkSpeedCrouched = Stats.MaxWalkSpeedCrouched + 240.0f;
		ref->MaxSwimSpeed = Stats.MaxSwimSpeed + 300.0f;
		ref->BrakingFrictionFactor = Stats.BrakingFrictionFactor +2.f;
		ref->BrakingFriction = Stats.BrakingFriction;
		ref->BrakingDecelerationWalking = Stats.BrakingDecelerationWalking  + 2048.0;
		ref->BrakingDecelerationSwimming = Stats.BrakingDecelerationSwimming + 100.0;
		ref->BrakingDecelerationFalling = Stats.BrakingDecelerationFalling;
		ref->FallingLateralFriction = Stats.FallingLateralFriction;

		ref->GravityScale = Stats.nGravityModifier + 1.2f;

		ref->mClimbSpeed = Stats.mClimbSpeed + 500.f;
		ref->mMaxSprintSpeed = Stats.mMaxSprintSpeed + 900.f;
		//  auto override ref->mMaxSlideAngle = Stats.mMaxSlideAngle + 1.64999997615814f;
		ref->mBoostJumpZMultiplier = Stats.mBoostJumpZMultiplier + 1.5f;
		ref->mBoostJumpVelocityMultiplier = Stats.mBoostJumpVelocityMultiplier + 1.29999995231628f;

	}
	
	FuseTimeOutTime = FMath::Clamp(Stats.nFuseTimeMod * (1 + Stats.nFuseTimeMult), 0.f, 10000.f);
	ActiveTimerDuration = FMath::Clamp(Stats.nActiveTimerMod*(1 + Stats.nActiveTimerMult), 0.f, 10000.f);
	ShieldRechargeDelay = FMath::Clamp(Stats.nShieldRegarcheDelayMod*(1 + Stats.nShieldRegarcheDelayMult), 0.f, 10000.f);
	PowerCapacity = Stats.nPowerCapacity;
	PowerProduction = Stats.nPowerProduction;

	
	ShieldRecharge = FMath::Clamp(((Stats.nShieldRegen) * (1+Stats.nShieldRegenMultiplier)), 0.f,10000.f);
	FuseTimeOutTime = FMath::Clamp(Stats.nFuseTimeMod * (1 + Stats.nFuseTimeMult), 0.f, 100.f);
	ActiveTimerDuration = FMath::Clamp(Stats.nActiveTimerMod *(1 + Stats.nActiveTimerMult), 0.f, 100.f);
	ShieldRechargeDelay = FMath::Clamp(Stats.nShieldRegarcheDelayMod *(1+ Stats.nShieldRegarcheDelayMult), 0.f, 100.f);
}

void  UEquipmentModuleComponent::OnInventoryItemAdd(TSubclassOf< UFGItemDescriptor > itemClass, int32 numAdded)
{
	if (EquipmentParent && nInventory)
		UpdateStats(nInventory);
}

void  UEquipmentModuleComponent::OnInventoryItemRemove(TSubclassOf< UFGItemDescriptor > itemClass, int32 numAdded)
{
	if (EquipmentParent && nInventory)
		UpdateStats(nInventory);
}

void UEquipmentModuleComponent::GatherEquipmentModInventory()
{
	TArray<UActorComponent*> AActors = EquipmentParent->GetComponentsByClass(UFGInventoryComponent::StaticClass());
	if (AActors.Num() == 0)
		return;
	for (int32 i = 0; i < AActors.Num(); i++)
	{
		if (Cast<UFGInventoryComponent>(AActors[i])->GetName() == "ModInventory")
		{
			nInventory = Cast<UFGInventoryComponent>(AActors[i]);
			break;
		}
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

		if(Inventory->GetSizeLinear() != Stats.InventorySlots)
			Inventory->Resize(Stats.InventorySlots);
		if(!Inventory->GetIsReplicated())
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

float UEquipmentModuleComponent::ApplyShieldDamage(float DmgIn)
{
	

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
				Dmg = FMath::Clamp(Dmg - (Dmg * (*Stats.nDamageResistance.Find(*BpType))), 0.f, 1000.f);
			}
		}
		

		switch (Type)
		{
			case ENDamageType::DamageTypeFallDamage :
			{
				if (CurrentShield > 0)
				{
					float AdjustedDmg = FMath::Clamp(Dmg - (Dmg * Stats.nFallDamageModifier), 0.f, 1000.f);
					SML::Logging::log(SML::Logging::Error, "DamageTypeFallDamage");
					SML::Logging::log(SML::Logging::Error, AdjustedDmg);

					return ApplyShieldDamage(AdjustedDmg);
				}
				else
					return FMath::Clamp(Dmg - (Dmg * Stats.nFallDamageModifier), 0.f, 1000.f);
			}
			case ENDamageType::DamageTypeGas :
			{
				if (CurrentShield > 0)
				{
					float AdjustedDmg = FMath::Clamp(Dmg - (Dmg * Stats.nGasResistance), 0.f, 1000.f);
					SML::Logging::log(SML::Logging::Error, "DamageTypeGas");
					SML::Logging::log(SML::Logging::Error, AdjustedDmg);
					return ApplyShieldDamage(AdjustedDmg);
				}
				else
					return FMath::Clamp(Dmg - (Dmg * Stats.nGasResistance), 0.f, 1000.f);
			}
			case ENDamageType::DamageTypePhysical :
			{
				if (CurrentShield > 0)
				{

					float AdjustedDmg = FMath::Clamp(Dmg - (Dmg * Stats.nPhysicalResistance), 0.f, 1000.f);
					SML::Logging::log(SML::Logging::Error, "DamageTypePhysical");
					SML::Logging::log(SML::Logging::Error, AdjustedDmg);
					return ApplyShieldDamage(AdjustedDmg);
				}
				else
					return FMath::Clamp(Dmg - (Dmg * Stats.nPhysicalResistance), 0.f, 1000.f);
			}
			case ENDamageType::DamageTypeNULL:
			{
				if (CurrentShield > 0)
				{
					float AdjustedDmg = FMath::Clamp(Dmg - (Dmg * Stats.nPhysicalResistance), 0.f, 1000.f);
					SML::Logging::log(SML::Logging::Error, "DamageTypeNULL");
					SML::Logging::log(SML::Logging::Error, AdjustedDmg);
					return ApplyShieldDamage(AdjustedDmg);
				}
				else
					return FMath::Clamp(Dmg - (Dmg * Stats.nPhysicalResistance), 0.f, 1000.f);
			}
			case ENDamageType::DamageTypeRadioActivity:
			{
				SML::Logging::log(SML::Logging::Error, "DamageTypeRadioActivity");
				SML::Logging::log(SML::Logging::Error, FMath::Clamp(Dmg - (Dmg * Stats.nRadioactiveRestistance), 0.f, 1000.f));
				return FMath::Clamp(Dmg - (Dmg * Stats.nRadioactiveRestistance), 0.f, 1000.f);
			}

		}

	}
	return Dmg;
}

void UEquipmentModuleComponent::AddDeltaPower(float delta, float PowerConsumption)
{
	
	if (AddedDeltaPower != PowerConsumption)
		AddedDeltaPower = PowerConsumption;
	
}

