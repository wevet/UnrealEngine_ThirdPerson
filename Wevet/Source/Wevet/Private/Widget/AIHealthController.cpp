// Copyright 2018 wevet works All Rights Reserved.


#include "Widget/AIHealthController.h"
#include "Engine/World.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "AI/AICharacterBase.h"
#include "Wevet.h"
#include "WevetExtension.h"

UAIHealthController::UAIHealthController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HealthProgressBarKeyName = (TEXT("HealthProgressBar"));
}

void UAIHealthController::NativeConstruct()
{
	Super::NativeConstruct();
	HealthProgressBar = Cast<UProgressBar>(GetWidgetFromName(HealthProgressBarKeyName));
}

void UAIHealthController::Initializer(ACharacterBase* const NewCharacterOwner)
{
	Super::Initializer(NewCharacterOwner);
	AICharacterPtr = MakeWeakObjectPtr<AAICharacterBase>(Cast<AAICharacterBase>(NewCharacterOwner));
}

void UAIHealthController::ResetCharacterOwner()
{
	if (AICharacterPtr.IsValid())
	{
		AICharacterPtr.Reset();
	}
	Super::ResetCharacterOwner();
}

void UAIHealthController::TickRenderer(const float InDeltaTime)
{
	Super::SetAlignmentInViewport(ViewPortOffset);
	if (!AICharacterPtr.IsValid() || Container == nullptr)
	{
		return;
	}

	if (Super::PlayerController == nullptr)
	{
		Super::PlayerController = Wevet::ControllerExtension::GetPlayer(AICharacterPtr->GetWorld());
	}

	// @NOTE
	// GetHead Position
	OwnerLocation = AICharacterPtr->GetHeadSocketLocation();

	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(AICharacterPtr->IsDeath_Implementation() ? ZERO_VALUE : AICharacterPtr->GetHealthToWidget());
	}

	// @NOTE
	// not renering or not found target or death 
	// equals hidden widget
	if (AICharacterPtr->IsDeath_Implementation() ||
		!AICharacterPtr->IsSeeTarget_Implementation() ||
		!AICharacterPtr->WasRecentlyRendered())
	{
		if (bWasVisible)
		{
			Super::Visibility(false);
		}
		return;
	}

	if (Super::PlayerController)
	{
		FVector2D ScreenLocation;
		ESlateVisibility SlateVisibility = Container->GetVisibility();
		bool bCanRendering = Super::PlayerController->ProjectWorldLocationToScreen(OwnerLocation, ScreenLocation, true);	
		Super::Visibility(bCanRendering);
		Super::SetPositionInViewport(ScreenLocation);
	}
}
