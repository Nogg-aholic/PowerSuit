#include "Equipment/Equipment_PowerSuit.h"

#include "Net/UnrealNetwork.h"

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

#include "EnhancedInputSubsystems.h"

#include "FactoryGame.h"

APowerSuit::APowerSuit()
{
	this->Module = CreateDefaultSubobject<UEquipmentModuleComponent>(TEXT("EquipmentModule")); 
	this->Module->SetupAttachment(this->RootComponent);
	this->Module->SetNetAddressable(); // Make net addressable
	this->Module->SetIsReplicated(true);
	this->SuitMappingPriority = -101; // Lower (higher prio) than PlayerMovement
}



void APowerSuit::BeginPlay()
{
	UE_LOG(LogPowerSuitCpp, Display, TEXT("**************** PowerSuit Spawn ****************\n %s"), *GetName());
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
				UE_LOG(LogPowerSuitCpp, Warning, TEXT("**************** PowerSuit Re-Enabling Hover ****************\n %s"), *GetName());
				Module->MoveC->CustomMovementMode = uint8(ECustomMovementMode::CMM_Hover);
				Module->EquipmentParent->SetHoverMode(EHoverPackMode::HPM_Hover);
			}
		}
	}
}
void APowerSuit::Destroyed()
{
	UE_LOG(LogPowerSuitCpp, Warning, TEXT("**************** PowerSuit Destroyed ****************\n %s"), *GetName());
	Super::Destroyed();
}

void APowerSuit::Equip(class AFGCharacterPlayer* character){
	UE_LOG(LogPowerSuitCpp, Warning, TEXT("**************** PowerSuit Equip ****************\n %s"), *GetName());
	Super::Equip(character);
	if (character && character->HasAuthority())
	{	
		SetInstigator(character);
		SetOwner(character->GetController());
		if (Module)
		{
			UE_LOG(LogPowerSuitCpp, Warning, TEXT("**************** PS EMC Init ****************\n %s"), *GetName());
			Module->Init(this);
		}
	}
};

void APowerSuit::WasSlottedIn(class AFGCharacterPlayer* holder) {
	UE_LOG(LogPowerSuitCpp, Warning, TEXT("**************** PowerSuit SlottedIn ****************\n %s"), *GetName());
	// Since it's not a hand equipment, this should never be called. Keeping this here in case it does
	Super::WasSlottedIn(holder);
}

void APowerSuit::UnEquip()
{	
	UE_LOG(LogPowerSuitCpp, Warning, TEXT("**************** PowerSuit UnEquip ****************\n %s"), *GetName());
	if (!IsValid(this)) {
		UE_LOG(LogPowerSuitCpp, Display, TEXT("is Pending kill "));
	}

	SuitClearEquipmentActionBindings();

	if (!Module || !Module->nInventory) {
		UE_LOG(LogPowerSuitCpp, Error, TEXT("Can't UnEquip a suit that doesn't have a Module or ModuleInventory?"));
		return;
	}

	if (GetInstigator() && Cast<AFGCharacterPlayer>(GetInstigator()))
	{
		if (Cast<AFGCharacterPlayer>(GetInstigator())->IsDrivingVehicle()) {
			UE_LOG(LogPowerSuitCpp, Warning, TEXT("is Driving a Vehicle, so OverrideReboot"));
			OverrideReboot = true;
		} else {
			OverrideReboot = false;
		}
	}
		
	if (Module->nInventory)
	{
		if (!IsValid(this)) {
			UE_LOG(LogPowerSuitCpp, Display, TEXT("(No Inventory because is Pending kill)"))
		} else {
			UE_LOG(LogPowerSuitCpp, Display, TEXT("Valid Inventory %s "), *Module->nInventory->GetName());
		}
	}
	else
	{
		UE_LOG(LogPowerSuitCpp, Display, TEXT("Inventory Invalid !!"));
	}
	if (Module && IsValid(this) && Module->MoveC)
	{
		UE_LOG(LogPowerSuitCpp, Display, TEXT("Unequip Called reset stats"));
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
		UE_LOG(LogPowerSuitCpp, Display, TEXT("Disabled Flight"));
	}
	Super::UnEquip();
};

void APowerSuit::WasRemovedFromSlot() {
	UE_LOG(LogPowerSuitCpp, Warning, TEXT("**************** PowerSuit WasRemovedFromSlot ****************\n %s"), *GetName());
	// Since it's not a hand equipment, this should never be called...? unsure. Keeping this here in case it does
	Super::WasRemovedFromSlot();
}

