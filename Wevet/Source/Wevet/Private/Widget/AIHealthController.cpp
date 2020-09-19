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

	ACharacterBase* Target = Cast<ACharacterBase>(IAIPawnOwner::Execute_GetTarget(AICharacterPtr.Get()));
	OwnerLocation = AICharacterPtr->GetHeadSocketLocation();
	const bool bWasDeath = AICharacterPtr->IsDeath_Implementation();
	const bool bWasRenderer = AICharacterPtr->WasRecentlyRendered(0.2f);

	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(bWasDeath ? ZERO_VALUE : AICharacterPtr->GetHealthToWidget());
	}

	if (!bWasRenderer || bWasDeath || !Target)
	{
		if (bWasVisible)
		{
			Super::Visibility(false);
		}
		return;
	}
	else if (!AICharacterPtr->IsSeeTarget_Implementation())
	{
		if (bWasVisible)
		{
			Super::Visibility(false);
		}
		return;
	}

	if (!Super::PlayerController)
	{
		Super::PlayerController = Cast<APlayerController>(Target->GetController());
	}
	else
	{
		FVector2D ScreenLocation;
		bool bCanRendering = Super::PlayerController->ProjectWorldLocationToScreen(OwnerLocation, ScreenLocation, true);
		Super::Visibility(bWasRenderer && bCanRendering);
		Super::SetPositionInViewport(ScreenLocation);
	}
}
