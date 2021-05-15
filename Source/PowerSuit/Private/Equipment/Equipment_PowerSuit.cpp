
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
	this->Module->SetupAttachment(this->RootComponent);
	this->Module->SetNetAddressable(); // Make net addressable
	this->Module->SetIsReplicated(true);

}



void APowerSuit::BeginPlay()
{
	UE_LOG(PowerSuit_Log, Display, TEXT("**************** PowerSuit Spawn ****************\n %s"), *GetName());
	Super::BeginPlay();
	//FScriptDelegate NewDel = FScriptDelegate();
	//NewDel.BindUFunction(this, "OnConnectionStatusUpdatedReplacement");
	//HoverModeChangedDelegate.Add(NewDel);
}


void APowerSuit::OnConnectionStatusUpdatedReplacement(bool HasConnection)
{
	if (!HasConnection)
	{
		if (Module->nProducing)
		{
			if (Module->SuitState == EPowerSuitState::PS_HOVER || Module->SuitState == EPowerSuitState::PS_HOVERSPRINT || Module->SuitState == EPowerSuitState::PS_HOVERMOVE || Module->SuitState == EPowerSuitState::PS_SLOWFALL)
			{
				UE_LOG(PowerSuit_Log, Display, TEXT("**************** PowerSuit Re-Enabling Hover ****************\n %s"), *GetName());
				Module->MoveC->CustomMovementMode = uint8(ECustomMovementMode::CMM_Hover);
				Module->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Hover);
			}
		}
	}
}
void APowerSuit::Destroyed()
{
	UE_LOG(PowerSuit_Log, Display, TEXT("**************** PowerSuit Destroyed ****************\n %s"), *GetName());
	Super::Destroyed();
}

void  APowerSuit::Equip(class AFGCharacterPlayer* character){
	UE_LOG(PowerSuit_Log, Display, TEXT("**************** PowerSuit Equip ****************\n %s"), *GetName());
	Super::Equip(character);;
	if (character)
	{
		if (character->HasAuthority())
		{
			SetInstigator(character);
			SetOwner(character);
			if (Module)
			{
				Module->Init(this);
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

void APowerSuit::AddEquipmentActionBindings()
{
	Super::AddEquipmentActionBindings();
	if (!GetInstigator())
		return;
	AFGPlayerController * Pc = Cast<AFGPlayerController>(GetInstigator()->GetController());
	if (!Pc)
		return;
	UInputSettings* InputSettings = UInputSettings::GetInputSettings();

	if (!InputSettings->DoesActionExist("PowerSuit_Acceleration"))
	{
		FInputActionKeyMapping KB = FInputActionKeyMapping();
		KB.ActionName = "PowerSuit_Acceleration";
		KB.Key = EKeys::LeftShift;
		InputSettings->AddActionMapping(KB);
	}

	if (!InputSettings->DoesActionExist("PowerSuit_DeAcceleration"))
	{
		FInputActionKeyMapping KB = FInputActionKeyMapping();
		KB.ActionName = "PowerSuit_DeAcceleration";
		KB.Key = EKeys::LeftControl;
		InputSettings->AddActionMapping(KB);
	}

	if (!InputSettings->DoesActionExist("PowerSuit_UIToggle"))
	{
		FInputActionKeyMapping KB = FInputActionKeyMapping();
		KB.ActionName = "PowerSuit_UIToggle";
		KB.Key = EKeys::RightAlt;
		InputSettings->AddActionMapping(KB);
	}

	if (!InputSettings->DoesActionExist("PowerSuit_FrictionToggle"))
	{
		FInputActionKeyMapping KB = FInputActionKeyMapping();
		KB.ActionName = "PowerSuit_FrictionToggle";
		KB.Key = EKeys::LeftAlt;

		InputSettings->AddActionMapping(KB);
	}
	UFGInputLibrary::UpdateInputMappings(Pc);
	FInputActionKeyMapping JumpKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "Jump_Drift", false);
	FInputActionKeyMapping crouchKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "Crouch", false);

	FInputActionKeyMapping AccelKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "PowerSuit_Acceleration", false);
	FInputActionKeyMapping DeAccelKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "PowerSuit_DeAcceleration", false);
	FInputActionKeyMapping TogKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "PowerSuit_FrictionToggle", false);

	FInputActionBinding NewBinding("PowerSuit_FrictionToggle", EInputEvent::IE_Pressed);
	NewBinding.bConsumeInput = false;
	NewBinding.ActionDelegate.BindDelegate(this, "OnPowerSuitFrictionToggle");

	FInputActionBinding NewBinding2("PowerSuit_UIToggle", EInputEvent::IE_Pressed);
	NewBinding2.bConsumeInput = false;
	NewBinding2.ActionDelegate.BindDelegate(this, "OnPowerSuitUIToggle");

	Module->KB_Up = JumpKey.Key;
	Module->KB_Accel = AccelKey.Key;
	Module->KB_Down = crouchKey.Key;
	Module->KB_Breaks = DeAccelKey.Key;
	Module->KB_Toggle = TogKey.Key;
	
}

