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


	if (!Producing)
	{
		if (timesinceFuseBreak.GetTotalSeconds() > GetFuseTimerDuration())
		{
			if (Stats.nPowerProduction > 0)
			{
				CurrentPower = FMath::Clamp((Stats.nPowerProduction* GetFuseTimerDuration() / Stats.nPowerCapacity),0.01f,.5f);
				Producing = true;
				if (Active)
				{
					// this will reactivate itself on next frame if we dont offset the Timer
					// we do this to prevent to trigger the Fuse instantly again 
					// maybe should solve this better
					ActiveTimer = ActiveTimer - FTimespan(0, 0, GetActiveTimerDuration());
					Active = false;
				}
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
	CurrentShield = CurrentShield + (GetShieldRechargeRate() * DeltaTime);
	if (!Active)
		ActiveTimer = FDateTime::Now();
	ShieldBroke = false;
}

void UEquipmentModuleComponent::RegenPower(float DeltaTime) {
	
	CurrentPower = FMath::Clamp(CurrentPower + ((GetPowerDraw() / Stats.nPowerCapacity)*DeltaTime), 0.f, 1.f);
	
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
				HealthBuffer = HealthBuffer + ((Stats.nHealthRegen * (1 + Stats.nHealthRegenMult)) * DeltaTime);
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
	if (!Producing || CurrentPower <= 0)
		pen = Stats.nEmtpyPowerFuelPenalty;
	if (MJcurrent > 0)
		Cast<APowerSuit>(EquipmentParent)->FuelAmount = FMath::Clamp(((MJcurrent - ((Stats.nBaseFuelConsumption +pen+ AddedDeltaFuel)*DeltaTime)) / MJ), 0.f, 1.f);
}

bool UEquipmentModuleComponent::IsProducing() {
	if (CurrentPower <= 0.f )
	{

		if (Cast<APowerSuit>(EquipmentParent)->FuelAmount > 0)
			Producing = true;
		else
			Producing = false;

		if (FTimespan(FDateTime::Now() - FuseBreak).GetTotalSeconds() > GetFuseTimerDuration())
		{
			FuseBreak = FDateTime::Now();
			FuseTriggered(this, Producing, FuseBreak);
			OnFuseTriggered.Broadcast(Producing, FuseBreak);
			ShieldDmg = FDateTime::Now();
		}

		
		
		return false;
	}
	else
		return true;
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

	Stats = DefaultStats;
	UniquesActive = {};
	CurrentShield = 0.f;
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
					if (EquipmentParent)
					{
						Cast<APowerSuit>(EquipmentParent)->OnChangeDampCurve.Broadcast(item.GetDefaultObject()->EquipmentStats.nDampeningCurve);
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
			// fine?
			heatlhcomp->mMaxHealth = GetNewMaxHealth();
		}
		UFGCharacterMovementComponent  * MovementComponent = Cast< AFGCharacterPlayer>(EquipmentParent->Instigator)->GetFGMovementComponent();


		MovementComponent->JumpZVelocity = Stats.JumpZVelocity + 500.f;
		MovementComponent->JumpOffJumpZFactor = Stats.JumpOffJumpZFactor + 0.5f;
		MovementComponent->GroundFriction = Stats.GroundFriction + 8.0f;
		MovementComponent->MaxWalkSpeed = Stats.MaxWalkSpeed + 500.0f;
		MovementComponent->MaxWalkSpeedCrouched = Stats.MaxWalkSpeedCrouched + 240.0f;
		MovementComponent->MaxSwimSpeed = Stats.MaxSwimSpeed + 300.0f;
		MovementComponent->BrakingFrictionFactor = Stats.BrakingFrictionFactor + 2.f;
		MovementComponent->BrakingFriction = Stats.BrakingFriction;
		MovementComponent->BrakingDecelerationWalking = Stats.BrakingDecelerationWalking + 2048.0;
		MovementComponent->BrakingDecelerationSwimming = Stats.BrakingDecelerationSwimming + 100.0;
		MovementComponent->BrakingDecelerationFalling = Stats.BrakingDecelerationFalling;
		MovementComponent->FallingLateralFriction = Stats.FallingLateralFriction;

		MovementComponent->GravityScale = Stats.nGravityMod + 1.2f;

		MovementComponent->mClimbSpeed = Stats.mClimbSpeed + 500.f;
		MovementComponent->mMaxSprintSpeed = Stats.mMaxSprintSpeed + 900.f;
		//  Tick overrides ref->mMaxSlideAngle = Stats.mMaxSlideAngle + 1.64999997615814f;
		MovementComponent->mBoostJumpZMultiplier = Stats.mBoostJumpZMult + 1.5f;
		MovementComponent->mBoostJumpVelocityMultiplier = Stats.mBoostJumpVelocityMult + 1.29999995231628f;

	}
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
				float AdjustedDmg = FMath::Clamp((Dmg - Stats.nFallDamageMod) - (FMath::Clamp(Dmg - Stats.nFallDamageMod, 0.f, 1000.f) * Stats.nFallDamageResistance), 0.f, 1000.f);
				if (CurrentShield > 0)
				{
					SML::Logging::log(SML::Logging::Error, "DamageTypeFallDamage");
					SML::Logging::log(SML::Logging::Error, AdjustedDmg);

					return ApplyShieldDamage(AdjustedDmg);
				}
				else
					return AdjustedDmg;
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

void UEquipmentModuleComponent::AddDeltaPower(float PowerConsumption)
{
	if (AddedDeltaPower != PowerConsumption)
		AddedDeltaPower = PowerConsumption;
}

float UEquipmentModuleComponent::GetPowerDraw()
{
	if (!EquipmentParent)
		return 0;

	float PowerUsage = Stats.nPowerProduction;

	if (Active)
		PowerUsage += Stats.nPowerProductionActive;

	if (Cast<APowerSuit>(EquipmentParent)->FuelAmount == 0)
		PowerUsage -= Stats.nEmtpyFuelPowerPenalty;

	return PowerUsage - AddedDeltaPower;
}

