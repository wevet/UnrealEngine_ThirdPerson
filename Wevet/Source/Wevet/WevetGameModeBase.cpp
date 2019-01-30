// Copyright 2018 wevet works All Rights Reserved.

#include "WevetGameModeBase.h"
#include "MockPlayerController.h"
#include "Engine.h"


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

	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ACharacterBase::StaticClass(), FoundActors);

	for (AActor*& Actor : FoundActors)
	{
		if (ACharacterBase* Character = Cast<ACharacterBase>(Actor))
		{
			CharacterArray.Add(Character);
		}
	}

	UE_LOG(LogWevetClient, Warning, TEXT("StartPlay \n CharacterNum : [%d] \n funcName : [%s]"), 
		CharacterArray.Num(), 
		*FString(__FUNCTION__));
}
