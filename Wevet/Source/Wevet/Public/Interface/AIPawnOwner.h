// Copyright G2-Studios All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WevetTypes.h"
#include "AIPawnOwner.generated.h"


UINTERFACE(BlueprintType)
class WEVET_API UAIPawnOwner : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

// @NOTE
// AIController get property interface
class WEVET_API IAIPawnOwner
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	bool IsSeeTarget() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	bool IsHearTarget() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	float GetAttackTraceForwardDistance() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	float GetAttackTraceLongDistance() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	float GetAttackTraceMiddleDistance() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	AActor* GetTarget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	void StateChange(const EAIActionState NewAIActionState);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	bool CanMeleeStrike() const;
};
