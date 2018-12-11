// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MockCharacter.h"
#include "UMGManager.generated.h"

class UProgressBar;
class UCanvasPanel;
class UImage;

UCLASS()
class WEVET_API UUMGManager : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	UUMGManager(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	void Init(ACharacterBase* NewCharacter);
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UMGManager|Variable")
	ACharacterBase* CharacterOwner;

	UImage* ProgressBar;
	UCanvasPanel* CanvasPanel;
	/* widget item name */
	const FName ProgressHealthBarKeyName = TEXT("RadialProgressImage");
	const FName CanvasPanelKeyName = TEXT("BasePanel");

	/* material param name */
	const FName HealthParameterName = TEXT("FillAmount");
};
