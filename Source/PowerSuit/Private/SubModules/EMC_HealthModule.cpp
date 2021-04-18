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
	class UFGHealthComponent* heatlhcomp = Cast< AFGCharacterPlayer>(Parent->EquipmentParent->GetInstigator())->GetHealthComponent();
	if (heatlhcomp)
	{
		if (heatlhcomp->GetCurrentHealth() < heatlhcomp->GetMaxHealth())
			if (HealthBuffer > 1.f)
			{
				heatlhcomp->Heal(HealthBuffer);
				HealthBuffer = 0.f;
			}
			else
				HealthBuffer = HealthBuffer + (Parent->Stats.nSuitProperties[ESuitProperty::nHealthRegen].value() * Parent->Delta);
	}
}
// Server Only
void UEMC_HealthModule::SetMaxHealth()
{
	if (!Cast< AFGCharacterPlayer>(Parent->EquipmentParent->GetInstigator()))
		return;

	if (Parent->EquipmentParent->HasAuthority() && Parent->EquipmentParent->GetInstigator())
	{
		class UFGHealthComponent* heatlhcomp = Cast< AFGCharacterPlayer>(Parent->EquipmentParent->GetInstigator())->GetHealthComponent();
		if (heatlhcomp)
		{
			// fine? - this is kinda bad but the max health is not really tracked after its changed :I so we reset to our Default Max
			heatlhcomp->mMaxHealth = GetNewMaxHealth();
		}
	}
}

float UEMC_HealthModule::GetNewMaxHealth()
{
	return FMath::Clamp(100.f + Parent->Stats.nSuitProperties[ESuitProperty::nHealth].value(), 1.f, 1000000.f); 
}

