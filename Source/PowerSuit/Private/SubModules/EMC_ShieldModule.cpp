


#include "SubModules/EMC_ShieldModule.h"
#include "SubModules/EMC_PowerModule.h"


void UEMC_ShieldModule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
}


// Block incoming damage with the shield and return however much could not be absorbed
float UEMC_ShieldModule::ApplyShieldDamage(const float DmgIn,FHitResult Impact) const
{
	if (DmgIn <= 0.f)
		return 0.f;

	Parent->nShieldDmg = FDateTime::Now();
	Parent->Client_ShieldDmg();
	// If the shield would still has HP left after taking this hit ...
	if (Parent->nCurrentShield - DmgIn > 0)
	{
		// ... take the damage.
		Parent->OnShieldDamageTaken.Broadcast(DmgIn - Parent->nCurrentShield, Parent->nShieldDmg, true,Impact);
		Parent->nCurrentShield = Parent->nCurrentShield - DmgIn;
		return 0;
	}
	else
	{
		// ... take the damage and break the shield, returning the unabsorbed damage amount
		const float DmgOut = DmgIn - Parent->nCurrentShield;
		Parent->OnShieldDamageTaken.Broadcast(Parent->nCurrentShield, Parent->nShieldDmg, true,Impact);
		Parent->nCurrentShield = 0;
		return DmgOut;
	}
}

// Check if shield should regenerate given time since last damage
bool UEMC_ShieldModule::ShouldRegenShield() const
{
	if (!Parent->EquipmentParent)
		return false;

	if (!Parent->nProducing)
		return false;

	const FTimespan timeSinceShieldDmg = FDateTime::Now() - Parent->nShieldDmg;

	if (timeSinceShieldDmg.GetTotalSeconds() > GetShieldRechargeDelayDuration())
		if (Parent->nCurrentShield < GetMaxShield())
			return true;

	return false;
}

// Handle shield regeneration per tick and mark shield active timer if needed
void UEMC_ShieldModule::RegenShield() const
{
	Parent->nCurrentShield = FMath::Clamp(Parent->nCurrentShield + (GetShieldRechargeRate() * Parent->LastDeltaTime), 0.f, GetMaxShield());
	// Regenerating Shield causes the Suit to Go Active ( Consume additional Power )
}


void UEMC_ShieldModule::PreTick()
{
}

void UEMC_ShieldModule::Tick()
{
}

void  UEMC_ShieldModule::PostTick() const
{
	if (Parent->nProducing)
	{

		// Shield handling
		// shield is dependend on Power so we handle it based on the State of last frames Powerstate
		if (ShouldRegenShield())
		{
			// regenerating shield
			RegenShield();
			// when the shield is  being regenerated , its not fully broken anymore 
			Parent->nShieldRegenCooldown = false;
		}
		else if (Parent->nCurrentShield == GetMaxShield()) // if not activley regenerating, check if done regenerating shield
			Parent->nShieldRegenCooldown = false;
		else if (GetMaxShield() != 0) // if not done regenerating and not actively regenerating, mark the shield as broken
			Parent->nShieldRegenCooldown = true;
		else  // otherwise shield is fine
			Parent->nShieldRegenCooldown = false;
	}
}
