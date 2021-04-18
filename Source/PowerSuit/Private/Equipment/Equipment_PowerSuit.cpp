
#include "Equipment/Equipment_PowerSuit.h"

#include "FGGameState.h"
#include "Buildables/FGBuildableConveyorBelt.h"
#include "Buildables/FGBuildableRailroadTrack.h"


#include "FGCharacterMovementComponent.h"
#include "SubModules/EMC_FuelModule.h"
#include "SubModules/EMC_ShieldModule.h"
#include "SubModules/EMC_PowerModule.h"
#include "SubModules/EMC_HealthModule.h"
#include "SubModules/EMC_StateModule.h"

#include "FactoryGame.h"

APowerSuit::APowerSuit()
{
	this->Module = CreateDefaultSubobject<UEquipmentModuleComponent>(TEXT("EquipmentModule")); this->Module->SetupAttachment(this->RootComponent);
	this->Module->SetNetAddressable(); // Make net addressable
	this->Module->SetIsReplicated(true);

}


void  APowerSuit::Equip(class AFGCharacterPlayer* character){
	Super::Equip(character);
	if (character)
	{
		if (character->HasAuthority())
		{
			if (Module)
			{
				Module->Init(this);
			}
		}
	}
};






void  APowerSuit::UnEquip()
{
	Super::UnEquip();
	if (Module)
	{
		Module->ResetStats();
	}
};



void APowerSuit::Tick(float DeltaSeconds)
{
	
	if (!GetInstigator())
		return;

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

	if((mCurrentPowerLevel_ / mPowerCapacity_) - (Module->PowerModule->CurrentPower/ mPowerCapacity_) > 0.7f)
		mCurrentPowerLevel_ = FMath::Clamp(Module->PowerModule->CurrentPower, 0.f, mPowerCapacity_);


}

void APowerSuit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

