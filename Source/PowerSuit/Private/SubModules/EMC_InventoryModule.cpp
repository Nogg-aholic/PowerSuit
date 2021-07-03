#include "SubModules/EMC_InventoryModule.h"
#include "SubModules/EMC_HealthModule.h"
#include "SubModules/EMC_AttachmentModule.h"
#include "SubModules/EMC_FuelModule.h"

#include "FGInventoryLibrary.h"


#include "PowerSuitBPLibrary.h"
#include "Equipment/Equipment_PowerSuit.h"
AFGCharacterPlayer* UEMC_InventoryModule::InitInventory()
{
	if (!Parent->EquipmentParent->GetInstigator())
		return nullptr;


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
			Parent->nInventory = UFGInventoryLibrary::CreateInventoryComponent(Parent->EquipmentParent, FName("nInventory" + Parent->EquipmentParent->GetName()));
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

		Parent->nInventory->mItemFilter.BindUFunction(Parent,"VerifyItem");
		
		UpdateInventorySize();
	}
	return Character;
}


void UEMC_InventoryModule::UpdateInventorySize()
{
	if (!Parent->EquipmentParent->GetInstigator())
		return;

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
	if (!Parent->EquipmentParent->GetInstigator())
		return;

	UE_LOG(PowerSuit_Log, Display, TEXT("ResetInventoryStats"));

	ItemsRemembered.Empty();
	UniquesActive.Empty();
	Parent->FuelModule->nAllowedFuels.Empty();
	Parent->DefaultStats.nUnlockedAllowedFuels.Empty();
	TArray<FItemAmount> Arr;

	#ifdef FOR_MODSHIPPING
		Arr = Parent->EquipmentParent->mCostToUse;
	#else
		Arr = Parent->EquipmentParent->*get(steal_mCostToUse());
	#endif

	for (auto i : Arr)
	{
		Parent->FuelModule->nAllowedFuels.Add(i.ItemClass);
		Parent->DefaultStats.nUnlockedAllowedFuels.Add(i.ItemClass);
	}
}


void UEMC_InventoryModule::RefreshInventoryRemove(TSubclassOf<UFGItemDescriptor> ItemClass, int32 NumAdded)
{
	if (!Parent->EquipmentParent->GetInstigator())
		return;
	
	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;
	
	UE_LOG(PowerSuit_Log, Display, TEXT("RefreshInventoryRemove Called from Binding with Class %s and Amount %i"), *ItemClass->GetName(), NumAdded);
	
	TArray<TSubclassOf<UFGItemDescriptor>> RelevantClasses; RelevantClasses.Add(UEquipmentModuleDescriptor::StaticClass());
	TArray<int32> RelevantSlotIndexes = Parent->nInventory->GetRelevantStackIndexes(RelevantClasses);
	TArray<int32> CachedIndexes; ItemsRemembered.GetKeys(CachedIndexes);
	for (auto i : RelevantSlotIndexes)
	{
		if (CachedIndexes.Contains(i))
		{
			CachedIndexes.Remove(i);
		}
	}

	if (CachedIndexes.Num() == 1)
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("Found Item Removed to be %s"), *ItemsRemembered[CachedIndexes[0]].mCachedDescriptor->GetName());
		if (ItemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()))
		{
			if (ItemClass != ItemsRemembered[CachedIndexes[0]].mCachedDescriptor)
			{
				UE_LOG(PowerSuit_Log, Error, TEXT("Input ItemClass %s is not == to the Cached Item Class"), *ItemClass->GetName());
				const TSubclassOf<class UEquipmentModuleDescriptor> Item = ItemsRemembered[CachedIndexes[0]].mCachedDescriptor;
				SubtractModuleStats(Item, CachedIndexes[0]);
			}
			else
			{
				const TSubclassOf<class UEquipmentModuleDescriptor> Item = ItemClass;
				SubtractModuleStats(Item, CachedIndexes[0]);
			}
		}
		else
		{
			UE_LOG(PowerSuit_Log, Error, TEXT("No Valid Index Found or Item is not ModuleDescriptor"));
			RefreshInventory();
		}
	}
	else if (CachedIndexes.Num() > 1)
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("More Items in Cache than in Inventory. Doing Full Update"));
		RefreshInventory();
	}
	else
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("Slots did not change ?! Direct Replacement ?! Doing Nothing !!"));
	}

	UPowerSuitBPLibrary::UpdateAllNoRefresh(Parent->EquipmentParent);

	if (Parent->EquipmentParent->HasAuthority() && !Parent->EquipmentParent->GetInstigator()->IsLocallyControlled())
	{
		Parent->RemoteInventoryRefresh(false, ItemClass, NumAdded);
		UE_LOG(PowerSuit_Log, Display, TEXT("Calling Remote with Refresh Remove"));
	}
	
	
	TimerDel.BindUFunction(this, FName("RefreshInventoryRemove_Latent"), ItemClass, NumAdded);

	Parent->EquipmentParent->GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, .1f, false);
}

