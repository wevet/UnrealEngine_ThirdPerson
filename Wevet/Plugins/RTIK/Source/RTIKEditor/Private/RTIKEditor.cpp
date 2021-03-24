 #include "RTIKEditor.h"
 
DEFINE_LOG_CATEGORY(LogNIKEditor)
 
class FRTIKEditorPlugin : public IRTIKEditorPlugin
{
	virtual void StartupModule() override
	{
		UE_LOG(LogNIKEditor, Log, TEXT("FRTIKEditorPlugin : StartupModule"));
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogNIKEditor, Log, TEXT("FRTIKEditorPlugin : StartupModule"));
	}
};

IMPLEMENT_GAME_MODULE(FRTIKEditorPlugin, RTIKEditor)
