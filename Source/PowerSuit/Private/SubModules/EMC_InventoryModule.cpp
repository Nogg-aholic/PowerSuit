#include "SubModules/EMC_InventoryModule.h"
#include "SubModules/EMC_HealthModule.h"
#include "SubModules/EMC_AttachmentModule.h"
#include "SubModules/EMC_FuelModule.h"

#include "FGInventoryLibrary.h"


#include "PowerSuitBPLibrary.h"
#include "Equipment/Equipment_PowerSuit.h"
AFGCharacterPlayer* UEMC_InventoryModule::InitInventory()
{
	if (Parent->nInventory)
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("InitInventory %s"), *Parent->nInventory->GetName());

	}
	else
		UE_LOG(PowerSuit_Log, Display, TEXT("InitInventory Invalid"));


	AFGCharacterPlayer* Character = Cast< AFGCharacterPlayer>(Parent->EquipmentParent->GetInstigator());
	if (!Character)
		return nullptr;
	if (!Parent->nInventory)
	{
		
		if (Parent->EquipmentParent->HasAuthority())
		{
			Parent->nInventory = UFGInventoryLibrary::CreateInventoryComponent(Character, FName("nInventory"));
			UE_LOG(PowerSuit_Log, Display,TEXT("Created Inventory; Calling Owning Client for Replication"))
			Parent->EquipmentParent->Server_WaitAndInitRemote();
			Parent->ResetStats();
			Parent->nProducing = true;
		}

	}
	else
	{
		BulkUpdateStats(Parent->nInventory);
		// setup binding to Inventory changes 
	}

	if (!Parent->nInventory)
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("No Inventory Component on Init, the Inventory Component hasnt Replicated yet or Shit´s broken"))

	}

	if (Parent->EquipmentParent->HasAuthority())
	{
		Parent->nInventory->OnItemAddedDelegate.AddDynamic(this, &UEMC_InventoryModule::RefreshInventoryAdd);
		Parent->nInventory->OnItemRemovedDelegate.AddDynamic(this, &UEMC_InventoryModule::RefreshInventoryRemove);

		// maybe redundant but incase the BP fails to compile , maybe we dont want to crash bc of it
		if (Parent->Stats.InventorySlots == 0)
		{
			FString out = Parent->EquipmentParent->GetName();
			UE_LOG(PowerSuit_Log, Warning, TEXT("No Inventory SlotStats on this Suit ?! Did this BP fail to compile or is this intentional? %s "), *out);
			return Character;
		}


		if (!Parent->GetIsReplicated())
			Parent->SetIsReplicated(true);

		// make sure we are replicated
		if (!Parent->nInventory->GetIsReplicated())
			Parent->nInventory->SetIsReplicated(true);

		UpdateInventorySize();
	}
	return Character;
}


void UEMC_InventoryModule::UpdateInventorySize()
{
	UE_LOG(PowerSuit_Log, Display, TEXT("UpdateInventorySize"));

	if (!Parent->EquipmentParent->HasAuthority())
		return;

	// Size is loaded so skip setting it if Size is the same  
	if (Parent->nInventory->GetSizeLinear() != Parent->Stats.InventorySlots)
		Parent->nInventory->Resize(Parent->Stats.InventorySlots);

	// Limit Slots to SlotSize of 1
	// Modules may stack but they must be Single when accounted for
	for (int32 i = 0; i < Parent->Stats.InventorySlots; i++)
	{
		if(Parent->nInventory->GetSlotSize(i) != 1)
			Parent->nInventory->AddArbitrarySlotSize(i, 1);
	}
}

void UEMC_InventoryModule::ResetInventoryStats()
{
	UE_LOG(PowerSuit_Log, Display, TEXT("ResetInventoryStats"));

	ItemsRemembered.Empty();
	UniquesActive.Empty();
	Parent->FuelModule->nAllowedFuels.Empty();

}


