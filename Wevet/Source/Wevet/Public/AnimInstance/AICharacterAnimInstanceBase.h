// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAnimInstanceBase.h"
#include "AICharacterAnimInstanceBase.generated.h"

class AAICharacterBase;
/**
 * 
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class WEVET_API UAICharacterAnimInstanceBase : public UCharacterAnimInstanceBase
{
	GENERATED_BODY()
	
public:
	UAICharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer);
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Variable")
	class AAICharacterBase* Character;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	float LookAtInterpSpeed;

	virtual void SetRotator() override;
	virtual void SetMovementSpeed() override;
};
