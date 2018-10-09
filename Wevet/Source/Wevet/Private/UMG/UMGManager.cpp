// Fill out your copyright notice in the Description page of Project Settings.

#include "UMGManager.h"
#include "MockCharacter.h"


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

