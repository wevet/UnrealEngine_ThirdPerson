

#include "IExtraCameraWindowPlugin.h"


class FExtraCameraWindowPlugin : public IExtraCameraWindowPlugin
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool IsGameModule() const override
	{
		return true;
	}
};

IMPLEMENT_MODULE(FExtraCameraWindowPlugin, ExtraCameraWindow)



void FExtraCameraWindowPlugin::StartupModule()
{
	
}


void FExtraCameraWindowPlugin::ShutdownModule()
{
	
}



