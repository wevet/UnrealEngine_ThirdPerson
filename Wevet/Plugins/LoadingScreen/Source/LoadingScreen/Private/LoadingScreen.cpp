// Copyright 2018 wevet works All Rights Reserved.

#include "LoadingScreen.h"
#include "LoadingScreenWidget.h"
#include "MoviePlayer.h"

#define LOCTEXT_NAMESPACE "FLoadingScreenModule"


class FLoadingScreenModule : public ILoadingScreenModule
{
public:
	virtual void StartupModule() override
	{
		LoadObject<UObject>(nullptr, TEXT("/Game/UI/T_ActionRPG_TransparentLogo.T_ActionRPG_TransparentLogo") );

		if (IsMoviePlayerEnabled())
		{
			CreateScreen();
		}
	}
	
	virtual bool IsGameModule() const override
	{
		return true;
	}

	virtual void StartInGameLoadingScreen(bool bPlayUntilStopped, float PlayTime) override
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = !bPlayUntilStopped;
		LoadingScreen.bWaitForManualStop = bPlayUntilStopped;
		LoadingScreen.bAllowEngineTick = bPlayUntilStopped;
		LoadingScreen.MinimumLoadingScreenDisplayTime = PlayTime;
		LoadingScreen.WidgetLoadingScreen = SNew(SLoadingScreenWidget);
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}

	virtual void StopInGameLoadingScreen() override
	{
		GetMoviePlayer()->StopMovie();
	}

	virtual void CreateScreen()
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
		LoadingScreen.MinimumLoadingScreenDisplayTime = 3.f;
		LoadingScreen.WidgetLoadingScreen = SNew(SLoadingScreenWidget);
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}

};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FLoadingScreenModule, LoadingScreen);
