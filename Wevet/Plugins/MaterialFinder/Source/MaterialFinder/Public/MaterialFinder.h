// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMaterialFinder, All, All)


class IMaterialFinderPlugin : public IModuleInterface
{
public:
	static inline IMaterialFinderPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked<IMaterialFinderPlugin>("MaterialFinderPlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("MaterialFinderPlugin");
	}
};

