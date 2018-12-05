// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MockCharacter.h"
#include "UMGManager.generated.h"

/**
 * 
 */
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
};
