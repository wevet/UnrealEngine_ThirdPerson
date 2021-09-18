// Copyright 2018 wevet works All Rights Reserved.

#include "Player/MockPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"


AMockPlayerController::AMockPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
	UMGManager(nullptr)
{

	static ConstructorHelpers::FObjectFinder<UClass> FindAsset(Wevet::ProjectFile::GetUMGPath());
	UMGManagerTemplate = FindAsset.Object;

}


void AMockPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Character = nullptr;
	Manager = nullptr;
	Super::EndPlay(EndPlayReason);
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

	if (ICombatInstigator* Interface = Cast<ICombatInstigator>(Character))
	{
		// DeathEventBind..
		{
			FCombatDelegate* Delegate = Interface->GetDeathDelegate();
			if (Delegate)
			{
				(*Delegate).AddDynamic(this, &AMockPlayerController::OnDeath);
			}
		}


		// KillEventBind..
		{
			FCombatOneDelegate* Delegate = Interface->GetKillDelegate();
			if (Delegate)
			{
				(*Delegate).AddDynamic(this, &AMockPlayerController::OnKill);
			}
		}


		// AliveEventBind..
		{
			FCombatDelegate* Delegate = Interface->GetAliveDelegate();
			if (Delegate)
			{
				(*Delegate).AddDynamic(this, &AMockPlayerController::OnAlive);
			}
		}
	}

	if (Manager)
	{
		Manager->OnPossess(InPawn);
	}
}


void AMockPlayerController::OnUnPossess()
{
	if (ICombatInstigator* Interface = Cast<ICombatInstigator>(Character))
	{
		// DeathEventUnBind..
		{
			FCombatDelegate* Delegate = Interface->GetDeathDelegate();
			if (Delegate)
			{
				(*Delegate).RemoveDynamic(this, &AMockPlayerController::OnDeath);
			}
		}


		// KillEventUnBind..
		{
			FCombatOneDelegate* Delegate = Interface->GetKillDelegate();
			if (Delegate)
			{
				(*Delegate).RemoveDynamic(this, &AMockPlayerController::OnKill);
			}
		}


		// AliveEventUnBind..
		{
			FCombatDelegate* Delegate = Interface->GetAliveDelegate();
			if (Delegate)
			{
				(*Delegate).RemoveDynamic(this, &AMockPlayerController::OnAlive);
			}
		}
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

