// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseInvestigationNode.h"
#include "SearchNodeGenerator.generated.h"

UCLASS()
class WEVET_API ASearchNodeGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ASearchNodeGenerator();
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SearchNodeGenerator|Variable")
	TArray<class ABaseInvestigationNode*> GridNodes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SearchNodeGenerator|Variable")
	TSubclassOf<class ABaseInvestigationNode> SearchNodeTemplate;

public:
	UFUNCTION(BlueprintCallable, Category = "SearchNodeGenerator|Function", BlueprintPure)
	const TArray<class ABaseInvestigationNode*>& GetGridNodes()
	{
		return GridNodes;
	}

	void PrepareDestroy();
};
