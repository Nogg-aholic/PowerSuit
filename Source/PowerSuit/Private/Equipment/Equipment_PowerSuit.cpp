
#include "Equipment/Equipment_PowerSuit.h"

#include "FGGameState.h"
#include "Buildables/FGBuildableConveyorBelt.h"
#include "Buildables/FGBuildableRailroadTrack.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
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
		if (Module && Module->MoveC && Module->EquipmentParent  && Module->nProducing)
		{
			if (Module->SuitState == EPowerSuitState::PS_HOVER || Module->SuitState == EPowerSuitState::PS_HOVERSPRINT || Module->SuitState == EPowerSuitState::PS_HOVERMOVE || Module->SuitState == EPowerSuitState::PS_SLOWFALL)
			{
				UE_LOG(PowerSuit_Log, Warning, TEXT("**************** PowerSuit Re-Enabling Hover ****************\n %s"), *GetName());
				Module->MoveC->CustomMovementMode = uint8(ECustomMovementMode::CMM_Hover);
				Module->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Hover);
			}
		}
	}
}
void APowerSuit::Destroyed()
{
	UE_LOG(PowerSuit_Log, Warning, TEXT("**************** PowerSuit Destroyed ****************\n %s"), *GetName());
	Super::Destroyed();
}

void APowerSuit::Equip(class AFGCharacterPlayer* character){
	UE_LOG(PowerSuit_Log, Warning, TEXT("**************** PowerSuit Equip ****************\n %s"), *GetName());
	Super::Equip(character);
	if (character && character->HasAuthority())
	{	
		SetInstigator(character);
		SetOwner(character->GetController());
		if (Module)
		{
			UE_LOG(PowerSuit_Log, Warning, TEXT("**************** PS EMC Init ****************\n %s"), *GetName());
			Module->Init(this);
		}
	}
};

void APowerSuit::WasSlottedIn(class AFGCharacterPlayer* holder) {
	UE_LOG(PowerSuit_Log, Warning, TEXT("**************** PowerSuit SlottedIn ****************\n %s"), *GetName());
	// Since it's not a hand equipment, this should never be called. Keeping this here in case it does
	Super::WasSlottedIn(holder);
}

void APowerSuit::UnEquip()
{	
	UE_LOG(PowerSuit_Log, Warning, TEXT("**************** PowerSuit UnEquip ****************\n %s"), *GetName());
	if (IsPendingKill()) {
		UE_LOG(PowerSuit_Log, Display, TEXT("is Pending kill "));
	}

	if (!Module || !Module->nInventory) {
		UE_LOG(PowerSuit_Log, Error, TEXT("Can't UnEquip a suit that doesn't have a Module or ModuleInventory?"));
		return;
	}

	if (GetInstigator() && Cast<AFGCharacterPlayer>(GetInstigator()))
	{
		if (Cast<AFGCharacterPlayer>(GetInstigator())->IsDrivingVehicle()) {
			UE_LOG(PowerSuit_Log, Warning, TEXT("is Driving a Vehicle, so OverrideReboot"));
			OverrideReboot = true;
		} else {
			OverrideReboot = false;
		}
	}
		
	if (Module->nInventory)
	{
		if (IsPendingKill())
			UE_LOG(PowerSuit_Log, Display, TEXT("(No Inventory becase is Pending kill)"))
		else
		{
			UE_LOG(PowerSuit_Log, Display, TEXT("Valid Inventory %s "), *Module->nInventory->GetName());
		}
	}
	else
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("Inventory Invalid !!"));
	}
	if (Module && !IsPendingKill() && Module->MoveC)
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("Unequip Called reset stats"));
		// Don't notify because could be unequipping on disconnect
		Module->ResetStats(false);
	}
	if (Module->MoveC &&
		Module->MoveC->MovementMode == MOVE_Custom &&
		(Module->MoveC->CustomMovementMode == ECustomMovementMode::CMM_Hover || Module->MoveC->CustomMovementMode == ECustomMovementMode::CMM_HoverSlowFall))
	{
		SetHoverMode(EHoverPackMode::HPM_Inactive);
		Module->MoveC->CustomMovementMode = static_cast<uint8>(ECustomMovementMode::CMM_None);
		Module->MoveC->MovementMode = EMovementMode::MOVE_Falling;
		UE_LOG(PowerSuit_Log, Display, TEXT("Disabled Flight"));
	}
	Super::UnEquip();
};

void APowerSuit::WasRemovedFromSlot() {
	UE_LOG(PowerSuit_Log, Warning, TEXT("**************** PowerSuit WasRemovedFromSlot ****************\n %s"), *GetName());
	// Since it's not a hand equipment, this should never be called. Keeping this here in case it does
	Super::WasRemovedFromSlot();
}

