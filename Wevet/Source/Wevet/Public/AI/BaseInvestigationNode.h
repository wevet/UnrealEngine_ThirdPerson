// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h"
#include "BaseInvestigationNode.generated.h"

UCLASS()
class WEVET_API ABaseInvestigationNode : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	ABaseInvestigationNode();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	FGameplayTagContainer GameplayTagContainer;

public:
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer = GameplayTagContainer;
	}

};
