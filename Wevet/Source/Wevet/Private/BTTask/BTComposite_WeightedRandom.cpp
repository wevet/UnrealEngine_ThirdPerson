// Copyright 2018 wevet works All Rights Reserved.


#include "BTTask/BTComposite_WeightedRandom.h"

#define TWO 2
#define MAX 100

UBTComposite_WeightedRandom::UBTComposite_WeightedRandom(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	LeftChildSelectingRate(1.0f)
{
	NodeName = "Weighted Random";
	//OnNextChild.BindUObject(this, &UBTComposite_WeightedRandom::GetNextChildHandler);
}

/*
*	Define conditions and methods for transitioning to the next child node
*/
int32 UBTComposite_WeightedRandom::GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	int32 NextChildIndex = BTSpecialChild::ReturnToParent;
	// If the number of child Nodes is 2 and it has just transitioned to this Node
	if (GetChildrenNum() == TWO && PrevChild == BTSpecialChild::NotInitialized)
	{
		const int32 LeftChildIndex = 0;
		const int32 RightChildIndex = 1;
		NextChildIndex = (FMath::FRand() <= LeftChildSelectingRate) ? LeftChildIndex : RightChildIndex;
	}
	return NextChildIndex;
}


FString UBTComposite_WeightedRandom::GetStaticDescription() const
{
	int32 ChildrenNum = GetChildrenNum();
	// If the number of child Nodes is 2, display each branch probability
	if (ChildrenNum == TWO)
	{
		float LeftPercentage = LeftChildSelectingRate * MAX;
		float RightPercentage = MAX - LeftPercentage;
		return FString::Printf(TEXT("Left : %.2f / Right : %.2f"), LeftPercentage, RightPercentage);
	}
	// Show warning if the number of child Nodes is not 2
	return FString::Printf(TEXT("Warning : Connect Just 2 Children Nodes (Currently %d Node(s))"), ChildrenNum);
}


bool UBTComposite_WeightedRandom::CanAbortLowerPriority() const
{
	// As with Sequence Nodes, prevent child Nodes from stealing processing from lower priority Nodes
	return false;
}


FName UBTComposite_WeightedRandom::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Composite.Selector.Icon");
}

