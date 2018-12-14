// Copyright 2018 wevet works All Rights Reserved.

#include "WevetGameModeBase.h"
#include "MockPlayerController.h"
#include "Engine.h"

AWevetGameModeBase::AWevetGameModeBase()
{
	//
}

AWevetGameModeBase::AWevetGameModeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//
}

void AWevetGameModeBase::InitGameState()
{
	Super::InitGameState();
}

void AWevetGameModeBase::StartPlay()
{
	Super::StartPlay();

	UE_LOG(LogWevetClient, Log, TEXT("StartPlay"));
}
