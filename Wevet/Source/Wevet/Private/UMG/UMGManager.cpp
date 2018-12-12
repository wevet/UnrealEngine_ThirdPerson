// Copyright 2018 wevet works All Rights Reserved.

#include "UMGManager.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "WidgetBlueprintLibrary.h"

UUMGManager::UUMGManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CharacterOwner(nullptr),
	BasePanel(nullptr),
	FocusPanel(nullptr),
	RadialProgressImage(nullptr),
	BasePanelKeyName(TEXT("BasePanel")),
	FocusPanelKeyName(TEXT("FocusPanel")),
	WeaponGridPanelKeyName(TEXT("WeaponGridPanel")),
	RadialProgressImageKeyName(TEXT("RadialProgressImage")),
	WeaponItemImageKeyName(TEXT("WeaponItemImage")),
	HealthScalarParameterValueName(TEXT("FillAmount")),
	bHasWeapon(false)
{
}

void UUMGManager::NativeConstruct()
{
	Super::NativeConstruct();
	BasePanel  = Cast<UCanvasPanel>(GetWidgetFromName(BasePanelKeyName));
	FocusPanel = Cast<UCanvasPanel>(GetWidgetFromName(FocusPanelKeyName));
	WeaponGridPanel = Cast<UUniformGridPanel>(GetWidgetFromName(WeaponGridPanelKeyName));
	RadialProgressImage = Cast<UImage>(GetWidgetFromName(RadialProgressImageKeyName));

	for (int index = 0; index < WeaponGridPanel->GetChildrenCount(); ++index)
	{
		if (UWidget* Widget = WeaponGridPanel->GetChildAt(index))
		{
			UImage* Image = Cast<UImage>(Widget);
			if (Image && Image->GetName() == WeaponItemImageKeyName.ToString())
			{
				WeaponItemImage = Image;
			}
			UE_LOG(LogWevetClient, Log, TEXT("Widget : %s"), *Widget->GetName());
		}
	}

	check(BasePanel && RadialProgressImage && FocusPanel && WeaponGridPanel && WeaponItemImage);
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

	SetHealth();
	SetVisibilityWeapon();
	SetVisibilityWeaponDetail();
}

void UUMGManager::SetHealth()
{
	if (RadialProgressImage == nullptr)
	{
		return;
	}
	float Health = CharacterOwner->GetHealthToWidget();
	float Offset = 0.01f;

	Health = (CharacterOwner->IsDeath_Implementation()) ? 0.f : (Health + Offset);
	if (UMaterialInstanceDynamic* Material = RadialProgressImage->GetDynamicMaterial())
	{
		Material->SetScalarParameterValue(HealthScalarParameterValueName, Health);
	}
	const FSlateBrush Brush = RadialProgressImage->Brush;
	RadialProgressImage->SetBrush(Brush);
}

void UUMGManager::SetVisibilityWeapon()
{
	if (FocusPanel == nullptr || WeaponGridPanel == nullptr || WeaponItemImage == nullptr)
	{
		return;
	}

	const bool bWeapon = (CharacterOwner->GetSelectedWeapon() != nullptr);
	if (bWeapon == bHasWeapon)
	{
		// same state
		return;
	}
	bHasWeapon = bWeapon;
	WeaponGridPanel->SetVisibility(bHasWeapon ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	WeaponItemImage->SetVisibility(bHasWeapon ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	FocusPanel->SetVisibility(bHasWeapon ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UUMGManager::SetVisibilityWeaponDetail()
{
	if (!bHasWeapon)
	{
		return;
	}
	check(CharacterOwner->GetSelectedWeapon());

	const FWeaponItemInfo& WeaponItemInfo = CharacterOwner->GetSelectedWeapon()->WeaponItemInfo;
	if (WeaponItemImage)
	{
		FSlateBrush Brush = WeaponItemImage->Brush;
		UWidgetBlueprintLibrary::MakeBrushFromTexture(WeaponItemInfo.Texture, 256, 194);
		WeaponItemImage->SetBrush(Brush);
	}
}
