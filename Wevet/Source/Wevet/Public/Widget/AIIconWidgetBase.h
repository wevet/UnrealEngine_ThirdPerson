// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AIIconWidgetBase.generated.h"

class USizeBox;
class ACharacterBase;

UCLASS()
class WEVET_API UAIIconWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UAIIconWidgetBase(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UAIIconWidgetBase|Variable")
	FName ContainerKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UAIIconWidgetBase|Variable")
	FVector	OwnerLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UAIIconWidgetBase|Variable")
	FVector2D ViewPortOffset;

	class USizeBox* Container;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UAIIconWidgetBase|Variable")
	class APlayerController* PlayerController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UAIIconWidgetBase|Variable")
	float Offset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UAIIconWidgetBase|Variable")
	float Subtract;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UAIIconWidgetBase|Variable")
	float MinScaleValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UAIIconWidgetBase|Variable")
	float MaxScaleValue;

	TWeakObjectPtr<class ACharacterBase> CharacterPtr;

public:
	virtual void Initializer(ACharacterBase* const NewCharacterOwner);
	virtual void ResetCharacterOwner();

	virtual void SetViewPortOffset(const FVector2D InViewPortOffset);

	UFUNCTION(BlueprintCallable, Category = "UAIIconWidgetBase|Function")
	virtual void TickRenderer(const float InDeltaTime);

	virtual void UpdateRenderingViewport();
	virtual void Visibility(const bool InVisibility);
};