void UEMC_InventoryModule::RefreshInventoryRemove_Latent(TSubclassOf<UFGItemDescriptor> ItemClass, int32 NumAdded)
{
	if (!Parent->EquipmentParent->GetInstigator())
		return;

	UE_LOG(PowerSuit_Log, Display, TEXT("RefreshInventoryRemove_Latent Called from Timer with Class %s and Amount %i"), *ItemClass->GetName(), NumAdded);

	
}



void UEMC_InventoryModule::RefreshInventory()
{
	if (!Parent->EquipmentParent->GetInstigator())
		return;
	UE_LOG(PowerSuit_Log, Display, TEXT("RefreshInventory"));


	if (Parent->EquipmentParent && Parent->nInventory)
		BulkUpdateStats(Parent->nInventory);
}




void UEMC_InventoryModule::RefreshInventoryAdd(TSubclassOf<UFGItemDescriptor> ItemClass, int32 NumAdded)
{

	if (!Parent->EquipmentParent->GetInstigator())
		return; 

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;
	UE_LOG(PowerSuit_Log, Display, TEXT("RefreshInventoryAdd Called from Binding with Class %s and Amount %i"), *ItemClass->GetName(), NumAdded);

	int32 NewIndex = -1; int32 ItemsLeft = NumAdded;
	TArray<TSubclassOf<UFGItemDescriptor>> RelevantClasses; RelevantClasses.Add(UEquipmentModuleDescriptor::StaticClass());
	TArray<int32> RelevantSlotIndexes = Parent->nInventory->GetRelevantStackIndexes(RelevantClasses);

	for (auto i : ItemsRemembered)
	{
		if (RelevantSlotIndexes.Contains(i.Key))
		{
			RelevantSlotIndexes.Remove(i.Key);
		}
	}

	if (RelevantSlotIndexes.Num() == 0)
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("No New Item !?"));
		//RefreshInventory();

	}
	else if (RelevantSlotIndexes.Num() == 1)
	{
		if (!MergeOnIndex(RelevantSlotIndexes[0]))
		{
			UE_LOG(PowerSuit_Log, Error, TEXT("Logic failed to resolve Stack index doing Re-Evaluation"));
			RefreshInventory();
			return;
		}
	}
	else
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("More than 1 Relevant Indexes missing in Cache, Updating All"));
		RefreshInventory();
		return;
	}

	UPowerSuitBPLibrary::UpdateAllNoRefresh(Parent->EquipmentParent);
	if (Parent->EquipmentParent->GetInstigator()->HasAuthority() && !Parent->EquipmentParent->GetInstigator()->IsLocallyControlled())
	{
		Parent->RemoteInventoryRefresh(true, ItemClass, NumAdded);
	}


	TimerDel.BindUFunction(this, FName("RefreshInventoryAdd_Latent"), ItemClass, NumAdded);

	Parent->EquipmentParent->GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, .1f, false);
}


void UEMC_InventoryModule::RefreshInventoryAdd_Latent(TSubclassOf<UFGItemDescriptor> ItemClass, int32 NumAdded)
{

	if (!Parent->EquipmentParent->GetInstigator())
		return;

	UE_LOG(PowerSuit_Log, Display, TEXT("RefreshInventoryAdd_Latent Called from Timer with Class %s and Amount %i"), *ItemClass->GetName(), NumAdded);
}



