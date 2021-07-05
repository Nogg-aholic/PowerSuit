#include "SubModules/EMC_ZiplineModule.h"

#include "SubModules/EMC_StateModule.h"

void UEMC_ZiplineModule::PreTick()
{
	if (Parent->SuitState == EPowerSuitState::PS_ZIPLINE_SPRINT)
	{
		auto& Ref = Parent->MoveC->GetZiplineDataRef();
		if (Ref.SpeedMultiplier < 2.f && Parent->StateModule->HKey_Accel)
			Ref.SpeedMultiplier = 2.f;
		else if(Ref.SpeedMultiplier >= 2.f)
			Ref.SpeedMultiplier = 1.08f;
	}
}

void UEMC_ZiplineModule::Tick()
{
}

void UEMC_ZiplineModule::PostTick()
{
}
