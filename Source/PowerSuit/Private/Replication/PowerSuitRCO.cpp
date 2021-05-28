


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
}

void UPowerSuitRCO::ServerSetEMovementMode_Implementation(UEquipmentModuleComponent* Component, uint8 Enum)
{
	if (!Component)
		return;
	Component->nMovementMode = static_cast<EMovementMode>(Enum);

}

void UPowerSuitRCO::ServerSetEMovementModeCustom_Implementation(UEquipmentModuleComponent* Component, uint8 Enum)
{
	if (!Component)
		return;
	Component->nCustomMovementMode = Enum;

}

void UPowerSuitRCO::ServerSetHotKeyDirectionalAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->HKey_Accel = State;
}


void UPowerSuitRCO::ServerSetHotKeyDirectionalDeAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->HKey_Breaks = State;
}


void UPowerSuitRCO::ServerSetHotKeyDownAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->HKey_Down = State;
}


void UPowerSuitRCO::ServerSetHotKeyUpAccel_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->HKey_Up = State;
}


void UPowerSuitRCO::ServerSetFlying_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->TKey_Fly = State;
}


void UPowerSuitRCO::ServerSetIsSliding_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->nIsSliding = State;
}


void UPowerSuitRCO::ServerSetIsSprinting_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->StateModule->nIsSprinting = State;
}


void UPowerSuitRCO::ServerSetGravityMode_Implementation(UEquipmentModuleComponent* Component, bool State)
{
	if (!Component)
		return;
	Component->TKey_NoGravity = State;
}



void UPowerSuitRCO::ServerSetAttachmentFloatValue_Implementation(APowerSuitModuleAttachment* Component, float State, uint8 Index)
{
	if (!Component)
		return;
	Component->ServerSetFloatValue(State, Index);
};



void UPowerSuitRCO::ServerSetAttachmentBoolValue_Implementation(APowerSuitModuleAttachment* Component, bool State, uint8 Index)
{
	if (!Component)
		return;
	Component->ServerSetBoolValue(State, Index);

};



void UPowerSuitRCO::ServerUpdateOnIndex_Implementation(UEquipmentModuleComponent* Component, int32 Index)
{
	if (!Component)
		return;
	Component->InventoryModule->UpdateOnIndex(Index);

}




void UPowerSuitRCO::ServerSetSuitMovementProperty_Implementation(APowerSuitModuleAttachment* Attachment, ESuitMovementProperty Enum, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetSuitMovementProperty(Enum, Property);

}

void UPowerSuitRCO::ServerSetSuitFlightProperty_Implementation(APowerSuitModuleAttachment* Attachment, ESuitFlightProperty Enum, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetSuitFlightProperty(Enum, Property);


}

void UPowerSuitRCO::ServerSetSuitProperty_Implementation(APowerSuitModuleAttachment* Attachment, ESuitProperty Enum, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetSuitProperty(Enum, Property);

}

void UPowerSuitRCO::ServerSetPropertyGeneral_Implementation(APowerSuitModuleAttachment* Attachment, EEquipmentStatsProperty PropertyType, uint8 Index, FModMultProperty Property)
{
	if (!Attachment)
		return;
	Attachment->SetPropertyGeneral(PropertyType,Index,  Property);
}

void UPowerSuitRCO::ServerSetSuitFlag_Implementation(APowerSuitModuleAttachment* Attachment, ESuitFlag Flag, bool Enabled)
{
	if (!Attachment)
		return;
	Attachment->SetSuitFlag(Flag, Enabled);
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
}

