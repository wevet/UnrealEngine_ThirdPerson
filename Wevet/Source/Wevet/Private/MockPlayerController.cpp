// Fill out your copyright notice in the Description page of Project Settings.

#include "MockPlayerController.h"
#include "Engine.h"
#include "Blueprint/UserWidget.h"


AMockPlayerController::AMockPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//
}

void AMockPlayerController::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);

	AMockCharacter* MockCharacter = Cast<AMockCharacter>(Pawn);
	if (MockCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found"));
	}
}

void AMockPlayerController::BeginPlay()
{
	if (this->WidgetMainUI)
	{
		this->Widget = CreateWidget<UUserWidget>(this, this->WidgetMainUI);

		if (this->Widget)
		{
			this->Widget->AddToViewport();
		}

	}
}