void UEMC_InventoryModule::RefreshInventoryRemove(TSubclassOf<UFGItemDescriptor> itemClass, int32 numAdded)
{
	UE_LOG(PowerSuit_Log, Display, TEXT("RefreshInventoryRemove"));

	if (!ItemsRemembered.Contains(itemClass))
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("An Item was removed we didnt know was in there ?! "));
		RefreshInventory();
	}
	else
	{
		TArray<FEquipmentStats>&  SlotStats = ItemsRemembered.Find(itemClass)->Stats;
		int32 RemoveIdx = -1;
		int32 idx = 0;

		for (FEquipmentStats & i : SlotStats)
		{
			// looking for what was here once but isnt anymore
			if (!Parent->nInventory->IsSomethingOnIndex(i.mCachedInventorySlot))
			{
				if (RemoveIdx != -1)
				{
					UE_LOG(PowerSuit_Log, Error, TEXT("More items missing than expected; Doing Full re-Evaluation of Stats"));
					RefreshInventory();
					return;
				}
				else
				{
					RemoveIdx = idx;
				}
			}
			else
			{
				FInventoryStack Stack;
				Parent->nInventory->GetStackFromIndex(i.mCachedInventorySlot, Stack);
				if (Stack.Item.ItemClass != itemClass)
				{
					// this Slot was replaced directly ? 
					// fuck this .. lets just update all 
					RefreshInventory();
					return;
				}
			}
			idx++;
		}

		//FInventoryStack Stack;
		//Parent->nInventory->GetStackFromIndex(SlotStats[RemoveIdx].mCachedInventorySlot, Stack);

		if (itemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()) && SlotStats.IsValidIndex(RemoveIdx))
		{
			TSubclassOf<class UEquipmentModuleDescriptor> Item = itemClass;
			SubtractModuleStats(Item, SlotStats[RemoveIdx].mCachedInventorySlot);
		}
		else
		{
			UE_LOG(PowerSuit_Log, Error, TEXT("No Valid Index Found or Item is not ModuleDescriptor"));

			RefreshInventory();
		}
	}
	UPowerSuitBPLibrary::UpdateAllNoRefresh(Parent->EquipmentParent);

	if (Parent->EquipmentParent->HasAuthority())
	{
		Parent->RemoteInventoryRefresh(false,itemClass,numAdded);
		UE_LOG(PowerSuit_Log, Display, TEXT("Calling Remote with Refresh Add : False"));
	}
}



void UEMC_InventoryModule::RefreshInventory()
{
	UE_LOG(PowerSuit_Log, Display, TEXT("RefreshInventory"));

	if (Parent->EquipmentParent && Parent->nInventory)
		BulkUpdateStats(Parent->nInventory);
}

void UEMC_InventoryModule::RefreshInventoryAdd(TSubclassOf<UFGItemDescriptor> itemClass, int32 numAdded)
{
	UE_LOG(PowerSuit_Log, Display, TEXT("RefreshInventoryAdd"));

	int32 NewIndex = -1; int32 ItemsLeft = numAdded;
	TArray<TSubclassOf<UFGItemDescriptor>> RelevantClasses; RelevantClasses.Add(itemClass);
	TArray<int32> RelevantSlotIndexes = Parent->nInventory->GetRelevantStackIndexes(RelevantClasses);
	while (ItemsLeft > 0)
	{
		if (ItemsRemembered.Contains(itemClass))
		{
			TArray<FEquipmentStats> & RememberedStats = ItemsRemembered.Find(itemClass)->Stats;
			// Iterate remembered Modules of that Type to remove Irrelevant Indexes
			for (FEquipmentStats RememberedStat : RememberedStats)
			{
				if (!RelevantSlotIndexes.Contains(RememberedStat.mCachedInventorySlot))
				{
					UE_LOG(PowerSuit_Log, Error, TEXT("Item Expected to be there isnt, doing Re-Evaluation"));
					RefreshInventory();
					return;
				}
				else
				{
					// checked and still valid
					RelevantSlotIndexes.Remove(RememberedStat.mCachedInventorySlot);
				}
			}
			// Left should be the Amount we added 
			if (RelevantSlotIndexes.Num() != ItemsLeft)
			{
				UE_LOG(PowerSuit_Log, Error, TEXT("Item Amount Mismatch doing Re- Evaluation"));
				RefreshInventory();
				return;
			}
			for (int32 i : RelevantSlotIndexes)
			{
				NewIndex = i;
				break;
			}
			if (!MergeOnIndex(NewIndex))
			{
				UE_LOG(PowerSuit_Log, Error, TEXT("Logic failed to resolve Stack index doing Re-Evaluation"));
				RefreshInventory();
				return;
			}
		}
		else
		{
			for (int32 i : RelevantSlotIndexes)
			{
				NewIndex = i;
				if (MergeOnIndex(NewIndex))
					break;
			}
		}
		--ItemsLeft;
	}

	UPowerSuitBPLibrary::UpdateAllNoRefresh(Parent->EquipmentParent);
	if (Parent->EquipmentParent->GetInstigator()->HasAuthority())
	{
		Parent->RemoteInventoryRefresh(true, itemClass, numAdded);
	}

}





