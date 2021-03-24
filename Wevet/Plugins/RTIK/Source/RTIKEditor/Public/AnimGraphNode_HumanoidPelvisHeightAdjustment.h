
#pragma once

#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_HumanoidPelvisHeightAdjustment.h"
#include "AnimGraphNode_HumanoidPelvisHeightAdjustment.generated.h"

UCLASS()
class RTIKEDITOR_API UAnimGraphNode_HumanoidPelvisHeightAdjustment : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()
	
public:
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return FText::FromString(FString("Humanoid Pelvis Height Adjustment"));
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
		return FText::FromString(FString("Adjusts the hips and pelvis so legs can reach the floor during IK"));
	}

protected:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_HumanoidPelvisHeightAdjustment Node;
	
};
