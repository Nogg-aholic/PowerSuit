
#include "Equipment/Equipment_PowerSuit.h"

#include "FGGameState.h"
#include "Buildables/FGBuildableConveyorBelt.h"
#include "Buildables/FGBuildableRailroadTrack.h"
#include "FGPowerConnectionComponent.h"

#include "FGCharacterMovementComponent.h"
#include "SubModules/EMC_FuelModule.h"
#include "SubModules/EMC_ShieldModule.h"
#include "SubModules/EMC_PowerModule.h"
#include "SubModules/EMC_HealthModule.h"
#include "SubModules/EMC_StateModule.h"

#include "FactoryGame.h"

APowerSuit::APowerSuit()
{
	this->Module = CreateDefaultSubobject<UEquipmentModuleComponent>(TEXT("EquipmentModule")); 
	this->SetRootComponent(this->Module);
	this->Module->SetNetAddressable(); // Make net addressable
	this->Module->SetIsReplicated(true);

}



void APowerSuit::BeginPlay()
{
	UE_LOG(PowerSuit_Log, Display, TEXT("**************** PowerSuit Spawn ****************\n %s"), *GetName());
	Super::BeginPlay();
}

void APowerSuit::Destroyed()
{
	UE_LOG(PowerSuit_Log, Display, TEXT("**************** PowerSuit Destroyed ****************\n %s"), *GetName());
	Super::Destroyed();
}

void  APowerSuit::Equip(class AFGCharacterPlayer* character){
	UE_LOG(PowerSuit_Log, Display, TEXT("**************** PowerSuit Equip ****************\n %s"), *GetName());
	SetInstigator(nullptr);
	Super::Equip(character);
	SetInstigator(character);
	if (character)
	{
		if (character->HasAuthority())
		{
			if (Module)
			{
				Module->Init(this);
				//WasEquipped();
			}
		}
	}
};






void  APowerSuit::UnEquip()
{	
	UE_LOG(PowerSuit_Log, Display, TEXT("**************** PowerSuit UnEquip ****************\n %s"), *GetName());
	if (Module)
	{
		Module->ResetStats();
	}
	Super::UnEquip();
};



void APowerSuit::Tick(float DeltaSeconds)
{
	
	if (!GetInstigator())
		return;

	// incase our instigator died we want to prevent the suit from being deleted even if instigator goes nullptr
	// the next pickup would overwrite instigator again so temporairly setting it to something valid hopefully will prevent
	// the suit from being cleaned up on a players Death
	if (GetInstigator()->IsPendingKill())
	{
		if (UWorld* World = GetWorld())
		{
			for (TActorIterator<AFGCharacterPlayer> It(World, AFGCharacterPlayer::StaticClass()); It; ++It)
			{
				if (It)
				{
					if (!It->IsPendingKill())
					{
						SetInstigator(*It);
						return;
					}
				}
			}
		}
	}

	if (!IsEquipped() || !Module)
		return;

	Module->EquippedTick(DeltaSeconds);
	
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
		return;

#ifdef MODDING_SHIPPING
	float & mCurrentPowerLevel_ = mCurrentPowerLevel;
	float & mPowerCapacity_ =mPowerCapacity;
#else
	float & mCurrentPowerLevel_ = this->*get(steal_mCurrentPowerLevel());
	float & mPowerCapacity_ = this->*get(steal_mPowerCapacity());
#endif

	if((mCurrentPowerLevel_ / mPowerCapacity_) - (Module->nCurrentPower/ mPowerCapacity_) > 0.7f)
		mCurrentPowerLevel_ = FMath::Clamp(Module->nCurrentPower, 0.f, mPowerCapacity_);


}

void APowerSuit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool APowerSuit::ShouldSaveState() const
{
	return true;
}

