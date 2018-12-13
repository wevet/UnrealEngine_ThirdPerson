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

	if (AMockCharacter* Character = Cast<AMockCharacter>(Pawn))
	{
		CharacterOwner = Character;
		check(CharacterOwner);
		Initializer();
	}
}

void AMockPlayerController::Initializer()
{
	if (UMGManagerClass == nullptr)
	{
		return;
	}

	UMGManager = CreateWidget<UUMGManager>(this, UMGManagerClass);
	if (UMGManager)
	{
		UMGManager->Initializer(CharacterOwner);
		UMGManager->AddToViewport();
	}
}

UUMGManager* AMockPlayerController::GetPlayerHUD() const
{
	return UMGManager;
}
