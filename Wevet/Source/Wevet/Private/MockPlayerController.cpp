// Fill out your copyright notice in the Description page of Project Settings.

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
			this->UMGManager->AddToViewport();
			this->UMGManager->BuildInitialize(this->CharacterOwner);
		}

	}
}

