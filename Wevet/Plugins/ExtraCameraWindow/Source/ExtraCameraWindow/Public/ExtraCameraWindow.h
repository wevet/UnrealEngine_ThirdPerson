#pragma once

#include "CoreMinimal.h"
#include "ModuleInterface.h"
#include "Modules/ModuleManager.h"

#include "Engine.h"
#include "Slate/SGameLayerManager.h"
#include "Slate/SceneViewport.h"


/**
 * The public interface to this module
 */
class IExtraCameraWindowModule : public IModuleInterface
{

public:

	static inline IExtraCameraWindowModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IExtraCameraWindowModule>("ExtraCameraWindow");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded( "ExtraCameraWindow" );
	}
};

