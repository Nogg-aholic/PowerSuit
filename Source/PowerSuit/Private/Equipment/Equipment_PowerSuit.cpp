
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
	if (Module && !IsPendingKill())
	{
		Module->ResetStats();
	}
	if (Module->MoveC->MovementMode == MOVE_Custom && (Module->MoveC->CustomMovementMode == ECustomMovementMode::CMM_Hover || Module->MoveC->CustomMovementMode == ECustomMovementMode::CMM_HoverSlowFall))
	{
		SetHoverMode(EHoverPackMode::HPM_Inactive);
		Module->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_None);
		Module->MoveC->MovementMode = EMovementMode::MOVE_Falling;
		UE_LOG(PowerSuit_Log, Display, TEXT("Disabled Flight"));
	}
	Super::UnEquip();
	SetInstigator(nullptr);
	SetOwner(nullptr);
};



void APowerSuit::Tick(float DeltaSeconds)
{
	
	if (!GetInstigator())
		return;

	if (!IsEquipped() || !Module)
		return;

	Module->EquippedTick(DeltaSeconds);
	
	Super::Tick(DeltaSeconds);
}

void APowerSuit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool APowerSuit::ShouldSaveState() const
{
	return true;
}


float APowerSuit::GetCurrentPowerNormalized() const
{
	if (!Module)
		return 0.f;

	return Module->nCurrentPower / FMath::Clamp(Module->nPowerCapacity,0.000001f,99999999.f);
}

float APowerSuit::GetSuitPowerCapacity() const
{
	if (!Module)
		return 0.f;

	return Module->nPowerCapacity;
}

float APowerSuit::GetCurrentPower() const
{
	if (!Module)
		return 0.f;

	return Module->nCurrentPower;
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

	UFGInputLibrary::UpdateInputMappings(Pc);
	FInputActionKeyMapping JumpKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "Jump_Drift", false);
	FInputActionKeyMapping crouchKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "Crouch", false);

	FInputActionKeyMapping AccelKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "PowerSuit.Acceleration", false);
	FInputActionKeyMapping DeAccelKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "PowerSuit.DeAcceleration", false);
	FInputActionKeyMapping GravityKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "PowerSuit.GravityToggle", false);
	FInputActionKeyMapping TogKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "PowerSuit.FrictionToggle", false);
	FInputActionKeyMapping UIgKey = UFGInputLibrary::GetKeyMappingForAction(Pc, "PowerSuit.UIToggle", false);

	Module->KB_Up = JumpKey.Key;
	Module->KB_Accel = AccelKey.Key;
	Module->KB_Down = crouchKey.Key;
	Module->KB_Breaks = DeAccelKey.Key;
	Module->KB_Toggle = GravityKey.Key;
	Module->KB_Toggle2 = TogKey.Key;
	Module->KB_UI = UIgKey.Key;
}

