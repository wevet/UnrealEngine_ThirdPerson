#include "LocomotionSystem.h"

class FLocomotionSystemPlugin : public ILocomotionSystemPlugin
{
	virtual void StartupModule() override
	{

	}

	virtual bool IsGameModule() const override
	{
		return true;
	}

	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_GAME_MODULE(FLocomotionSystemPlugin, LocomotionSystem)
