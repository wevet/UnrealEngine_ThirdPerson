#include "IExtraCameraWindowPlugin.h"

class FExtraCameraWindowPlugin : public IExtraCameraWindowPlugin
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FExtraCameraWindowPlugin, ExtraCameraWindow)

void FExtraCameraWindowPlugin::StartupModule()
{
	
}

void FExtraCameraWindowPlugin::ShutdownModule()
{
	
}
