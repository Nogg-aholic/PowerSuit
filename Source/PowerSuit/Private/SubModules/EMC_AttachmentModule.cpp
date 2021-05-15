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
				Parent->InventoryModule->UpdateOnIndex(i->InventorySlot);
			}
			else
			{
				i->AttachmentPreTick(Parent->Delta);
			}
	}
	for (auto * i : Attachments)
	{
		if (i)
		{
			if (!i->GetIsConditionMet())
			{
				Parent->InventoryModule->UpdateOnIndex(i->InventorySlot);
				return;
			}
			else
			{
				i->AttachmentPreTick(Parent->Delta);
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
			i->AttachmentTick(Parent->Delta);
	}
	for (auto * i : Attachments)
	{
		if (i)
			i->AttachmentTick(Parent->Delta);
	}
}
// Server and Client
void UEMC_AttachmentModule::PostTick()
{
	for (auto * i :InactiveAttachments)
	{
		if (i)
			i->AttachmentPostTick(Parent->Delta);
	}
	for (auto * i : Attachments)
	{
		if (i)
			i->AttachmentPostTick(Parent->Delta);
	}
}

// Server and Client
void UEMC_AttachmentModule::ResetAttachments()
{
	if (IsPendingKill())
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

