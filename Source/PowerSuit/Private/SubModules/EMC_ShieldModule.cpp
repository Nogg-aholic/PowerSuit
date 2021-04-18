


#include "SubModules/EMC_ShieldModule.h"
#include "SubModules/EMC_PowerModule.h"


void UEMC_ShieldModule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEMC_ShieldModule, ShieldRegenCooldown);
	DOREPLIFETIME(UEMC_ShieldModule, CurrentShield);
	
}


// Block incoming damage with the shield and return however much could not be absorbed
float UEMC_ShieldModule::ApplyShieldDamage(float DmgIn)
{
	if (DmgIn <= 0.f)
		return 0.f;

	ShieldDmg = FDateTime::Now();
	Parent->PowerModule->GoActive();
	// If the shield would still has HP left after taking this hit ...
	if (CurrentShield - DmgIn > 0)
	{
		// ... take the damage.
		Parent->OnShieldDamageTaken.Broadcast(DmgIn - CurrentShield, ShieldDmg, true);
		CurrentShield = CurrentShield - DmgIn;
		return 0;
	}
	else
	{
		// ... take the damage and break the shield, returning the unabsorbed damage amount
		float dmgout = DmgIn - CurrentShield;
		Parent->OnShieldDamageTaken.Broadcast(CurrentShield, ShieldDmg, true);
		CurrentShield = 0;
		return dmgout;
	}
}

// Check if shield should regenerate given time since last damage
bool UEMC_ShieldModule::ShouldRegenShield()
{
	if (!Parent->EquipmentParent)
		return false;

	if (!Parent->PowerModule->Producing)
		return false;

	const FTimespan timeSinceShieldDmg = FDateTime::Now() - ShieldDmg;

	if (timeSinceShieldDmg.GetTotalSeconds() > GetShieldRechargeDelayDuration())
		if (CurrentShield < GetMaxShield())
			return true;

	return false;
}

// Handle shield regeneration per tick and mark shield active timer if needed
void UEMC_ShieldModule::RegenShield() {
	CurrentShield = FMath::Clamp(CurrentShield + (GetShieldRechargeRate() * Parent->Delta), 0.f, GetMaxShield());
	// Regenerating Shield causes the Suit to Go Active ( Consume additional Power )
	Parent->PowerModule->GoActive();
}


void UEMC_ShieldModule::PreTick()
{
}

void UEMC_ShieldModule::Tick()
{
}

void  UEMC_ShieldModule::PostTick()
{
	if (Parent->PowerModule->Producing)
	{

		// Shield handling
		// shield is dependend on Power so we handle it based on the State of last frames Powerstate
		if (ShouldRegenShield())
		{
			// regenerating shield
			RegenShield();
			// when the shield is  being regenerated , its not fully broken anymore 
			ShieldRegenCooldown = false;
		}
		else if (CurrentShield == GetMaxShield()) // if not activley regenerating, check if done regenerating shield
			ShieldRegenCooldown = false;
		else if (GetMaxShield() != 0) // if not done regenerating and not actively regenerating, mark the shield as broken
			ShieldRegenCooldown = true;
		else  // otherwise shield is fine
			ShieldRegenCooldown = false;
	}
}