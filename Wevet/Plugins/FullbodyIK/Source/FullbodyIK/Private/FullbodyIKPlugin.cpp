#include "IFullbodyIKPlugin.h"

class FFullbodyIKPlugin : public IFullbodyIKPlugin
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FFullbodyIKPlugin, FullbodyIK)

void FFullbodyIKPlugin::StartupModule()
{
}

void FFullbodyIKPlugin::ShutdownModule()
{
}
