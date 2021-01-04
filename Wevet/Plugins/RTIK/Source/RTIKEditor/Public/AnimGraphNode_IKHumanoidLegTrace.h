
#pragma once

#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_IKHumanoidLegTrace.h"
#include "AnimGraphNode_IKHumanoidLegTrace.generated.h"

UCLASS()
class RTIKEDITOR_API UAnimGraphNode_IKHumanoidLegTrace : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()
	
public:

	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return FText::FromString(FString("Humanoid IK Leg Trace"));
	}

	FLinearColor GetNodeTitleColor() const override
	{
		return FLinearColor(0, 1, 1, 1);
	}
	
	FString GetNodeCategory() const override
	{
		return FString("IK Nodes");
	}
	
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override 
	{
		return &Node; 
	}

protected:
	virtual FText GetControllerDescription() const
	{
		return FText::FromString(FString("Traces from the leg to the floor, providing trace data used later in IK"));
	}

protected:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_IKHumanoidLegTrace Node;

};