void APowerSuit::Tick(float DeltaSeconds)
{
	
	if (!GetInstigator())
		return;

	if (!IsEquipped() || !Module)
		return;

	Module->EquippedTick(DeltaSeconds);
	


	if (Module->TKey_Fly && Module->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked) && Module->nProducing)
	{
		mCurrentConnectionLocation = GetInstigator()->GetActorLocation();
		mCurrentPowerLevel = 1.f;
		if (InnerBattery && !mHasConnection)
		{
			if (!InnerBattery->HasPower() && InnerBattery->GetPowerInfo() && HasAuthority())
			{
				if (!InnerBattery->GetPowerInfo()->GetBatteryInfo())
					InnerBattery->GetPowerInfo()->InitializeBatteryInfo(1000.f, 1000.f);
				InnerBattery->SetHasPower(true);
				InnerBattery->GetPowerInfo()->SetHasPower(true);
				InnerBattery->GetPowerInfo()->GetBatteryInfo()->SetActive(true);
				InnerBattery->GetPowerInfo()->GetBatteryInfo()->SetPowerStore(1000.f);
				UE_LOG(PowerSuit_Log, Display, TEXT("SetBattery Storage"));

			}
			//UE_LOG(PowerSuit_Log, Display, TEXT("NO CON"));
			ConnectToPowerConnection(mCurrentPowerConnection);
			mCurrentPowerConnection = InnerBattery;
			mHasConnection = true;
			SetCharacterHoverMovementMode();
		}
	}
	else
	{
		if (mHasConnection && mCurrentPowerConnection == InnerBattery)
		{
			//UE_LOG(PowerSuit_Log, Display, TEXT("CON Removed"));
			DisconnectFromCurrentPowerConnection();
			mHasConnection = false;
		}
	}
	Super::Tick(DeltaSeconds);
}

void APowerSuit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APowerSuit,OverrideReboot);
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

void APowerSuit::OnCharacterMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode, EMovementMode NewMovementMode, uint8 NewCustomMode)
{
	if (!Module || !Module->MoveC)
		return;
	OnMovementModeChanged.Broadcast(PreviousMovementMode, PreviousCustomMode, NewMovementMode, NewCustomMode);

	if (NewCustomMode == 0 || (PreviousMovementMode == MOVE_Swimming && (NewMovementMode == MOVE_Walking || NewMovementMode == MOVE_Falling) ))
	{
		Module->MoveC->mMaxSprintSpeed = Cast< UFGCharacterMovementComponent>(Module->MoveC->GetClass()->GetDefaultObject())->mMaxSprintSpeed;//900.f;
		Module->MoveC->mMaxSprintSpeed += Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mMaxSprintSpeed).value(); //ESMC_mMaxSprintSpeed
		Module->MoveC->mMaxSprintSpeed *= Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mMaxSprintSpeed).ClampMult(); //ESMC_mMaxSprintSpeed
		if(HasAuthority() && !UFGBlueprintFunctionLibrary::IsLocallyHumanControlled(GetInstigator()))
		{
			Module->RemoteUpdateMovementGround(Module->MoveC->mMaxSprintSpeed);
		}
	}
	else if(NewCustomMode == 4)
	{
		
		Module->MoveC->mMaxSprintSpeed = Cast<APowerSuit>(GetClass()->GetDefaultObject())->mHoverSpeed;//900.f;
		//Module->MoveC->CheatFlySpeedVertical = Module->HoverMovementSpeedAdded;
		Module->MoveC->mMaxSprintSpeed += Module->GetFlightPropertySafe(ESuitFlightProperty::EFP_mHoverSpeed).value(); //ESMC_mMaxSprintSpeed
		Module->MoveC->mMaxSprintSpeed *= Module->GetFlightPropertySafe(ESuitFlightProperty::EFP_mHoverSpeed).ClampMult(); //ESMC_mMaxSprintSpeed
		Module->MoveC->mMaxSprintSpeed += Module->HoverMovementSpeedAdded;
		mHoverSpeed = Module->MoveC->mMaxSprintSpeed;


		mHoverAccelerationSpeed = Cast<APowerSuit>(GetClass()->GetDefaultObject())->mHoverAccelerationSpeed;//900.f;
		mHoverAccelerationSpeed += Module->GetFlightPropertySafe(ESuitFlightProperty::EFP_mHoverAccelerationSpeed).value(); //ESMC_mMaxSprintSpeed
		mHoverAccelerationSpeed *= Module->GetFlightPropertySafe(ESuitFlightProperty::EFP_mHoverAccelerationSpeed).ClampMult(); //ESMC_mMaxSprintSpeed
		mHoverAccelerationSpeed += Module->HoverMovementSpeedAccelAdded;

		if(HasAuthority() && !UFGBlueprintFunctionLibrary::IsLocallyHumanControlled(GetInstigator()))
		{
			Module->RemoteUpdateMovementAir(Module->MoveC->mMaxSprintSpeed,mHoverAccelerationSpeed);
		}
	}
}
