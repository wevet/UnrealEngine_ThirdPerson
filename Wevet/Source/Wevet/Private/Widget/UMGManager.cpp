// Copyright 2018 wevet works All Rights Reserved.

#include "Widget/UMGManager.h"
#include "Widget/MainUIController.h"
#include "Character/CharacterBase.h"


UUMGManager::UUMGManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	MainUIController(nullptr)
{
}

void UUMGManager::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUMGManager::BeginDestroy()
{
	Super::BeginDestroy();
}

void UUMGManager::Initializer(ACharacterBase* const NewCharacter)
{
	MainUIController = CreateWidget<UMainUIController>(this, MainUIControllerTemp);
	if (MainUIController)
	{
		MainUIController->Initializer(NewCharacter);
		MainUIController->AddToViewport((int32)EUMGLayerType::Main);
	}

	check(NewCharacter->InputComponent);
	NewCharacter->InputComponent->BindAction("Pause", IE_Pressed, this, &UUMGManager::SetTickableWhenPaused);
}

void UUMGManager::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (MainUIController)
	{
		MainUIController->Renderer(InDeltaTime);
	}
}

void UUMGManager::SetTickableWhenPaused()
{
	if (MainUIController)
	{
		MainUIController->SetTickableWhenPaused();
	}
}
