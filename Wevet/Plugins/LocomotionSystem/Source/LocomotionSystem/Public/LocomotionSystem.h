#pragma once

#include "ModuleInterface.h"
#include "Modules/ModuleManager.h"

/**
 * The public interface to this module
 */
class ILocomotionSystemPlugin : public IModuleInterface
{

public:

	static inline ILocomotionSystemPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked<ILocomotionSystemPlugin>("LocomotionSystemPlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded( "LocomotionSystemPlugin" );
	}
};

