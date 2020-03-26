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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IconWidgetBase|Variable")
	FName ContainerKeyName;

	class USizeBox* Container;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IconWidgetBase|Variable")
	float Offset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IconWidgetBase|Variable")
	float Subtract;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IconWidgetBase|Variable")
	float MinScaleValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IconWidgetBase|Variable")
	float MaxScaleValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IconWidgetBase|Variable")
	class APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IconWidgetBase|Variable")
	FVector	OwnerLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IconWidgetBase|Variable")
	FVector2D ViewPortOffset;

	TWeakObjectPtr<class ACharacterBase> CharacterPtr;
	bool bWasVisible;

public:
	virtual void Initializer(ACharacterBase* const NewCharacterOwner);
	virtual void ResetCharacterOwner();
	void Visibility(const bool InVisibility);
	void SetViewPortOffset(const FVector2D InViewPortOffset);

protected:
	UFUNCTION(BlueprintCallable, Category = "IconWidgetBase|Function")
	virtual void TickRenderer(const float InDeltaTime);
};
