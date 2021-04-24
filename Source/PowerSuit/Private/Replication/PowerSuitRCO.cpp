


#include "Replication/PowerSuitRCO.h"
#include "SubModules/EMC_StateModule.h"
#include "Stats/PowerSuitData.h"
#include "..\..\Public\Replication\PowerSuitRCO.h"

void UPowerSuitRCO::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPowerSuitRCO, Dummy);

}

void UPowerSuitRCO::ServerSetEMovementMode_Implementation(UEquipmentModuleComponent* Component, uint8 Enum)
{
	if (!Component)
		return;
	Component->nMovementMode = EMovementMode(Enum);
	UE_LOG(PowerSuit_Log, Error, TEXT("ServerSetEMovementMode_Implementation"));

}

void UPowerSuitRCO::ServerSetEMovementModeCustom_Implementation(UEquipmentModuleComponent* Component, uint8 Enum)
{
	if (!Component)
		return;
	Component->nCustomMovementMode = Enum;
	UE_LOG(PowerSuit_Log, Error, TEXT("ServerSetEMovementModeCustom_Implementation"));

}

void UPowerSuitRCO::ServerSetHotKeyDirectionalAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	UE_LOG(PowerSuit_Log, Error, TEXT("Server Accel Key ? %s"), *FString::FromInt(State));

	Component->StateModule->HKey_Accel = State;
}


void UPowerSuitRCO::ServerSetHotKeyDirectionalDeAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	UE_LOG(PowerSuit_Log, Error, TEXT("Server DeAccel Key ? %s"), *FString::FromInt(State));
	Component->StateModule->HKey_Breaks = State;
}


void UPowerSuitRCO::ServerSetHotKeyDownAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	UE_LOG(PowerSuit_Log, Error, TEXT("Server Down ? %s"), *FString::FromInt(State));
	Component->StateModule->HKey_Down = State;
}


void UPowerSuitRCO::ServerSetHotKeyUpAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	UE_LOG(PowerSuit_Log, Error, TEXT("Server UP Key ? %s"), *FString::FromInt(State));
	Component->StateModule->HKey_Up = State;
}


void UPowerSuitRCO::ServerSetFlying_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	UE_LOG(PowerSuit_Log, Error, TEXT("Server SetFlying Key ? %s"), *FString::FromInt(State));
	Component->TKey_Fly = State;
}


void UPowerSuitRCO::ServerSetIsSliding_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	UE_LOG(PowerSuit_Log, Error, TEXT("Server Sliding Key ? %s"), *FString::FromInt(State));
	Component->StateModule->nIsSliding = State;
}


void UPowerSuitRCO::ServerSetIsSprinting_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	UE_LOG(PowerSuit_Log, Error, TEXT("Server Sprinting Key ? %s"), *FString::FromInt(State));
	Component->StateModule->nIsSprinting = State;
}


void UPowerSuitRCO::ServerSetHovering_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	UE_LOG(PowerSuit_Log, Error, TEXT("Server No Friction Key ? %s"), *FString::FromInt(State));
	Component->TKey_NoFriction = State;
}



void UPowerSuitRCO::ServerSetAttachmentFloatValue_Implementation(APowerSuitModuleAttachment* Component, float State, uint8 Index)
{
	if (!Component)
		return;
	Component->ServerSetFloatValue(State, Index);
	UE_LOG(PowerSuit_Log, Error, TEXT("ServerSetAttachmentFloatValue_Implementation"));

};



void UPowerSuitRCO::ServerSetAttachmentBoolValue_Implementation(APowerSuitModuleAttachment* Component, bool State, uint8 Index)
{
	if (!Component)
		return;
	Component->ServerSetBoolValue(State, Index);
	UE_LOG(PowerSuit_Log, Error, TEXT("ServerSetAttachmentBoolValue_Implementation"));

};



void UPowerSuitRCO::ServerUpdateOnIndex_Implementation(UEquipmentModuleComponent* Component, int32 Index)
{
	if (!Component)
		return;
	Component->InventoryModule->UpdateOnIndex(Index);
	UE_LOG(PowerSuit_Log, Error, TEXT("ServerUpdateOnIndex_Implementation"));

}




void UPowerSuitRCO::ServerSetSuitMovementProperty_Implementation(APowerSuitModuleAttachment* Attachment, ESuitMovementProperty Enum, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetSuitMovementProperty(Enum, Property);
	UE_LOG(PowerSuit_Log, Error, TEXT("ServerSetSuitMovementProperty_Implementation"));

}

void UPowerSuitRCO::ServerSetSuitFlightProperty_Implementation(APowerSuitModuleAttachment* Attachment, ESuitFlightProperty Enum, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetSuitFlightProperty(Enum, Property);
	UE_LOG(PowerSuit_Log, Error, TEXT("ServerSetSuitFlightProperty_Implementation"));


}

void UPowerSuitRCO::ServerSetSuitProperty_Implementation(APowerSuitModuleAttachment* Attachment, ESuitProperty Enum, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetSuitProperty(Enum, Property);
	UE_LOG(PowerSuit_Log, Error, TEXT("ServerSetSuitProperty_Implementation"));

}

void UPowerSuitRCO::ServerSetPropertyGeneral_Implementation(APowerSuitModuleAttachment* Attachment, EEquipmentStatsProperty PropertyType, uint8 Index, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetPropertyGeneral(PropertyType,Index,  Property);
	UE_LOG(PowerSuit_Log, Error, TEXT("ServerSetPropertyGeneral_Implementation"));

}

void UPowerSuitRCO::ServerSetSuitFlag_Implementation(APowerSuitModuleAttachment* Attachment, ESuitFlag Flag, bool Enabled)
{
	if (!Attachment)
		return;
	Attachment->SetSuitFlag(Flag, Enabled);
	UE_LOG(PowerSuit_Log, Error, TEXT("Server SuitFlag changed ? %i TO %b"), int32(Flag), Enabled);

}


void UPowerSuitRCO::ServerUpdateCurrentHoverMode_Implementation(UEquipmentModuleComponent* Component, EHoverPackMode Index)
{
	if (!Component)
		return;
	if (!Component->EquipmentParent)
		return;
#ifdef MODDING_SHIPPING
	int32 Value = int32(Component->EquipmentParent->mCurrentHoverMode);

#else
	int32 Value = int32(Component->EquipmentParent->*get(steal_mCurrentHoverMode()));

#endif
	int32 ValueNew = int32(Index);


	UE_LOG(PowerSuit_Log, Error, TEXT("Server Hover modo changu ? %i TO %i"),Value,ValueNew );

	Component->EquipmentParent->SetHoverMode(Index, false);
}

