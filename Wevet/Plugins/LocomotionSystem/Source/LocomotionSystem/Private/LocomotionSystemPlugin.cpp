#include "ILocomotionSystemPlugin.h"

class FLocomotionSystemPlugin : public ILocomotionSystemPlugin
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FLocomotionSystemPlugin, LocomotionSystem)

void FLocomotionSystemPlugin::StartupModule()
{

}

void FLocomotionSystemPlugin::ShutdownModule()
{

}
