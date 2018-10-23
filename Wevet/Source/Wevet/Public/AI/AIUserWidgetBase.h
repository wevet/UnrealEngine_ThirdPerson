// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AICharacterBase.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "AIUserWidgetBase.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class WEVET_API UAIUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UAIUserWidgetBase(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void Init(AAICharacterBase* InAICharacter);
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable")
	AAICharacterBase* CharacterOwner;

	UCanvasPanel* CanvasPanel;
	UProgressBar* ProgressBar;
	const FName ProgressHealthBarKeyName = TEXT("ProgressHealthBar");
};
