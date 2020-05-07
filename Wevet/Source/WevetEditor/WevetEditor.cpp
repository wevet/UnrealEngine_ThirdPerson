// Copyright 2018 wevet works All Rights Reserved.

#include "WevetEditor.h"

DEFINE_LOG_CATEGORY(LogWevetEditor)

class FWevetEditorPlugin : public IWevetEditorPlugin
{
	virtual void StartupModule() override
	{
		UE_LOG(LogWevetEditor, Log, TEXT("FWevetEditorPlugin : StartupModule"));
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogWevetEditor, Log, TEXT("FWevetEditorPlugin : ShutdownModule"));
	}
};

IMPLEMENT_GAME_MODULE(FWevetEditorPlugin, WevetEditor)
