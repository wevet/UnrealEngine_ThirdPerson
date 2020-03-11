// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoundInstigator.generated.h"

class USoundBase;

/*
* require interface
* ACharacterBase Class
*/
UINTERFACE(BlueprintType)
class WEVET_API USoundInstigator : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API ISoundInstigator
{
	GENERATED_IINTERFACE_BODY()

public :
	// Ignore FootStep AnimBP NotifyEvent
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SoundInstigator")
	void ReportNoise(USoundBase* Sound, float Volume);

	// FootStep NotifyEvent
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SoundInstigator")
	void FootStep(USoundBase* Sound, float Volume);

	// OtherActor MakeNoise
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SoundInstigator")
	void ReportNoiseOther(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location);
};
