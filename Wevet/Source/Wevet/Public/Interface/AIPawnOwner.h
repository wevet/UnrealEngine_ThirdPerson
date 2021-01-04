// Copyright 2018 wevet works All Rights Reserved.

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
	float GetMeleeDistance() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	AActor* GetTarget() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	void CombatStateChange(const EAICombatState NewAICombatState);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	void ActionStateChange(const EAIActionState NewAIActionState);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIPawnOwner")
	bool CanMeleeStrike() const;
};
