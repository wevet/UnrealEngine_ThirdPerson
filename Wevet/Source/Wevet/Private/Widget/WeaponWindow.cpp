// Copyright 2018 wevet works All Rights Reserved.


#include "Widget/WeaponWindow.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine.h"

#define WIDTH 256
#define HEIGHT 128

UWeaponWindow::UWeaponWindow(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ContainerKeyName = FName(TEXT("Container"));
	HeaderKeyName = FName(TEXT("Header"));
	HeaderRootKeyName = FName(TEXT("HeaderRoot"));
	DisplayKeyName = FName(TEXT("DisplayName"));

	FooterKeyName = FName(TEXT("Footer"));
	WeaponRootKeyName = FName(TEXT("WeaponRoot"));
	WeaponImageKeyName = FName(TEXT("WeaponImage"));
	AmmoCounterKeyName = FName(TEXT("AmmoCounter"));
	CurrentAmmoKeyName = FName(TEXT("CurrentAmmo"));
	MaxAmmoKeyName = FName(TEXT("MaxAmmo"));
	bWasVisibility = false;
}

void UWeaponWindow::NativeConstruct()
{
	Super::NativeConstruct();

	Container = Cast<UVerticalBox>(GetWidgetFromName(ContainerKeyName));
	auto Header = Cast<UPanelWidget>(GetTargetWidget(Container, HeaderKeyName));
	auto HeaderRoot = Cast<UPanelWidget>(GetTargetWidget(Header, HeaderRootKeyName));
	DisplayName = Cast<UTextBlock>(GetTargetWidget(HeaderRoot, DisplayKeyName));

	auto WeaponRoot = Cast<UPanelWidget>(GetWidgetFromName(WeaponRootKeyName));
	WeaponImage = Cast<UImage>(GetTargetWidget(WeaponRoot, WeaponImageKeyName));

	auto Footer = Cast<UPanelWidget>(GetTargetWidget(Container, FooterKeyName));
	auto AmmoCounter = Cast<UPanelWidget>(GetTargetWidget(Footer, AmmoCounterKeyName));
	CurrentAmmoText = Cast<UTextBlock>(GetTargetWidget(AmmoCounter, CurrentAmmoKeyName));
	MaxAmmoText = Cast<UTextBlock>(GetTargetWidget(AmmoCounter, MaxAmmoKeyName));
}

void UWeaponWindow::RendererAmmos(AAbstractWeapon* const InWeapon)
{
	if (!InWeapon || !CurrentAmmoText || !MaxAmmoText)
	{
		return;
	}

	CurrentAmmoText->SetText(FText::FromString(FString::FromInt(InWeapon->GetWeaponItemInfo().CurrentAmmo)));
	MaxAmmoText->SetText(FText::FromString(FString::FromInt(InWeapon->GetWeaponItemInfo().MaxAmmo)));
}

void UWeaponWindow::RendererImage(AAbstractWeapon* const InWeapon)
{
	if (!InWeapon || !WeaponImage || !DisplayName)
	{
		return;
	}

	const FWeaponItemInfo& WeaponItemInfo = InWeapon->GetWeaponItemInfo();
	FSlateBrush Brush = UWidgetBlueprintLibrary::MakeBrushFromTexture(InWeapon->GetWeaponItemInfo().Texture, WIDTH, HEIGHT);
	WeaponImage->SetBrush(Brush);

	DisplayName->SetText(FText::FromString(WeaponItemInfo.DisplayName));
}

void UWeaponWindow::Visibility(const bool InVisibility)
{
	if (!Container)
	{
		return;
	}

	if (bWasVisibility != InVisibility)
	{
		bWasVisibility = InVisibility;
		Container->SetVisibility(bWasVisibility ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
}

UWidget* UWeaponWindow::GetTargetWidget(UPanelWidget* const InRootWidget, const FName InTargetWidgetName)
{
	if (!InRootWidget)
	{
		return nullptr;
	}

	for (int index = 0; index < InRootWidget->GetChildrenCount(); ++index)
	{
		UWidget* Widget = InRootWidget->GetChildAt(index);
		if (!Widget)
		{
			continue;
		}

		if (Widget->GetName() == InTargetWidgetName.ToString())
		{
			return Widget;
		}
		else 
		{
			if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
			{
				if (Panel->GetChildrenCount() > 0)
				{
					GetTargetWidget(Panel, InTargetWidgetName);
				}
			}
		}
	}
	return nullptr;
}
