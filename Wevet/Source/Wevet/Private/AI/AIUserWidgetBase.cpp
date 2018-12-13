// Copyright 2018 wevet works All Rights Reserved.
#include "AIUserWidgetBase.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"

UAIUserWidgetBase::UAIUserWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CharacterOwner(nullptr),
	CanvasPanel(nullptr),
	ProgressBar(nullptr),
	ProgressHealthBarKeyName(TEXT("ProgressHealthBar")),
	CanvasPanelKeyName(TEXT("BasePanel"))
{
	//
}

void UAIUserWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	CanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(CanvasPanelKeyName));
	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(ProgressHealthBarKeyName));
	check(CanvasPanel && ProgressBar);
}

void UAIUserWidgetBase::Init(AAICharacterBase* NewCharacter)
{
	CharacterOwner = NewCharacter;
	check(CharacterOwner);
}

void UAIUserWidgetBase::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CharacterOwner == nullptr)
	{
		return;
	}

	// health
	if (ProgressBar)
	{
		float Health = CharacterOwner->GetHealthToWidget();
		if (CharacterOwner->IsDeath_Implementation())
		{
			Health = 0.f;
		}
		ProgressBar->SetPercent(Health);
	}
}
