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
	void PostTick() const;
public:

	void RegenShield() const;

	bool ShouldRegenShield() const;


	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetShieldRechargeRate() const { return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nShieldRegen).value(), 0.f, 10000.f); };
	// Max Shield is Mod *Mult 
	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetMaxShield() const { if (Parent->nProducing) return (Parent->GetSuitPropertySafe(ESuitProperty::nShield).value()); else return 0; };


	UFUNCTION(BlueprintPure, Category = "EquipmentModule")
		float GetShieldRechargeDelayDuration() const { return FMath::Clamp(Parent->GetSuitPropertySafe(ESuitProperty::nShieldRechargeDelay).value(), 0.f, 10000.f); };


	float ApplyShieldDamage(const float DmgIn, FHitResult Impact) const;


};
