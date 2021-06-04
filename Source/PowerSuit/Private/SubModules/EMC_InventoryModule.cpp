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
			Parent->nInventory = UFGInventoryLibrary::CreateInventoryComponent(Character, FName("nInventory" + Parent->EquipmentParent->GetName()));
			UE_LOG(PowerSuit_Log, Display,TEXT("Created Inventory; Calling Owning Client for Replication"))
			Parent->EquipmentParent->Server_WaitAndInitRemote();
			Parent->ResetStats();
			Parent->nProducing = false;
		}

	}
	else
	{
		if (Parent->EquipmentParent->HasAuthority())
		{
			Parent->EquipmentParent->Server_WaitAndInitRemote();
			Parent->ResetStats();
			Parent->nProducing = false;
		}
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
	if (Parent->nInventory->GetSizeLinear() < Parent->Stats.InventorySlots)
		Parent->nInventory->Resize(Parent->Stats.InventorySlots);
	else if (Parent->nInventory->GetSizeLinear() > Parent->Stats.InventorySlots)
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("SuitInventory shrinking, we dont check if slots are filled :I"));
		Parent->nInventory->Resize(Parent->Stats.InventorySlots);

	}
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
	for (auto i : Parent->EquipmentParent->mCostToUse)
	{
		Parent->FuelModule->nAllowedFuels.Add(i.ItemClass);
	}
}


void UEMC_InventoryModule::RefreshInventoryRemove(TSubclassOf<UFGItemDescriptor> ItemClass, int32 NumAdded)
{
	UE_LOG(PowerSuit_Log, Display, TEXT("RefreshInventoryRemove"));

	if (!ItemsRemembered.Contains(ItemClass))
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("An Item was removed we didnt know was in there ?! "));
		RefreshInventory();
	}
	else
	{
		TArray<FEquipmentStats>&  SlotStats = ItemsRemembered.Find(ItemClass)->Stats;
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
				if (Stack.Item.ItemClass != ItemClass)
				{
					UE_LOG(PowerSuit_Log, Error, TEXT("this Slot was replaced directly ? "));

					// 
					// fuck this .. lets just update all 
					RefreshInventory();
					return;
				}
			}
			idx++;
		}

		//FInventoryStack Stack;
		//Parent->nInventory->GetStackFromIndex(SlotStats[RemoveIdx].mCachedInventorySlot, Stack);

		if (ItemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()) && SlotStats.IsValidIndex(RemoveIdx))
		{
			const TSubclassOf<class UEquipmentModuleDescriptor> Item = ItemClass;
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
		Parent->RemoteInventoryRefresh(false,ItemClass,NumAdded);
		UE_LOG(PowerSuit_Log, Display, TEXT("Calling Remote with Refresh Add : False"));
	}
}



void UEMC_InventoryModule::RefreshInventory()
{
	UE_LOG(PowerSuit_Log, Display, TEXT("RefreshInventory"));

	if (Parent->EquipmentParent && Parent->nInventory)
		BulkUpdateStats(Parent->nInventory);
}