void UEMC_InventoryModule::MergeStats(FInventoryStack Stack, FEquipmentStats & StatsRef)
{
	if (!Stack.Item.ItemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()))
	{
		return;
	}
	if (Stack.Item.HasState())
	{
		APowerSuitModuleAttachment* Equipment = Cast< APowerSuitModuleAttachment>(Stack.Item.ItemState.Get());
		if (Equipment)
		{
			if (Equipment->GetIsConditionMet())
			{
				if (Parent->AttachmentModule->InactiveAttachments.Contains(Equipment))
					Parent->AttachmentModule->InactiveAttachments.Remove(Equipment);

				if (!Parent->AttachmentModule->Attachments.Contains(Equipment))
					Parent->AttachmentModule->Attachments.Add(Equipment);

				Equipment->AttachToSuit(Parent->EquipmentParent);
				Equipment->AttachmentInstalled(Stack.Item);
				Equipment->InventorySlot = StatsRef.mCachedInventorySlot;
				StatsRef.mCachedAttachment = Equipment;
				Parent->Stats + StatsRef;
				const TSubclassOf< class UEquipmentModuleDescriptor> item = Stack.Item.ItemClass;
				StatsRef.UnlockFuels(Parent, item.GetDefaultObject()->nAllowedFuels);
			}
			else
			{
				if (Parent->AttachmentModule->Attachments.Contains(Equipment))
					Parent->AttachmentModule->Attachments.Remove(Equipment);

				if (!Parent->AttachmentModule->InactiveAttachments.Contains(Equipment))
					Parent->AttachmentModule->InactiveAttachments.Add(Equipment);
			}
		}
	}
	else
	{
		
		Parent->Stats + StatsRef;
		const TSubclassOf< class UEquipmentModuleDescriptor> item = Stack.Item.ItemClass;
		StatsRef.UnlockFuels(Parent, item.GetDefaultObject()->nAllowedFuels);
		
	}


	if (ItemsRemembered.Contains(Stack.Item.ItemClass))
	{
		ItemsRemembered.Find(Stack.Item.ItemClass)->Stats.Add(StatsRef);
	}
	else
	{
		FEquipmentStatStack Insert; Insert.Stats.Add(StatsRef);
		ItemsRemembered.Add(Stack.Item.ItemClass, Insert);
	}


}





void UEMC_InventoryModule::BulkUpdateStats(UFGInventoryComponent * Inventory)
{
	UE_LOG(PowerSuit_Log, Display, TEXT("BulkUpdate Stats"));

	Parent->ResetStats();
	if (Inventory)
	{
		TArray< FInventoryStack > out_stacks;
		Inventory->GetInventoryStacks(out_stacks);

		int32 num = Inventory->GetSizeLinear();

		for (int32 ind = 0; ind < num; ind++)
		{
			MergeOnIndex(ind);
		}
	}

	Parent->HealthModule->SetMaxHealth();

	UPowerSuitBPLibrary::UpdateAllNoRefresh(Parent->EquipmentParent);
}

