// Copyright 2018 wevet works All Rights Reserved.
#include "UMGManager.h"

UUMGManager::UUMGManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CharacterOwner(nullptr)
{
	//
}

void UUMGManager::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUMGManager::Init(ACharacterBase* NewCharacter)
{
	CharacterOwner = NewCharacter;
}

void UUMGManager::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
}

