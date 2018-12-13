// Copyright 2018 wevet works All Rights Reserved.

#include "UMGManager.h"
#include "CharacterBase.h"
#include "MainUIController.h"


UUMGManager::UUMGManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UUMGManager::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUMGManager::Initializer(ACharacterBase* NewCharacter)
{
	check(MainUIControllerTemp);
	MainUIController = CreateWidget<UMainUIController>(this, MainUIControllerTemp);
	if (MainUIController)
	{
		MainUIController->Initializer(NewCharacter);
		MainUIController->AddToViewport((int32)EUMGLayerType::Main);
	}
}

void UUMGManager::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (MainUIController)
	{
		MainUIController->Renderer(InDeltaTime);
	}
}
