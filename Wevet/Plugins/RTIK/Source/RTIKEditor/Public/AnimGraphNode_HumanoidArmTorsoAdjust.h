
#pragma once

#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_HumanoidArmTorsoAdjust.h"
#include "AnimGraphNode_HumanoidArmTorsoAdjust.generated.h"

UCLASS()
class RTIKEDITOR_API UAnimGraphNode_HumanoidArmTorsoAdjust : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()
	
public:

	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return FText::FromString(FString("IK Humanoid Arm Torso Adjustment"));
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
		return FText::FromString(FString("Adjust Humanoid Torso Rotation Before IK"));		
	}
	
protected:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_HumanoidArmTorsoAdjust Node;

};