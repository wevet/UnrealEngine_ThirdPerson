// Copyright Epic Games, Inc. All Rights Reserved.

#include "MaterialFinder.h"

DEFINE_LOG_CATEGORY(LogMaterialFinder)


class FMaterialFinderPlugin : public IMaterialFinderPlugin
{
	virtual void StartupModule() override
	{
		UE_LOG(LogMaterialFinder, Log, TEXT("MaterialFinder Plugin : StartupModule"));
	}

	virtual bool IsGameModule() const override
	{
		return true;
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogMaterialFinder, Log, TEXT("MaterialFinder Plugin : ShutdownModule"));
	}
};

IMPLEMENT_MODULE(FMaterialFinderPlugin, MaterialFinder)
