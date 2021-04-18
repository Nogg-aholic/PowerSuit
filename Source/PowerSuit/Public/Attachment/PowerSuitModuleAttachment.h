#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../EquipmentModuleComponent.h"
#include "PowerSuitModuleAttachment.generated.h"

class UUserWidget;

UCLASS()
class POWERSUIT_API APowerSuitModuleAttachment : public AActor, public IFGSaveInterface
{
	GENERATED_BODY()

public:	
	APowerSuitModuleAttachment();



	void PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion);


	void PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion);


	void PreLoadGame_Implementation(int32 saveVersion, int32 gameVersion);


	void PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion);


	void GatherDependencies_Implementation(TArray< UObject* >& out_dependentObjects);


	bool ShouldSave_Implementation() const;


	bool NeedTransform_Implementation();

	// This is called before an Attachment is Installed and gives the Opportunity
	// to overwrite Default assigned Stats before they are accounted for
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		FEquipmentStats RecieveModuleStats(FEquipmentStats DefaultStats);
	// This will Attach the Attachment to the Suit, overwrite to do something else
	UFUNCTION(BlueprintNativeEvent)
	void AttachToSuit(AFGEquipment * EquipmentParent);
	// Detaching from Suit
	UFUNCTION(BlueprintNativeEvent)
	void DetachFromSuit();

	// Called after Stats are accounted for 
	UFUNCTION(BlueprintNativeEvent)
	void AttachmentInstalled(FInventoryItem Item);
	// Called when the Suit Resets Stats, the Inventory Changed or when this Module was Unequipped
	// We dont really know which one it is
	// If Install follows it was a Refresh if not , the item is no longer in the Suit Inventory
	UFUNCTION(BlueprintNativeEvent)
	void AttachmentUnInstalled();


	// How much fuel in MW do we consume this Frame can also be negative for production
	UFUNCTION(BlueprintNativeEvent)
	float GetDeltaFuelConsumption(float DeltaTime) const;

	// How much Power in MW do we consume this Frame? can also be negative for production
	UFUNCTION(BlueprintNativeEvent)
	float GetDeltaPowerConsumption(float DeltaTime) const;

	// Should this Module trigger the Suits Active State ? ( Additional Power Consumption/Production ) 
	UFUNCTION(BlueprintNativeEvent)
	bool GetShouldTriggerActive()const;

	// Are all Requirements met for this to be still Accounted for ?
	// If this is false and the Module is currently installed, a Refresh is triggered and this module
	// will become Inactive untill the Condition changes to True
	// then the Refresh is triggered again , Installing this Module
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool GetIsConditionMet()const;

	// This Tick is before the Suit State has been processed this Frame, you stil have the Chance here to influrence how the Result will be
	UFUNCTION(BlueprintNativeEvent)
	void AttachmentPreTick(float DeltaTime);

	// The Suits State was assigned and we have calculated this Frame Power and Fuel Consumption
	// Last chance to do something before Power and Fuel values are changed this Frame
	UFUNCTION(BlueprintNativeEvent)
	void AttachmentTick(float DeltaTime);
	
	// The Suits State was assigned and we have calculated this Frame Power and Fuel Consumption
	// Last chance to do something before Power and Fuel values are changed this Frame
	UFUNCTION(BlueprintNativeEvent)
	void AttachmentPostTick(float DeltaTime);


	UFUNCTION(BlueprintNativeEvent)
		void ServerSetFloatValue(float Value, uint8 Index);

	
	UFUNCTION(BlueprintNativeEvent)
		void ServerSetBoolValue(bool Value, uint8 Index);


	UPROPERTY(BlueprintReadOnly)
	UEquipmentModuleComponent * Parent;


	UPROPERTY(BlueprintReadOnly)
		int32 InventorySlot = -1;
	// Cached Stat values
	// Changes here wont have any direct Effect
	// Refresh needs to be Triggered and RecieveModuleStats needs to be overwritten for this to be changed at Runtime
	// dont do this frequently since Refreshing causes alot of overhead
	UPROPERTY(BlueprintReadWrite)
	FEquipmentStats AttachmentStats;


	/** Custom Widget - placed at the Bottom of the Suit Widget*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<UAttachmentModuleUserWidget> CustomWidget;

	UPROPERTY(BlueprintReadOnly)
		bool bIsActive = false;


	// NOT REPLICATED ! see SetProperty for Instructions;
	UFUNCTION(BlueprintCallable)
		void SetSuitMovementProperty(ESuitMovementProperty Enum, FModMultProperty Property);
	// NOT REPLICATED ! see SetProperty for Instructions;
	UFUNCTION(BlueprintCallable)
		void SetSuitFlightProperty(ESuitFlightProperty Enum, FModMultProperty Property);
	// NOT REPLICATED!  use this on the Server and Replicate changes to Clients by replicating for example the source variables and make functions deterministic
	// or by replicating result values. If this is not important on the Client you may not Replicate this;
	UFUNCTION(BlueprintCallable)
		void SetSuitProperty(ESuitProperty Enum, FModMultProperty Property);
	// NOT REPLICATED ! see SetProperty for Instructions;
	UFUNCTION(BlueprintCallable)
		void SetPropertyGeneral(EEquipmentStatsProperty PropertyType, uint8 Index, FModMultProperty Property);
	// NOT REPLICATED ! see SetProperty for Instructions;
	UFUNCTION(BlueprintCallable)
		void SetSuitFlag(ESuitFlag Flag, bool Enabled);
};