void UEMC_InventoryModule::MergeStats(FInventoryStack Stack, FEquipmentStats & StatsRef)
{
	const TSubclassOf< class UEquipmentModuleDescriptor> Item = Stack.Item.ItemClass;
	const bool Unique = Item.GetDefaultObject()->GetnUniqueUsage(Item);
	if (!Stack.Item.ItemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()) || (Unique && UniquesActive.Contains(Item)))
	{
		return;
	}
	if (Stack.Item.HasState() || StatsRef.mCachedAttachment)
	{
		UniquesActive.Add(Item);

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
				UE_LOG(PowerSuit_Log, Display, TEXT("Merged Attachment Stats from : %s"), *StatsRef.mCachedAttachment->GetName());

				Parent->Stats + StatsRef;
				StatsRef.UnlockFuels(Parent, Item.GetDefaultObject()->nAllowedFuels);
			}
			else
			{
				if (Parent->AttachmentModule->Attachments.Contains(Attachment))
					Parent->AttachmentModule->Attachments.Remove(Attachment);

				if (!Parent->AttachmentModule->InactiveAttachments.Contains(Attachment))
					Parent->AttachmentModule->InactiveAttachments.Add(Attachment);
				
				
				Parent->Stats + StatsRef;
				StatsRef.UnlockFuels(Parent, Item.GetDefaultObject()->nAllowedFuels);
				
				UE_LOG(PowerSuit_Log, Display, TEXT("Skipped Merging Attachment Stats from : %s. Condition is not Met"), *StatsRef.mCachedAttachment->GetName());
			}
		}
	}
	else
	{
		UE_LOG(PowerSuit_Log, Display, TEXT("Merged Stats from : %s"), *Stack.Item.ItemClass->GetName());
		Parent->Stats + StatsRef;
		StatsRef.UnlockFuels(Parent, Item.GetDefaultObject()->nAllowedFuels);
		
	}

	ItemsRemembered.Add(StatsRef.mCachedInventorySlot, StatsRef);
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
			CheckCreateModuleStats(Stack, Ind);
			MergeStats(Stack, GetModuleStats(Stack, Ind));
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

	if (!ItemsRemembered.Contains(Index))
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("Item not Remembered ? Update full Inventory"));
		RefreshInventory();
		return true;
	}

	if (Parent->nInventory->IsSomethingOnIndex(Index))
	{
		FInventoryStack Stack;
		Parent->nInventory->GetStackFromIndex(Index, Stack);
		if (Stack.HasItems())
		{
			{

				FEquipmentStats& OldStatsRef = *ItemsRemembered.Find(Index);
				// found
				TSubclassOf< class UEquipmentModuleDescriptor> item = Stack.Item.ItemClass;

				SubtractModuleStats(item,Index);
				CheckCreateModuleStats(Stack, Index);
				MergeStats(Stack, GetModuleStats(Stack, Index));

				UPowerSuitBPLibrary::UpdateAllNoRefresh(Parent->EquipmentParent);
				return true;
				
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

	if (!ItemsRemembered.Contains(Index))
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("An Item was removed we didnt know was in there ?! "));
		RefreshInventory();
	}
	else
	{

		FEquipmentStats& i = *ItemsRemembered.Find(Index);
		int32 indexToRemove = 0;
		APowerSuitModuleAttachment* Equipment = nullptr;
		UE_LOG(PowerSuit_Log, Display, TEXT("Subtraced Stats from Item: %s"), *Item->GetName());

		if (UniquesActive.Contains(Item))
			UniquesActive.Remove(Item);
			
		Parent->Stats - i;
		i.ForgetUnlockedFuels(Parent);
		Equipment = i.mCachedAttachment;
		
		
		
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

		ItemsRemembered.Remove(Index);
	}
}


