// Copyright 2018 wevet works All Rights Reserved.

#include "MockPlayerController.h"
#include "Engine.h"
#include "Blueprint/UserWidget.h"


AMockPlayerController::AMockPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	UMGManager(nullptr)
{
	//
}

void AMockPlayerController::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);

	AMockCharacter* Character = Cast<AMockCharacter>(Pawn);
	if (Character)
	{
		CharacterOwner = Character;
		check(CharacterOwner);
		Initialize();
	}
}

void AMockPlayerController::Initialize()
{
	if (UMGManagerClass)
	{
		UMGManager = CreateWidget<UUMGManager>(this, UMGManagerClass);

		if (UMGManager)
		{
			// call before event construct
			UMGManager->Init(CharacterOwner);
			UMGManager->AddToViewport();
		}

	}
}

