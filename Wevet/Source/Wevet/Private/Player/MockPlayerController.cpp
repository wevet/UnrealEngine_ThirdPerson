// Copyright 2018 wevet works All Rights Reserved.

#include "Player/MockPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"


AMockPlayerController::AMockPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
	UMGManager(nullptr)
{

	static ConstructorHelpers::FObjectFinder<UClass> FindAsset(TEXT("/Game/Game/Blueprints/Widgets/BP_UMGManager.BP_UMGManager_C"));
	UMGManagerTemplate = FindAsset.Object;

}

void AMockPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMockPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Character = Cast<AMockCharacter>(InPawn);
	UMGManager = CreateWidget<UUMGManager>(this, UMGManagerTemplate);
	Manager = Cast<AMockPlayerCameraManager>(PlayerCameraManager);

	if (UMGManager && Character)
	{
		UMGManager->Initializer(Character);
		UMGManager->AddToViewport();
	}

	if (Character)
	{
		Character->DeathDelegate.AddDynamic(this, &AMockPlayerController::OnDeath);
		Character->AliveDelegate.AddDynamic(this, &AMockPlayerController::OnAlive);
		Character->KillDelegate.AddDynamic(this, &AMockPlayerController::OnKill);
	}

	if (Manager)
	{
		Manager->OnPossess(InPawn);
	}
}

void AMockPlayerController::OnUnPossess()
{
	if (Character)
	{
		Character->DeathDelegate.RemoveDynamic(this, &AMockPlayerController::OnDeath);
		Character->AliveDelegate.RemoveDynamic(this, &AMockPlayerController::OnAlive);
		Character->KillDelegate.RemoveDynamic(this, &AMockPlayerController::OnKill);
	}

	if (UMGManager)
	{
		UMGManager->RemoveFromParent();
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
	UE_LOG(LogWevetClient, Log, TEXT("Player Killed.. Actor => %s"), *InActor->GetName());
}

