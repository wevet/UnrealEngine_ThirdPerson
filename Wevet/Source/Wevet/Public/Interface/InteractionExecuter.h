// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InteractionExecuter.generated.h"

class USoundBase;

UINTERFACE(BlueprintType)
class WEVET_API UInteractionExecuter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IInteractionExecuter
{
	GENERATED_IINTERFACE_BODY()

public :

	// Ignore FootStep AnimBP NotifyEvent
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IInteractionExecuter")
	void ReportNoise(USoundBase* Sound, float Volume);

	// FootStep NotifyEvent
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IInteractionExecuter")
	void FootStep(USoundBase* Sound, float Volume);

	// OtherActor MakeNoise
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IInteractionExecuter")
	void ReportNoiseOther(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IInteractionExecuter")
	void OnReleaseItemExecuter();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IInteractionExecuter")
	void OnPickupItemExecuter(AActor* Actor);
};