bool UEMC_InventoryModule::CheckCreateModuleStats(const FInventoryStack Stack, const int32 Ind)
{
	
	if (!Stack.HasItems() || !Stack.Item.ItemClass  || !Stack.Item.ItemClass->IsChildOf(UEquipmentModuleDescriptor::StaticClass()))
	{
		return false;
	};

	if (ItemsRemembered.Contains(Ind))
	{
		if (ItemsRemembered.Find(Ind)->mCachedDescriptor == Stack.Item.ItemClass)
		{
			return true;
		}
		else
		{
			UE_LOG(PowerSuit_Log, Error, TEXT("Mismatch ItemClass for Remembered Item: %s"), *ItemsRemembered.Find(Ind)->mCachedDescriptor->GetName())
			return false;
		}
	}
	else
	{
		const TSubclassOf< class UEquipmentModuleDescriptor> Item = Stack.Item.ItemClass;
		const UEquipmentModuleDescriptor* ItemObj = Item.GetDefaultObject();


		if (ItemObj->GetnAttachment(Item))
		{
			APowerSuitModuleAttachment* Equipment = nullptr;
			if (!Stack.Item.HasState())
			{
				if (Parent->EquipmentParent->HasAuthority())
				{
					Equipment = CreateAttachmentStateIfNeeded(Stack, ItemObj, Item, Ind);
				}
				else
				{
					UE_LOG(PowerSuit_Log, Error, TEXT("Remote should not End up here ! ItemState is not Replicated for %s"), *ItemObj->mDisplayName.ToString());
				}
			}
			else
			{
				Equipment = Cast< APowerSuitModuleAttachment>(Stack.Item.ItemState.Get());
			}

			if (Equipment)
			{
				Equipment->ParentModule = Parent;
				// If an attachment does not have its 'Condition met' it falls back to the module's default stats instead of those offered by ReceiveModuleStats
				// This allows, for example, a jetpack to disable flight but still offer a fuel tank size bonus while disabled.
				FEquipmentStats Obj = ItemObj->EquipmentStats; 
				Equipment->InventorySlot = Ind;

				if (Equipment->GetIsConditionMet())
				{
					Obj = Equipment->ReceiveModuleStats(ItemObj->EquipmentStats);
				}
				Obj.mCachedAttachment = Equipment;
				Obj.mCachedInventorySlot = Ind;
				Obj.mCachedDescriptor = Item;
				ItemsRemembered.Add(Ind, Obj);
			}
			else
			{
				UE_LOG(PowerSuit_Log, Error, TEXT("This should have an Attachment, but it's Invalid here. Falling back to DescriptorStats %s <--- FIX ME "), *ItemObj->mDisplayName.ToString());
				FEquipmentStats Obj = ItemObj->EquipmentStats;
				Obj.mCachedInventorySlot = Ind;
				Obj.mCachedDescriptor = Item;
				ItemsRemembered.Add(Ind, Obj);
			}
		}
		else
		{
			FEquipmentStats Obj = ItemObj->EquipmentStats;
			Obj.mCachedInventorySlot = Ind;
			Obj.mCachedDescriptor = Item;
			ItemsRemembered.Add(Ind, Obj);
		}
	}
	return true;

}


FEquipmentStats& UEMC_InventoryModule::GetModuleStats(const FInventoryStack Stack, const int32 Ind)
{
	if (ItemsRemembered.Contains(Ind))
	{
		if (ItemsRemembered.Find(Ind)->mCachedDescriptor == Stack.Item.ItemClass)
		{
			return *ItemsRemembered.Find(Ind);
		}
		else
		{
			UE_LOG(PowerSuit_Log, Error, TEXT("Mismatch ItemClass for Remembered Item: %s"), *ItemsRemembered.Find(Ind)->mCachedDescriptor ? *ItemsRemembered.Find(Ind)->mCachedDescriptor->GetName() : TEXT("nullpeter"))
		}
	}
	else
	{
		UE_LOG(PowerSuit_Log, Error, TEXT("No Stats Found for %s"), *ItemsRemembered.Find(Ind)->mCachedDescriptor->GetName())
		FEquipmentStats StatObject = FEquipmentStats();
		StatObject.mCachedInventorySlot = Ind;
		ItemsRemembered.Add(Ind,StatObject);
	}
	return ItemsRemembered[Ind];
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
