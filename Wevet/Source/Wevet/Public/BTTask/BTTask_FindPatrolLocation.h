// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindPatrolLocation.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API UBTTask_FindPatrolLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
};
