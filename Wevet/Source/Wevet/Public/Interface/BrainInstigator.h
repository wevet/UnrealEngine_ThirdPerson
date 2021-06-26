// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WevetTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "BrainInstigator.generated.h"


class UBehaviorTree;

UINTERFACE(BlueprintType)
class WEVET_API UBrainInstigator : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IBrainInstigator
{
	GENERATED_IINTERFACE_BODY()

public :
	/// <summary>
	/// Asset to manage AI
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BrainInstigator")
	class UBehaviorTree* GetBehaviorTree() const;

	/// <summary>
	/// Vision
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BrainInstigator")
	void DoSightReceive(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew);

	/// <summary>
	/// Hearing
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BrainInstigator")
	void DoHearReceive(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew);

	/// <summary>
	/// Prediction
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BrainInstigator")
	void DoPredictionReceive(AActor* Actor, const FAIStimulus InStimulus);

	/// <summary>
	/// Pain
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BrainInstigator")
	void DoDamageReceive(AActor* Actor, const FAIStimulus InStimulus);
};
