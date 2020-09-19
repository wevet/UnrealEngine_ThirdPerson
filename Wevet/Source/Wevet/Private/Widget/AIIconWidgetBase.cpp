// Copyright 2018 wevet works All Rights Reserved.


#include "Widget/AIIconWidgetBase.h"
#include "Engine/World.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Character/CharacterBase.h"
#include "WevetExtension.h"


UAIIconWidgetBase::UAIIconWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	bWasVisible(false)
{
	ContainerKeyName = (TEXT("Container"));
	ViewPortOffset = FVector2D(0.5f, 0.f);
	Offset = 100.f;
	Subtract = 10.f;
	MinScaleValue = 0.2f;
	MaxScaleValue = 1.0f;
}

void UAIIconWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	Container = Cast<USizeBox>(GetWidgetFromName(ContainerKeyName));
}

void UAIIconWidgetBase::TickRenderer(const float InDeltaTime)
{
}

void UAIIconWidgetBase::Initializer(ACharacterBase* const NewCharacterOwner)
{
	CharacterPtr = MakeWeakObjectPtr<ACharacterBase>(NewCharacterOwner);
}

void UAIIconWidgetBase::ResetCharacterOwner()
{
	if (CharacterPtr.IsValid())
	{
		CharacterPtr.Reset();
	}
}

void UAIIconWidgetBase::Visibility(const bool InVisibility)
{
	if (!Container)
	{
		return;
	}
	if (bWasVisible != InVisibility)
	{
		bWasVisible = InVisibility;
		Container->SetVisibility(bWasVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
}

void UAIIconWidgetBase::SetViewPortOffset(const FVector2D InViewPortOffset)
{
	ViewPortOffset = InViewPortOffset;
}

