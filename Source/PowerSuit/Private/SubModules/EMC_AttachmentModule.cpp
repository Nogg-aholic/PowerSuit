#include "SubModules/EMC_AttachmentModule.h"
#include "SubModules/EMC_PowerModule.h"

#include "SubModules/EMC_InventoryModule.h"
// Server And Client
void UEMC_AttachmentModule::PreTick()
{
	for (auto * i : InactiveAttachments)
	{
		if (i)
			if (i->GetIsConditionMet())
			{
				UE_LOG(LogPowerSuitCpp,Warning,TEXT("Attachment : %s caused Update on Index %i while going Active"), *i->GetName(), i->InventorySlot)
				Parent->InventoryModule->UpdateOnIndex(i->InventorySlot);
				return;
			}
			else
			{
				i->AttachmentPreTick(Parent->LastDeltaTime);
			}
	}
	for (auto * i : Attachments)
	{
		if (i)
		{
			if (!i->GetIsConditionMet())
			{
				UE_LOG(LogPowerSuitCpp, Warning, TEXT("Attachment : %s caused Update on Index %i while going inActive"), *i->GetName(), i->InventorySlot)
				Parent->InventoryModule->UpdateOnIndex(i->InventorySlot);
				return;
			}
			else
			{
				i->AttachmentPreTick(Parent->LastDeltaTime);
			}
		}
	}
}
// Server and Client
void UEMC_AttachmentModule::Tick()
{
	for (auto * i : InactiveAttachments)
	{
		if (i)
			i->AttachmentTick(Parent->LastDeltaTime);
	}
	for (auto * i : Attachments)
	{
		if (i)
			i->AttachmentTick(Parent->LastDeltaTime);
	}
}
// Server and Client
void UEMC_AttachmentModule::PostTick()
{
	for (auto * i :InactiveAttachments)
	{
		if (i)
			i->AttachmentPostTick(Parent->LastDeltaTime);
	}
	for (auto * i : Attachments)
	{
		if (i)
			i->AttachmentPostTick(Parent->LastDeltaTime);
	}
}

// Server and Client
void UEMC_AttachmentModule::ResetAttachments()
{
	if (!IsValid(this))
		return;
	if (!Parent->GetOwner())
		return;

	if (Parent->GetOwner()->HasAuthority())
	{
		// Destroy Attachments
		for (auto i : Attachments)
			if (i)
			{
				i->AttachmentUnInstalled();
				i->DetachFromSuit();
				i->bIsActive = false;
				i->SetInstigator(nullptr);
				i->SetOwner(nullptr);
			}
		// Destroy Attachments
		for (auto i : InactiveAttachments)
			if (i)
			{
				i->AttachmentUnInstalled();
				i->DetachFromSuit();
				i->bIsActive = false;
				i->SetInstigator(nullptr);
				i->SetOwner(nullptr);
			}
	}
	Attachments.Empty();
	InactiveAttachments.Empty();
}