bool UEMC_InventoryModule::MergeOnIndex(int32 ind, bool Safe)
{
	if (Parent->nInventory->IsSomethingOnIndex(ind))
	{
		FInventoryStack Stack;
		Parent->nInventory->GetStackFromIndex(ind, Stack);
		if (Stack.Item.ItemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()))
		{
			FEquipmentStats StatsRef = GetModuleStats(Stack, ind);
			MergeStats(Stack, StatsRef);
		}
		return true;
	}
	else if (!Safe)
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("Tried to Merge Stats on Index with no Item"));
		return false;
	}
	else
		return false;
}

bool UEMC_InventoryModule::UpdateOnIndex(int32 index)
{
	if (index == -1)
		return false;
	if (Parent->nInventory->IsSomethingOnIndex(index))
	{
		FInventoryStack Stack;
		Parent->nInventory->GetStackFromIndex(index, Stack);
		if (Stack.HasItems())
		{
			if (!ItemsRemembered.Contains(Stack.Item.ItemClass))
			{
				UE_LOG(PowerSuit_Log, Error, TEXT("Item not Remembered ? Update full Inventory"));
				RefreshInventory();
				return true;
			}
			else
			{
				TArray<FEquipmentStats> & Arr = ItemsRemembered.Find(Stack.Item.ItemClass)->Stats;
				int32 indexToRemove = 0;
				for (FEquipmentStats& i : Arr)
				{
					if (i.mCachedInventorySlot == index)
					{
						Stack.Item.ItemClass;
						// found
						TSubclassOf< class UEquipmentModuleDescriptor> item = Stack.Item.ItemClass;

						SubtractModuleStats(item,index);
						FEquipmentStats StatsRef = GetModuleStats(Stack, index);
						MergeStats(Stack, StatsRef);
						UPowerSuitBPLibrary::UpdateAllNoRefresh(Parent->EquipmentParent);
						return true;
					}

				}
			}
		}
	}
	return false;
}


void UEMC_InventoryModule::PreTick()
{
}
void UEMC_InventoryModule::Tick()
{
}

void UEMC_InventoryModule::PostTick()
{
}

void  UEMC_InventoryModule::SubtractModuleStats(TSubclassOf< class UEquipmentModuleDescriptor> item, int32 Index)
{
	if (!item)
		return;

	if (!item->IsChildOf(UEquipmentModuleDescriptor::StaticClass()))
		return;

	const UEquipmentModuleDescriptor* ItemObj = item.GetDefaultObject();

	if (!ItemsRemembered.Contains(item))
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("An Item was removed we didnt know was in there ?! "));
		RefreshInventory();
	}
	else
	{

		TArray<FEquipmentStats>& Arr = ItemsRemembered.Find(item)->Stats;
		int32 indexToRemove = 0;
		APowerSuitModuleAttachment* Equipment = nullptr;
		for (FEquipmentStats& i : Arr)
		{
			if (i.mCachedInventorySlot == Index)
			{
				Parent->Stats - i;
				i.ForgetUnlockedFuels(Parent);
				Equipment = i.mCachedAttachment;
				Arr.RemoveAt(indexToRemove);
				FString nameout = Equipment->GetName();
				UE_LOG(PowerSuit_Log, Display, TEXT("Subtracted Stats from : %s Remebered Stats Left: %s ; Keys Total %s "), *nameout,*FString::FromInt(Arr.Num()), *FString::FromInt(ItemsRemembered.Num()));
				break;
			}
			indexToRemove++;
		}


		if (ItemObj->GetnUniqueUsage(item))
			if (UniquesActive.Contains(item))
			{
				UniquesActive.Remove(item);
			}

			
		if (Equipment)
		{
			if (Parent->AttachmentModule->InactiveAttachments.Contains(Equipment))
				Parent->AttachmentModule->InactiveAttachments.Remove(Equipment);
			else if (Parent->AttachmentModule->Attachments.Contains(Equipment))
				Parent->AttachmentModule->Attachments.Remove(Equipment);
		}
		
	}
}

