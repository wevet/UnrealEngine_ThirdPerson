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

void AMockPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AMockCharacter* OwnerPawn = Cast<AMockCharacter>(InPawn))
	{
		CharacterOwner = OwnerPawn;
		check(CharacterOwner);
		Initializer();
	}
}

void AMockPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
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
