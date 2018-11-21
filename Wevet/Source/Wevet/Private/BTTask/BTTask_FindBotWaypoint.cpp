// Copyright 2018 wevet works All Rights Reserved.

#include "BTTask_FindBotWaypoint.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AIControllerBase.h"
#include "WayPointBase.h"
#include "Engine.h"

EBTNodeResult::Type UBTTask_FindBotWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());

	if (AIController)
	{
		AWayPointBase* NewWaypoint = AIController->GetRandomAtWayPoint();
		if (NewWaypoint)
		{
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID(), NewWaypoint);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
