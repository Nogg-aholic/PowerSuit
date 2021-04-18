#pragma once
#include "SubModules/EMC_SubModule.h"
#include "SubModules/EMC_PowerModule.h"

#include "EMC_ShieldModule.generated.h"

/**
 * 
 */
UCLASS()
class POWERSUIT_API UEMC_ShieldModule : public UEMC_SubModule
{
	GENERATED_BODY()


	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
	friend class UEquipmentModuleComponent;


	void PreTick();
	void Tick();
	void PostTick();
public:

	void RegenShield();

	bool ShouldRegenShield();


	// Took ShieldDamage within the DelayTimer ran out"
	UPROPERTY(BlueprintReadonly, Replicated, Category = "EquipmentModule")
		bool ShieldRegenCooldown;

	// Amount of Shield Total
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule")
		float CurrentShield;

	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetShieldRechargeRate() { return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nShieldRegen).value(), 0.f, 10000.f); };
	// Max Shield is Mod *Mult 
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetMaxShield() { if (Parent->PowerModule->Producing) return (Parent->GetSuitPropertySafe(ESuitProperty::nShield).value()); else return 0; };


	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetShieldRechargeDelayDuration() { return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nShieldRechargeDelay).value(), 0.f, 10000.f); };

	// Timer for when we took Shield Damage"
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "EquipmentModule | Components")
		FDateTime ShieldDmg;

	float ApplyShieldDamage(float DmgIn);


};
