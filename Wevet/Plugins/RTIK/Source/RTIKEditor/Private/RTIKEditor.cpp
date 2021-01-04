 #include "RTIKEditor.h"
 
DEFINE_LOG_CATEGORY(LogRTIKEditor)
 
class FRTIKEditorPlugin : public IRTIKEditorPlugin
{
	virtual void StartupModule() override
	{
		UE_LOG(LogRTIKEditor, Log, TEXT("FRTIKEditorPlugin : StartupModule"));
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogRTIKEditor, Log, TEXT("FRTIKEditorPlugin : StartupModule"));
	}
};

IMPLEMENT_GAME_MODULE(FRTIKEditorPlugin, RTIKEditor)
