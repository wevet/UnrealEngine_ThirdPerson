// Copyright 2018 wevet works All Rights Reserved.

#include "Player/MockPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"


AMockPlayerController::AMockPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	UMGManager(nullptr)
{
	ViewPitchMin = -50.f;
	ViewPitchMax = 50.f;
}

void AMockPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerCameraManager* CameraManager = Wevet::ControllerExtension::GetCameraManager(this))
	{
		CameraManager->ViewPitchMin = ViewPitchMin;
		CameraManager->ViewPitchMax = ViewPitchMax;
	}
}

void AMockPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CharacterOwner = Cast<AMockCharacter>(InPawn);

	if (UMGManagerClass)
	{
		UMGManager = CreateWidget<UUMGManager>(this, UMGManagerClass);
	}

	if (UMGManager && CharacterOwner)
	{
		UMGManager->Initializer(CharacterOwner);
		UMGManager->AddToViewport();
	}

	if (CharacterOwner)
	{
		CharacterOwner->DeathDelegate.AddDynamic(this, &AMockPlayerController::OnDeath);
		CharacterOwner->AliveDelegate.AddDynamic(this, &AMockPlayerController::OnAlive);
		CharacterOwner->KillDelegate.AddDynamic(this, &AMockPlayerController::OnKill);
	}
}

void AMockPlayerController::OnUnPossess()
{
	if (CharacterOwner)
	{
		CharacterOwner->DeathDelegate.RemoveDynamic(this, &AMockPlayerController::OnDeath);
		CharacterOwner->AliveDelegate.RemoveDynamic(this, &AMockPlayerController::OnAlive);
		CharacterOwner->KillDelegate.RemoveDynamic(this, &AMockPlayerController::OnKill);
	}

	Super::OnUnPossess();
}

UUMGManager* AMockPlayerController::GetPlayerHUD() const
{
	return UMGManager;
}

void AMockPlayerController::OnDeath()
{
	UE_LOG(LogWevetClient, Log, TEXT("OnDeath => %s"), *FString(__FUNCTION__));
}

void AMockPlayerController::OnAlive()
{
	UE_LOG(LogWevetClient, Log, TEXT("OnAlive => %s"), *FString(__FUNCTION__));
}

void AMockPlayerController::OnKill(AActor* InActor)
{
	UE_LOG(LogWevetClient, Log, TEXT("OnKill => %s"), *FString(__FUNCTION__));
}

