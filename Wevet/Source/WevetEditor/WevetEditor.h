// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "Engine.h"
#include "CoreMinimal.h"
#include "ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "LogMacros.h"
#include "UnrealEd.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWevetEditor, All, All)

/**
 * The public interface to this module
 */
class IWevetEditorPlugin : public IModuleInterface
{

public:

	static inline IWevetEditorPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked<IWevetEditorPlugin>("WevetEditorPlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("WevetEditorPlugin");
	}
};
