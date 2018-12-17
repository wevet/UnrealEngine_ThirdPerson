#include "IFullbodyIKEditorPlugin.h"

class FFullbodyIKEditorPlugin : public IFullbodyIKEditorPlugin
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FFullbodyIKEditorPlugin, FullbodyIKEditor)

void FFullbodyIKEditorPlugin::StartupModule()
{
}

void FFullbodyIKEditorPlugin::ShutdownModule()
{
}
