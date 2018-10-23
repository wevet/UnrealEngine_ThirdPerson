// Copyright 2018 wevet works All Rights Reserved.

#include "AIUserWidgetBase.h"

UAIUserWidgetBase::UAIUserWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CharacterOwner(nullptr)
{
	//
}

void UAIUserWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	this->CanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName("BasePanel"));
	this->ProgressBar = Cast<UProgressBar>(GetWidgetFromName(ProgressHealthBarKeyName));
	check(this->CanvasPanel);
	check(this->ProgressBar);
}

void UAIUserWidgetBase::Init(AAICharacterBase * InAICharacter)
{
	this->CharacterOwner = InAICharacter;
	check(this->CharacterOwner);
}

void UAIUserWidgetBase::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{

}



