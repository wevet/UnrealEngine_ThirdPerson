// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMGManager.generated.h"

class AMockCharacter;
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
	void BuildInitialize(AMockCharacter* Character);
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "UMGManager|Variable")
	AMockCharacter* CharacterOwner;
};
