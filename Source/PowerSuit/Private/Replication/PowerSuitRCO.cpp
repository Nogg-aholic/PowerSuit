


#include "Replication/PowerSuitRCO.h"
#include "SubModules/EMC_StateModule.h"
#include "Stats/PowerSuitData.h"
#include "Attachment/PowerSuitModuleAttachment.h"
#include "SubModules/EMC_InventoryModule.h"

void UPowerSuitRCO::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPowerSuitRCO, Dummy);

}

void UPowerSuitRCO::ServerSetFrictionMode_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->TKey_NoFriction = State;
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO Server Friction %b"),Component->TKey_NoFriction);

}

void UPowerSuitRCO::ServerSetEMovementMode_Implementation(UEquipmentModuleComponent* Component, uint8 Enum)
{
	if (!Component)
		return;
	Component->nMovementMode = static_cast<EMovementMode>(Enum);
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO Server nMovementMode %i"),Component->nMovementMode);

}

void UPowerSuitRCO::ServerSetEMovementModeCustom_Implementation(UEquipmentModuleComponent* Component, uint8 Enum)
{
	if (!Component)
		return;
	Component->nCustomMovementMode = Enum;
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO Server nCustomMovementMode %i"),Component->nCustomMovementMode);

}

void UPowerSuitRCO::ServerSetHotKeyDirectionalAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->HKey_Accel = State;
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO Server HKey_Accel %i"),Component->StateModule->HKey_Accel);

}


void UPowerSuitRCO::ServerSetHotKeyDirectionalDeAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->HKey_Breaks = State;
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO Server HKey_Breaks %i"),Component->StateModule->HKey_Breaks);

}


void UPowerSuitRCO::ServerSetHotKeyDownAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->HKey_Down = State;
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO Server HKey_Down %i"),Component->StateModule->HKey_Down);

}


void UPowerSuitRCO::ServerSetHotKeyUpAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->HKey_Up = State;
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO Server HKey_Up %i"),Component->StateModule->HKey_Up);

}


void UPowerSuitRCO::ServerSetFlying_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->TKey_Fly = State;
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO Server TKey_Fly %i"),Component->TKey_Fly);

}


void UPowerSuitRCO::ServerSetIsSliding_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->nIsSliding = State;
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO Server nIsSliding %i"),Component->StateModule->nIsSliding);

}


void UPowerSuitRCO::ServerSetIsSprinting_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->nIsSprinting = State;
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO Server nIsSprinting %i"),Component->StateModule->nIsSprinting);

}


void UPowerSuitRCO::ServerSetGravityMode_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->TKey_NoGravity = State;
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO Server TKey_NoGravity %i"),Component->TKey_NoGravity);

}



void UPowerSuitRCO::ServerSetAttachmentFloatValue_Implementation(APowerSuitModuleAttachment* Component, float State, uint8 Index)
{
	if (!Component)
		return;
	Component->ServerSetFloatValue(State, Index);
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO  ServerSetFloatValue %f On Index : %i"),State,Index);

};



void UPowerSuitRCO::ServerSetAttachmentBoolValue_Implementation(APowerSuitModuleAttachment* Component, bool State, uint8 Index)
{
	if (!Component)
		return;
	Component->ServerSetBoolValue(State, Index);
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO  ServerSetBoolValue %b On Index : %i"),State,Index);

};



void UPowerSuitRCO::ServerUpdateOnIndex_Implementation(UEquipmentModuleComponent* Component, int32 Index)
{
	if (!Component)
		return;
	Component->InventoryModule->UpdateOnIndex(Index);
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO  UpdateOnIndex Index : %i"),Index);


}




void UPowerSuitRCO::ServerSetSuitMovementProperty_Implementation(APowerSuitModuleAttachment* Attachment, ESuitMovementProperty Enum, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetSuitMovementProperty(Enum, Property);
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO  SetSuitMovementProperty %i On Value: %f"),Enum,Property.value());

}

void UPowerSuitRCO::ServerSetSuitFlightProperty_Implementation(APowerSuitModuleAttachment* Attachment, ESuitFlightProperty Enum, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetSuitFlightProperty(Enum, Property);

	UE_LOG(PowerSuit_Log, Display, TEXT("RCO  SetSuitFlightProperty %i On Value: %f"),Enum,Property.value());

}

void UPowerSuitRCO::ServerSetSuitProperty_Implementation(APowerSuitModuleAttachment* Attachment, ESuitProperty Enum, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetSuitProperty(Enum, Property);
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO  SetSuitProperty %i On Value: %f"),Enum,Property.value());

}

void UPowerSuitRCO::ServerSetPropertyGeneral_Implementation(APowerSuitModuleAttachment* Attachment, EEquipmentStatsProperty PropertyType, uint8 Index, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetPropertyGeneral(PropertyType,Index,  Property);
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO  SetPropertyGeneral %i On Index : %i Value: %f"),PropertyType,Index,Property.value());

}

void UPowerSuitRCO::ServerSetSuitFlag_Implementation(APowerSuitModuleAttachment* Attachment, ESuitFlag Flag, bool Enabled)
{
	if (!Attachment)
		return;
	Attachment->SetSuitFlag(Flag, Enabled);
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO  SetSuitFlag %i  State : %b "),Flag,Enabled);

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
	int32 Value = static_cast<int32>(Component->EquipmentParent->*get(steal_mCurrentHoverMode()));

#endif
	int32 ValueNew = static_cast<int32>(Index);

	Component->EquipmentParent->SetHoverMode(Index, false);
	UE_LOG(PowerSuit_Log, Display, TEXT("RCO  SetHoverMode : %i "),Index);

}