void APowerSuit::Tick(float DeltaSeconds)
{
	
	if (!GetInstigator())
		return;

	if (!IsEquipped() || !Module)
		return;

	Module->EquippedTick(DeltaSeconds);
	
	// Manage HoverPack's power connection to prevent flight when not allowed
	if (Module->TKey_Fly && Module->Stats.HasFlag(ESuitFlag::SuitFlag_HasFlightUnlocked) && Module->nProducing)
	{
		mCurrentConnectionLocation = GetInstigator()->GetActorLocation();
		mCurrentPowerLevel = 1.f;
		if (InnerBattery && !mHasConnection) {
			if (!InnerBattery->HasPower() && InnerBattery->GetPowerInfo() && HasAuthority()) {
				if (!InnerBattery->GetPowerInfo()->GetBatteryInfo())
					InnerBattery->GetPowerInfo()->InitializeBatteryInfo(1000.f, 1000.f);
				InnerBattery->SetHasPower(true);
				InnerBattery->GetPowerInfo()->SetHasPower(true);
				InnerBattery->GetPowerInfo()->GetBatteryInfo()->SetActive(true);
				InnerBattery->GetPowerInfo()->GetBatteryInfo()->SetPowerStore(1000.f);
				UE_LOG(LogPowerSuitCpp, Display, TEXT("SetBattery Storage"));

			}
			//UE_LOG(LogPowerSuitCpp, Display, TEXT("NO CON"));
			ConnectToPowerConnection(mCurrentPowerConnection);
			mCurrentPowerConnection = InnerBattery;
			mHasConnection = true;
			SetCharacterHoverMovementMode();
		}
	} else {
		if (mHasConnection && mCurrentPowerConnection == InnerBattery) {
			//UE_LOG(LogPowerSuitCpp, Display, TEXT("CON Removed"));
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
	DOREPLIFETIME(APowerSuit, UserPreferredFuelType);
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

void APowerSuit::SetUserPreferredFuelType(TSubclassOf<UFGItemDescriptor> item) {
	if (this->Module->FuelModule->nAllowedFuels.Contains(item)) {
		this->UserPreferredFuelType = item;
		OnUserSelectedFuelTypeChange.Broadcast(item);
	} else {
		UE_LOG(LogPowerSuitCpp, Warning, TEXT("Tried to set preferred fuel to a fuel that wasn't in the Allowed Fuels, ignored"));
	}
}

void APowerSuit::AddEquipmentActionBindings()
{
	Super::AddEquipmentActionBindings();
	if (!GetInstigator())
		return;
	const auto Pc = Cast<AFGPlayerController>(GetInstigator()->GetController());
	if (!Pc)
		return;

	// We need to register it manually because the automatic one doesn't give it a low enough priority to beat PlayerMovement's -2
	if (const auto LocalPlayer = Pc->GetLocalPlayer())
	{
		if (const auto InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (!SuitInputMapping.IsNull())
			{
				const auto context = SuitInputMapping.LoadSynchronous();
				if (InputSystem->HasMappingContext(context)) {
					UE_LOG(LogPowerSuit_Input, Error, TEXT("SuitInputMapping was already loaded by something else, was it not cleaned up on unequip? did you accidentally list it in the"), *UKismetSystemLibrary::GetDisplayName(context));
				}
				else {
					UE_LOG(LogPowerSuit_Input, Display, TEXT("SuitInputMapping %s loaded by PS Cpp at Priority %d"), *UKismetSystemLibrary::GetDisplayName(context), SuitMappingPriority);
					InputSystem->AddMappingContext(context, SuitMappingPriority);
				}
			}
			else {
				UE_LOG(LogPowerSuit_Input, Error, TEXT("SuitInputMapping was null, further keybind attempts will probably fail"));
			}
		}
	}
	
	// TODOU8 transition to listen to the modifier keys
	// pass them in via Gameplay Tags? Arch says no... read from field on cpp-defined Instance Module?
	// https://discord.com/channels/555424930502541343/862002356626128907/1130872287197937825
	// https://docs.unrealengine.com/4.26/en-US/ProgrammingAndScripting/Tags/

	TArray<FKey> discard;
	UFGInputLibrary::GetCurrentMappingForAction(Pc, "PlayerMovement_Jump", Module->KB_Up, discard);
	UFGInputLibrary::GetCurrentMappingForAction(Pc, "PlayerMovement_Crouch", Module->KB_Down, discard);

	UFGInputLibrary::GetCurrentMappingForAction(Pc, "PowerSuit.Suit.Acceleration", Module->KB_Accel, discard);
	UFGInputLibrary::GetCurrentMappingForAction(Pc, "PowerSuit.Suit.DeAcceleration", Module->KB_DeAccel, discard);
	UFGInputLibrary::GetCurrentMappingForAction(Pc, "PowerSuit.Suit.GravityToggle", Module->KB_ToggleFlightGravity, discard);
	UFGInputLibrary::GetCurrentMappingForAction(Pc, "PowerSuit.Suit.FrictionToggle", Module->KB_ToggleFlightFriction, discard);
	UFGInputLibrary::GetCurrentMappingForAction(Pc, "PowerSuit.Suit.UIToggle", Module->KB_UI, discard);

	UE_LOG(LogPowerSuit_Input, Verbose, TEXT("Debug: GravityKey is bound to %s"), *Module->KB_ToggleFlightGravity.ToString());
}

void APowerSuit::SuitClearEquipmentActionBindings() {

	if (!GetInstigator()) {
		UE_LOG(LogPowerSuit_Input, Error, TEXT("GetInstigator was null when trying to ClearEquipmentActionBindings"));
		return;
	}
	const auto playerController = Cast<AFGPlayerController>(GetInstigator()->GetController());
	if (!playerController) {
		UE_LOG(LogPowerSuit_Input, Error, TEXT("PlayerController was null when trying to ClearEquipmentActionBindings"));
		return;
	}

	// Clean up our manually registered mapping context
	if (const auto LocalPlayer = playerController->GetLocalPlayer())
	{
		if (const auto InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (!SuitInputMapping.IsNull())
			{
				const auto context = SuitInputMapping.LoadSynchronous();
				if (InputSystem->HasMappingContext(context)) {
					UE_LOG(LogPowerSuit_Input, Display, TEXT("Removed SuitInputMappingContext"));
					InputSystem->RemoveMappingContext(context);
				}
				else {
					UE_LOG(LogPowerSuit_Input, Error, TEXT("SuitInputMapping was not applied when ClearEquipmentActionBindings tried to remove it"));
				}
			}
		}
	}
}

void APowerSuit::OnCharacterMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode, EMovementMode NewMovementMode, uint8 NewCustomMode)
{
	if (!Module || !Module->MoveC)
		return;
	const auto humanControlled = UFGBlueprintFunctionLibrary::IsLocallyHumanControlled(GetInstigator());
	OnMovementModeChanged.Broadcast(PreviousMovementMode, PreviousCustomMode, NewMovementMode, NewCustomMode);

	if (NewCustomMode == ECustomMovementMode::CMM_None || (PreviousMovementMode == MOVE_Swimming && (NewMovementMode == MOVE_Walking || NewMovementMode == MOVE_Falling) ))
	{
		Module->MoveC->mMaxSprintSpeed = Cast< UFGCharacterMovementComponent>(Module->MoveC->GetClass()->GetDefaultObject())->mMaxSprintSpeed;//900.f;
		Module->MoveC->mMaxSprintSpeed += Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mMaxSprintSpeed).value(); //ESMC_mMaxSprintSpeed
		Module->MoveC->mMaxSprintSpeed *= Module->GetMovementPropertySafe(ESuitMovementProperty::ESMC_mMaxSprintSpeed).ClampMult(); //ESMC_mMaxSprintSpeed
		if(HasAuthority() && !humanControlled)
		{
			Module->RemoteUpdateMovementGround(Module->MoveC->mMaxSprintSpeed);
		}
	}
	else if(NewCustomMode == ECustomMovementMode::CMM_Hover)
	{
		
		Module->MoveC->mMaxSprintSpeed = Cast<APowerSuit>(GetClass()->GetDefaultObject())->mHoverSpeed;//900.f;
		Module->MoveC->mMaxSprintSpeed += Module->GetFlightPropertySafe(ESuitFlightProperty::EFP_mHoverSpeed).value(); //ESMC_mMaxSprintSpeed
		Module->MoveC->mMaxSprintSpeed *= Module->GetFlightPropertySafe(ESuitFlightProperty::EFP_mHoverSpeed).ClampMult(); //ESMC_mMaxSprintSpeed
		Module->MoveC->mMaxSprintSpeed += Module->HoverMovementSpeedAdded;
		mHoverSpeed = Module->MoveC->mMaxSprintSpeed;


		mHoverAccelerationSpeed = Cast<APowerSuit>(GetClass()->GetDefaultObject())->mHoverAccelerationSpeed;//900.f;
		mHoverAccelerationSpeed += Module->GetFlightPropertySafe(ESuitFlightProperty::EFP_mHoverAccelerationSpeed).value(); //ESMC_mMaxSprintSpeed
		mHoverAccelerationSpeed *= Module->GetFlightPropertySafe(ESuitFlightProperty::EFP_mHoverAccelerationSpeed).ClampMult(); //ESMC_mMaxSprintSpeed
		mHoverAccelerationSpeed += Module->HoverMovementSpeedAccelAdded;

		if(HasAuthority() && !humanControlled)
		{
			Module->RemoteUpdateMovementAir(Module->MoveC->mMaxSprintSpeed,mHoverAccelerationSpeed);
		}
	}
}


