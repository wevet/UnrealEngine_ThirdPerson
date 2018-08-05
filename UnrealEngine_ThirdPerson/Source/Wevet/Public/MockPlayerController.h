// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MockPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API AMockPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMockPlayerController(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AMockPlayerController|Valiable")
	TSubclassOf<class UUserWidget> WidgetMainUI;

	UUserWidget* Widget;

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category="AMockPlayerController|Valiable")
	void Test(FString InString);
};
