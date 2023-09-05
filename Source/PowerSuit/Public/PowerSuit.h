// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

#define FOR_MODSHIPPING 1

DECLARE_LOG_CATEGORY_EXTERN(LogPowerSuitCpp, Verbose, All);

// Verbose for testing, Warning for release
DECLARE_LOG_CATEGORY_EXTERN(LogPowerSuit_Input, Warning, All);

class FPowerSuitModule : public FDefaultGameModuleImpl
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
