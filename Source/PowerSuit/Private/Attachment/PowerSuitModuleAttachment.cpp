#include "Attachment\PowerSuitModuleAttachment.h"
#include "Replication/PowerSuitRCO.h"


APowerSuitModuleAttachment::APowerSuitModuleAttachment()
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
}


void APowerSuitModuleAttachment::PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion)
{
}
void APowerSuitModuleAttachment::PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion)
{
}

void APowerSuitModuleAttachment::PreLoadGame_Implementation(int32 saveVersion, int32 gameVersion)
{
}

void APowerSuitModuleAttachment::PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion)
{
}

void APowerSuitModuleAttachment::GatherDependencies_Implementation(TArray<UObject*>& out_dependentObjects)
{
}

bool APowerSuitModuleAttachment::ShouldSave_Implementation() const
{
	return false;
}

bool APowerSuitModuleAttachment::NeedTransform_Implementation()
{
	return false;
}

float APowerSuitModuleAttachment::ReceiveDamage_Implementation(float damageAmount, const class UDamageType* damageType, class AController* instigatedBy, AActor* damageCauser)
{
	return damageAmount;
}






FEquipmentStats APowerSuitModuleAttachment::ReceiveModuleStats_Implementation(FEquipmentStats DefaultStats) { return DefaultStats; };

void APowerSuitModuleAttachment::AttachToSuit_Implementation(AFGEquipment * EquipmentParent) {
	AttachToActor(EquipmentParent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
};
void APowerSuitModuleAttachment::DetachFromSuit_Implementation() {
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
};

void APowerSuitModuleAttachment::AttachmentInstalled_Implementation(FInventoryItem Item) {  };
void APowerSuitModuleAttachment::AttachmentUnInstalled_Implementation() {  };

float APowerSuitModuleAttachment::GetDeltaPowerProduction_Implementation(float DeltaTime)const { return 0.0f; };

float APowerSuitModuleAttachment::GetDeltaPowerConsumption_Implementation(float DeltaTime)const  { return 0.0f;};

float APowerSuitModuleAttachment::GetDeltaFuelConsumption_Implementation(float DeltaTime)const { return 0.0f;};

bool APowerSuitModuleAttachment::GetIsConditionMet_Implementation()const {return true;};

void APowerSuitModuleAttachment::AttachmentPreTick_Implementation(float DeltaTime) { };
void APowerSuitModuleAttachment::AttachmentTick_Implementation(float DeltaTime) { };

void APowerSuitModuleAttachment::AttachmentPostTick_Implementation(float DeltaTime) { };

void APowerSuitModuleAttachment::ServerSetFloatValue_Implementation(float Value, uint8 Index) {};


void APowerSuitModuleAttachment::ServerSetBoolValue_Implementation(bool Value, uint8 Index)
{

};



void APowerSuitModuleAttachment::SetSuitMovementProperty(ESuitMovementProperty Enum, FModMultProperty Property)
{
	if (!HasAuthority())
	{
		ParentModule->RCO->ServerSetSuitMovementProperty(this, Enum, Property);
	}

	if (Enum >= ESuitMovementProperty::ESMC_JumpZVelocity && Enum <= ESuitMovementProperty::ESMC_MAX)
	{
		AttachmentStats.nMovementProperties.Add(Enum, Property);
	}
}

void APowerSuitModuleAttachment::SetSuitFlightProperty(ESuitFlightProperty Enum, FModMultProperty Property)
{
	if (!HasAuthority())
	{
		ParentModule->RCO->ServerSetSuitFlightProperty(this, Enum, Property);
	}
	if (Enum >= ESuitFlightProperty::EFP_mHoverSpeed && Enum <= ESuitFlightProperty::EFP_MAX)
	{
		AttachmentStats.nFlightProperties.Add(Enum, Property);
	}
}

void APowerSuitModuleAttachment::SetSuitProperty(ESuitProperty Enum, FModMultProperty Property)
{
	if (!HasAuthority())
	{
		ParentModule->RCO->ServerSetSuitProperty(this, Enum, Property);
	}
	if (Enum >= ESuitProperty::nPowerCapacity && Enum <= ESuitProperty::SuitPropertyMAX)
	{
		AttachmentStats.nSuitProperties.Add(Enum, Property);
	}
}



void APowerSuitModuleAttachment::SetPropertyGeneral(EEquipmentStatsProperty PropertyType, uint8 Index, FModMultProperty Property)
{
	if (!HasAuthority())
	{
		ParentModule->RCO->ServerSetPropertyGeneral(this, PropertyType,Index, Property);
	}
	switch (PropertyType)
	{
	case EEquipmentStatsProperty::ESSP_nSuitProperties:
	{
		SetSuitProperty( ESuitProperty(Index), Property);
		break;
	}
	case EEquipmentStatsProperty::ESSP_nMovementProperties:
	{
		SetSuitMovementProperty(ESuitMovementProperty(Index), Property);
		break;
	}
	case EEquipmentStatsProperty::ESSP_nFlightProperties:
	{
		SetSuitFlightProperty(ESuitFlightProperty(Index), Property);
		break;
	}
	default:
		break;
	}

}

void APowerSuitModuleAttachment::SetSuitFlag(ESuitFlag Flag, bool Enabled)
{
	if (!HasAuthority())
	{
		ParentModule->RCO->ServerSetSuitFlag(this, Flag, Enabled);
	}
	const bool Has = AttachmentStats.HasFlag(Flag);
	if (Has && Enabled)
		return;
	else if (!Has && Enabled)
	{
		AttachmentStats.SuitFlags += Flag;
	}
	else if (Has && !Enabled)
	{
		AttachmentStats.SuitFlags -= Flag;
	}
}