FEquipmentStats UEMC_InventoryModule::GetModuleStats(FInventoryStack Stack, int32 ind)
{
	FEquipmentStats StatObject;

	if (!Stack.HasItems())
	{

		UE_LOG(PowerSuit_Log, Error, TEXT("Stack has no Items ??"))
		StatObject = FEquipmentStats();
		StatObject.mCachedInventorySlot = ind;
		return StatObject;
	}

	if (Stack.Item.ItemClass)
	{
		if (!Stack.Item.ItemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()))
			return FEquipmentStats();
		const TSubclassOf< class UEquipmentModuleDescriptor> item = Stack.Item.ItemClass;


		const UEquipmentModuleDescriptor* ItemObj = item.GetDefaultObject();

		// if this is a Unique Use Module and we have one of this kind Accounted for already we skip it
		if (ItemObj->GetnUniqueUsage(item))
			if (UniquesActive.Contains(item))
			{

				UE_LOG(PowerSuit_Log, Warning, TEXT("Ignored Item %s because of an Already active Unique Item"), *ItemObj->mDisplayName.ToString())
				StatObject = FEquipmentStats();
				StatObject.mCachedInventorySlot = ind;
				return StatObject;
			}
			else
				UniquesActive.Add(item);

		CreateAttachmentStateIfNeeded(Stack, ItemObj, item, ind);

		
		if (Stack.Item.HasState())
		{
			APowerSuitModuleAttachment* Equipment = Cast< APowerSuitModuleAttachment>(Stack.Item.ItemState.Get());
			if (Equipment)
			{
				if (Equipment->GetIsConditionMet())
				{
					Equipment->Parent = Parent;
					StatObject = Equipment->RecieveModuleStats(ItemObj->EquipmentStats);
				}
				else
				{
					StatObject = FEquipmentStats();
				}
				StatObject.mCachedAttachment = Equipment;
			}
			else
			{
				StatObject = ItemObj->EquipmentStats;
			}
		}
		else
		{
			if (!Parent->EquipmentParent->HasAuthority())
			{
				if (ItemObj->GetnAttachment(item))
				{
					UE_LOG(PowerSuit_Log, Error, TEXT("Remote has not ItemState, Debug me"));
				}
			}
			StatObject = ItemObj->EquipmentStats;
		}
		StatObject.mCachedInventorySlot = ind;
		return StatObject;
	}
	StatObject = FEquipmentStats();
	StatObject.mCachedInventorySlot = ind;
	return StatObject;
}

void UEMC_InventoryModule::CreateAttachmentStateIfNeeded(FInventoryStack Stack, const UEquipmentModuleDescriptor* ItemObj, const TSubclassOf< class UEquipmentModuleDescriptor> item, const int32 index)
{
	if (ItemObj->GetnAttachment(item) && !Stack.Item.HasState())
	{
		if (Parent->EquipmentParent && Parent->EquipmentParent->HasAuthority())
		{
			FVector  Loc = FVector(0, 0, 0);
			FRotator Rot = FRotator(0, 0, 0);
			FActorSpawnParameters SpawnSettings = FActorSpawnParameters();
			SpawnSettings.bNoFail = true;
			SpawnSettings.Owner = Parent->EquipmentParent->GetOwner();
			SpawnSettings.Instigator = Parent->EquipmentParent->GetInstigator();
			APowerSuitModuleAttachment* Equipment = Cast< APowerSuitModuleAttachment>(Parent->EquipmentParent->GetWorld()->SpawnActor(ItemObj->GetnAttachment(item), &Loc, &Rot, SpawnSettings));
			check(Equipment);
			Equipment->SetReplicates(true);
			Equipment->Parent = Parent;
			Parent->nInventory->SetStateOnIndex(index, FSharedInventoryStatePtr::MakeShared(Equipment));
			UE_LOG(PowerSuit_Log, Display, TEXT("Created Module Descriptor Attachment Actor %s"), *Equipment->GetName());
		}
	}
}
