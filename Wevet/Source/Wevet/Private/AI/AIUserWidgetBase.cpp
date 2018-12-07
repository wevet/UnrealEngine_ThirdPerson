// Copyright 2018 wevet works All Rights Reserved.
#include "AIUserWidgetBase.h"

UAIUserWidgetBase::UAIUserWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CharacterOwner(nullptr),
	CanvasPanel(nullptr),
	ProgressBar(nullptr)
{
	//
}

void UAIUserWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	CanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(CanvasPanelKeyName));
	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(ProgressHealthBarKeyName));
	check(CanvasPanel);
	check(ProgressBar);
}

void UAIUserWidgetBase::Init(AAICharacterBase* NewCharacter)
{
	CharacterOwner = NewCharacter;
	check(CharacterOwner);
}

void UAIUserWidgetBase::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CharacterOwner && ProgressBar)
	{
		float Health = CharacterOwner->GetHealthToWidget();
		if (CharacterOwner->IsDeath_Implementation())
		{
			Health = 0.f;
		}
		ProgressBar->SetPercent(Health);
	}

}
