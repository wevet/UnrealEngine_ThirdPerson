#include "ExtraCameraWindow.h"

class FExtraCameraWindowPlugin : public IExtraCameraWindowModule
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
