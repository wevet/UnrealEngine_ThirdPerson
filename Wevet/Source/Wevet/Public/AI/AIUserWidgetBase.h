// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AICharacterBase.h"
#include "AIUserWidgetBase.generated.h"

class UProgressBar;
class UCanvasPanel;

UCLASS(ABSTRACT)
class WEVET_API UAIUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UAIUserWidgetBase(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	virtual void Init(AAICharacterBase* NewCharacter);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UAIUserWidgetBase|Variable")
	class AAICharacterBase* CharacterOwner;

	class UProgressBar* ProgressBar;
	class UCanvasPanel* CanvasPanel;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UAIUserWidgetBase|Variable")
	FName ProgressHealthBarKeyName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UAIUserWidgetBase|Variable")
	FName CanvasPanelKeyName;
};
