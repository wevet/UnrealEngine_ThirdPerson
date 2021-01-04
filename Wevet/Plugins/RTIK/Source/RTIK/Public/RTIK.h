#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRTIK, All, All)
 
/**
 * The public interface to this module
 */
class IRTIKPlugin : public IModuleInterface
{

public:

	static inline IRTIKPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked<IRTIKPlugin>("RTIKPlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("RTIKPlugin");
	}
};
