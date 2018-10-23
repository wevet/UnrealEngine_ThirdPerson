// Copyright 2018 wevet works All Rights Reserved.

#include "UMGManager.h"


UUMGManager::UUMGManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//
}

void UUMGManager::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUMGManager::BuildInitialize(AMockCharacter* Character)
{
	this->CharacterOwner = Character;
}

