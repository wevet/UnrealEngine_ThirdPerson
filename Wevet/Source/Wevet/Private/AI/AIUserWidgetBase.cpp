// Copyright 2018 wevet works All Rights Reserved.
#include "AIUserWidgetBase.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"

UAIUserWidgetBase::UAIUserWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CharacterOwner = nullptr;
	CanvasPanel = nullptr;
	ProgressBar = nullptr;
	ProgressHealthBarKeyName = (TEXT("ProgressHealthBar"));
	CanvasPanelKeyName = (TEXT("BasePanel"));
}

void UAIUserWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	CanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(CanvasPanelKeyName));
	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(ProgressHealthBarKeyName));
	check(CanvasPanel && ProgressBar);
}

void UAIUserWidgetBase::Initializer(AAICharacterBase* NewCharacter)
{
	CharacterOwner = NewCharacter;
}

void UAIUserWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CharacterOwner == nullptr || ProgressBar == nullptr)
	{
		return;
	}

	ProgressBar->SetPercent(CharacterOwner->IsDeath_Implementation() ? 0.f : CharacterOwner->GetHealthToWidget());
}
