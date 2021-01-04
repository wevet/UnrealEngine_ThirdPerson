// Copyright 2018 wevet works All Rights Reserved.

#include "BTTask/BTTask_FindPatrolLocation.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI/AIControllerBase.h"
#include "AI/AICharacterBase.h"
#include "AI/WayPointBase.h"
#include "Engine.h"
#include "NavigationSystem.h"
#include "Interface/AIPawnOwner.h"

UBTTask_FindPatrolLocation::UBTTask_FindPatrolLocation() : Super()
{
	NodeName = TEXT("BTTask_FindPatrolLocation");
}

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner()))
	{
		AWayPointBase* WayPoint = AIController->GetWayPoint();
		APawn* ControlPawn = AIController->GetPawn();
		IAIPawnOwner* Interface = Cast<IAIPawnOwner>(ControlPawn);

		if (!ControlPawn || !Interface || !WayPoint)
		{
			return EBTNodeResult::Failed;
		}

		const float SearchRadius = IAIPawnOwner::Execute_GetMeleeDistance(ControlPawn);
		const FVector SearchOrigin = WayPoint->GetActorLocation();
		FNavLocation ResultLocation;
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(AIController);
		if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, SearchRadius, ResultLocation))
		{
			AIController->SetBlackboardPatrolLocation(ResultLocation.Location);
			AIController->SetBlackboardPatrolPointsHolder(WayPoint);
		}

	}
	return EBTNodeResult::Succeeded;
}
