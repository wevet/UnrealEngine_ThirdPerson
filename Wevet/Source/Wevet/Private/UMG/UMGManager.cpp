// Copyright 2018 wevet works All Rights Reserved.
#include "UMGManager.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

UUMGManager::UUMGManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CharacterOwner(nullptr),
	CanvasPanel(nullptr),
	ProgressBar(nullptr)
{
	//
}

void UUMGManager::NativeConstruct()
{
	Super::NativeConstruct();
	CanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(CanvasPanelKeyName));
	ProgressBar = Cast<UImage>(GetWidgetFromName(ProgressHealthBarKeyName));
	check(CanvasPanel && ProgressBar);
}

void UUMGManager::Init(ACharacterBase* NewCharacter)
{
	CharacterOwner = NewCharacter;
	check(CharacterOwner);
}

void UUMGManager::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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
		float Offset = 0.01f;

		Health = (CharacterOwner->IsDeath_Implementation()) ? 0.f : (Health + Offset);
		if (UMaterialInstanceDynamic* Material = ProgressBar->GetDynamicMaterial())
		{
			Material->SetScalarParameterValue(HealthParameterName, Health);
		}
		const FSlateBrush Brush = ProgressBar->Brush;
		ProgressBar->SetBrush(Brush);
	}
}
