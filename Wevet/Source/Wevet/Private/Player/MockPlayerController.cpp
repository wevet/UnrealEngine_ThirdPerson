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
		this->CharacterOwner = Character;
		check(this->CharacterOwner);
		Initialize();
	}
}

void AMockPlayerController::Initialize()
{
	if (this->UMGManagerClass)
	{
		this->UMGManager = CreateWidget<UUMGManager>(this, this->UMGManagerClass);

		if (this->UMGManager)
		{
			// call before event construct
			this->UMGManager->BuildInitialize(this->CharacterOwner);
			this->UMGManager->AddToViewport();
		}

	}
}

