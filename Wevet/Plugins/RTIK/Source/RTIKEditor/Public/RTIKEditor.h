#pragma once
 
#include "Engine.h"
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"
#include "UnrealEd.h"
 
DECLARE_LOG_CATEGORY_EXTERN(LogRTIKEditor, All, All)
 
/**
 * The public interface to this module
 */
class IRTIKEditorPlugin : public IModuleInterface
{

public:

	static inline IRTIKEditorPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked<IRTIKEditorPlugin>("RTIKEditorPlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("RTIKEditorPlugin");
	}
};

