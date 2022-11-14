#include "SubModules/EMC_HealthModule.h"
#include "FGCharacterPlayer.h"
#include "SubModules/EMC_PowerModule.h"
#include "Equipment/Equipment_PowerSuit.h"

void UEMC_HealthModule::PreTick()
{
	
}


void UEMC_HealthModule::Tick()
{

}
// Server Only
void UEMC_HealthModule::PostTick()
{
	if (!Parent->EquipmentParent->HasAuthority())
	{
		return;
	}
	class UFGHealthComponent* HealthComponent = Cast< AFGCharacterPlayer>(Parent->EquipmentParent->GetInstigator())->GetHealthComponent();
	if (HealthComponent)
	{
		if (HealthComponent->GetCurrentHealth() < HealthComponent->GetMaxHealth())
			if (HealthBuffer > 1.f)
			{
				HealthComponent->Heal(HealthBuffer);
				HealthBuffer = 0.f;
			}
			else
				HealthBuffer = HealthBuffer + (Parent->GetSuitPropertySafe(ESuitProperty::nHealthRegen).value() * Parent->LastDeltaTime);
	}
}
// Server Only
void UEMC_HealthModule::SetMaxHealth() const
{
	if (!Cast< AFGCharacterPlayer>(Parent->EquipmentParent->GetInstigator()))
		return;

	if (Parent->EquipmentParent->HasAuthority() && Parent->EquipmentParent->GetInstigator())
	{
		class UFGHealthComponent* HealthComponent = Cast< AFGCharacterPlayer>(Parent->EquipmentParent->GetInstigator())->GetHealthComponent();
		if (HealthComponent)
		{
			// fine? - this is kinda bad but the max health is not really tracked after its changed :I so we reset to our Default Max
			HealthComponent->mMaxHealth = GetNewMaxHealth();
		}
	}
}

float UEMC_HealthModule::GetNewMaxHealth() const
{
	return FMath::Clamp(100.f + Parent->GetSuitPropertySafe(ESuitProperty::nHealth).value(), 1.f, 1000000.f); 
}