void UEMC_InventoryModule::RefreshInventoryAdd(TSubclassOf<UFGItemDescriptor> ItemClass, int32 NumAdded)
{
	UE_LOG(PowerSuit_Log, Display, TEXT("RefreshInventoryAdd"));

	int32 NewIndex = -1; int32 ItemsLeft = NumAdded;
	TArray<TSubclassOf<UFGItemDescriptor>> RelevantClasses; RelevantClasses.Add(ItemClass);
	TArray<int32> RelevantSlotIndexes = Parent->nInventory->GetRelevantStackIndexes(RelevantClasses);
	while (ItemsLeft > 0)
	{
		if (ItemsRemembered.Contains(ItemClass))
		{
			TArray<FEquipmentStats> & RememberedStats = ItemsRemembered.Find(ItemClass)->Stats;
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
	if (Parent->EquipmentParent->GetInstigator()->HasAuthority() && !Parent->EquipmentParent->GetInstigator()->IsLocallyControlled())
	{
		Parent->RemoteInventoryRefresh(true, ItemClass, NumAdded);
	}

}





void UEMC_InventoryModule::MergeStats(FInventoryStack Stack, FEquipmentStats & StatsRef)
{
	if (!Stack.Item.ItemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()))
	{
		return;
	}
	if (Stack.Item.HasState() || StatsRef.mCachedAttachment)
	{
		APowerSuitModuleAttachment* Attachment = StatsRef.mCachedAttachment ? StatsRef.mCachedAttachment : Cast< APowerSuitModuleAttachment>(Stack.Item.ItemState.Get());
		if (Attachment)
		{
			if (Attachment->GetIsConditionMet())
			{
				if (Parent->AttachmentModule->InactiveAttachments.Contains(Attachment))
					Parent->AttachmentModule->InactiveAttachments.Remove(Attachment);

				if (!Parent->AttachmentModule->Attachments.Contains(Attachment))
					Parent->AttachmentModule->Attachments.Add(Attachment);

				Attachment->InventorySlot = StatsRef.mCachedInventorySlot; // This happens before events so that InventorySlot is valid when the events process things
				Attachment->ParentModule = Parent;
				Attachment->AttachToSuit(Parent->EquipmentParent);
				Attachment->SetOwner(Parent->EquipmentParent->GetOwner());
				Attachment->SetInstigator(Parent->EquipmentParent->GetInstigator());
				Attachment->AttachmentInstalled(Stack.Item);
				
				StatsRef.mCachedAttachment = Attachment;
				Parent->Stats + StatsRef;
				const TSubclassOf< class UEquipmentModuleDescriptor> Item = Stack.Item.ItemClass;
				StatsRef.UnlockFuels(Parent, Item.GetDefaultObject()->nAllowedFuels);
			}
			else
			{
				if (Parent->AttachmentModule->Attachments.Contains(Attachment))
					Parent->AttachmentModule->Attachments.Remove(Attachment);

				if (!Parent->AttachmentModule->InactiveAttachments.Contains(Attachment))
					Parent->AttachmentModule->InactiveAttachments.Add(Attachment);
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

bool UEMC_InventoryModule::MergeOnIndex(const int32 Ind, const bool Safe)
{
	if (Parent->nInventory->IsSomethingOnIndex(Ind))
	{
		FInventoryStack Stack;
		Parent->nInventory->GetStackFromIndex(Ind, Stack);
		if (Stack.Item.ItemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()))
		{
			FEquipmentStats StatsRef = GetModuleStats(Stack, Ind);
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

bool UEMC_InventoryModule::UpdateOnIndex(const int32 Index)
{
	if (Index == -1)
		return false;
	if (Parent->nInventory->IsSomethingOnIndex(Index))
	{
		FInventoryStack Stack;
		Parent->nInventory->GetStackFromIndex(Index, Stack);
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
					if (i.mCachedInventorySlot == Index)
					{
						Stack.Item.ItemClass;
						// found
						TSubclassOf< class UEquipmentModuleDescriptor> item = Stack.Item.ItemClass;

						SubtractModuleStats(item,Index);
						FEquipmentStats StatsRef = GetModuleStats(Stack, Index);
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

void  UEMC_InventoryModule::SubtractModuleStats(const TSubclassOf< class UEquipmentModuleDescriptor> Item, const int32 Index)
{
	if (!Item)
		return;

	if (!Item->IsChildOf(UEquipmentModuleDescriptor::StaticClass()))
		return;

	const UEquipmentModuleDescriptor* ItemObj = Item.GetDefaultObject();

	if (!ItemsRemembered.Contains(Item))
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("An Item was removed we didnt know was in there ?! "));
		RefreshInventory();
	}
	else
	{

		TArray<FEquipmentStats>& Arr = ItemsRemembered.Find(Item)->Stats;
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
				FString Out = "";
				if (Equipment)
				{
					Out = Equipment->GetName();
				}
				else
				{
					Out = Item->GetName();
				}
				UE_LOG(PowerSuit_Log, Display, TEXT("Subtracted Stats from : %s Remebered Stats Left: %s ; Keys Total %s "), *Out, *FString::FromInt(Arr.Num()), *FString::FromInt(ItemsRemembered.Num()));

				break;
			}
			indexToRemove++;
		}


		if (ItemObj->GetnUniqueUsage(Item))
			if (UniquesActive.Contains(Item))
			{
				UniquesActive.Remove(Item);
			}

			
		if (Equipment)
		{
			if (Parent->AttachmentModule->InactiveAttachments.Contains(Equipment))
				Parent->AttachmentModule->InactiveAttachments.Remove(Equipment);
			else if (Parent->AttachmentModule->Attachments.Contains(Equipment))
				Parent->AttachmentModule->Attachments.Remove(Equipment);

			Equipment->DetachFromSuit();
			Equipment->AttachmentUnInstalled();
			Equipment->ParentModule = nullptr;
		}
	}
}

FEquipmentStats UEMC_InventoryModule::GetModuleStats(const FInventoryStack Stack, const int32 Ind)
{
	FEquipmentStats StatObject;

	if (!Stack.HasItems())
	{

		UE_LOG(PowerSuit_Log, Error, TEXT("Stack has no Items ??"))
		StatObject = FEquipmentStats();
		StatObject.mCachedInventorySlot = Ind;
		return StatObject;
	}

	if (Stack.Item.ItemClass)
	{
		if (!Stack.Item.ItemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()))
			return FEquipmentStats();
		const TSubclassOf< class UEquipmentModuleDescriptor> Item = Stack.Item.ItemClass;


		const UEquipmentModuleDescriptor* ItemObj = Item.GetDefaultObject();

		// if this is a Unique Use Module and we have one of this kind Accounted for already we skip it
		if (ItemObj->GetnUniqueUsage(Item))
			if (UniquesActive.Contains(Item))
			{

				UE_LOG(PowerSuit_Log, Warning, TEXT("Ignored Item %s because of an Already active Unique Item"), *ItemObj->mDisplayName.ToString())
				StatObject = FEquipmentStats();
				StatObject.mCachedInventorySlot = Ind;
				return StatObject;
			}
			else
				UniquesActive.Add(Item);

		APowerSuitModuleAttachment* Equipment = CreateAttachmentStateIfNeeded(Stack, ItemObj, Item, Ind);

		
		if (Stack.Item.HasState())
		{
			Equipment = Cast< APowerSuitModuleAttachment>(Stack.Item.ItemState.Get());
			if (Equipment)
			{
				Equipment->ParentModule = Parent;
				if (Equipment->GetIsConditionMet())
				{
					StatObject = Equipment->ReceiveModuleStats(ItemObj->EquipmentStats);
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
			if (ItemObj->GetnAttachment(Item))
			{
				if (!Parent->EquipmentParent->HasAuthority())
				{

					UE_LOG(PowerSuit_Log, Error, TEXT("Remote has not ItemState, Debug me"));
				}
				else
				{
					// ItemState isnt directly aviable even on Server apparantly
					if (Equipment)
					{
						Equipment->ParentModule = Parent;
						if (Equipment->GetIsConditionMet())
						{
							StatObject = Equipment->ReceiveModuleStats(ItemObj->EquipmentStats);
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
			}
			else
			{
				
				StatObject = ItemObj->EquipmentStats;

			}
		}
		StatObject.mCachedInventorySlot = Ind;
		return StatObject;
	}
	StatObject = FEquipmentStats();
	StatObject.mCachedInventorySlot = Ind;
	return StatObject;
}

APowerSuitModuleAttachment* UEMC_InventoryModule::CreateAttachmentStateIfNeeded(const FInventoryStack Stack, const UEquipmentModuleDescriptor* ItemObj, const TSubclassOf< class UEquipmentModuleDescriptor> Item, const int32 Index)
{
	if (ItemObj->GetnAttachment(Item) && !Stack.Item.HasState())
	{
		if (Parent->EquipmentParent && Parent->EquipmentParent->HasAuthority())
		{
			FVector  Loc = FVector(0, 0, 0);
			FRotator Rot = FRotator(0, 0, 0);
			FActorSpawnParameters SpawnSettings = FActorSpawnParameters();
			SpawnSettings.bNoFail = true;
			SpawnSettings.Owner = Parent->EquipmentParent->GetOwner();
			SpawnSettings.Instigator = Parent->EquipmentParent->GetInstigator();
			APowerSuitModuleAttachment* Equipment = Cast< APowerSuitModuleAttachment>(Parent->EquipmentParent->GetWorld()->SpawnActor(ItemObj->GetnAttachment(Item), &Loc, &Rot, SpawnSettings));
			check(Equipment);
			Equipment->SetReplicates(true);
			Equipment->ParentModule = Parent;
			Parent->nInventory->SetStateOnIndex(Index, FSharedInventoryStatePtr::MakeShared(Equipment));
			UE_LOG(PowerSuit_Log, Display, TEXT("Created Module Descriptor Attachment Actor %s"), *Equipment->GetName());
			return Equipment;
		}
	}
	return nullptr;
}
