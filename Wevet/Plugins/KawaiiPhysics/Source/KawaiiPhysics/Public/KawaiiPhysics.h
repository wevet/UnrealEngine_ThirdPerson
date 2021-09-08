#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogKawaiiPhysics, All, All)


class IKawaiiPhysicsPlugin : public IModuleInterface
{

public:

	static inline IKawaiiPhysicsPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked<IKawaiiPhysicsPlugin>("KawaiiPhysics");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("KawaiiPhysics");
	}
};

