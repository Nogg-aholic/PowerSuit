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
			if(i->GetShouldTriggerActive())
			{
				Parent->PowerModule->GoActive();
				break;
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
	// Destroy Attachments
	for (auto i : Attachments)
		if (i)
		{
			i->AttachmentUnInstalled();
			i->DetachFromSuit();
			i->bIsActive = false;
		}
	// Destroy Attachments
	for (auto i : InactiveAttachments)
		if (i)
		{
			i->AttachmentUnInstalled();
			i->DetachFromSuit();
			i->bIsActive = false;
		}
	Attachments.Empty();
	InactiveAttachments.Empty();
}

