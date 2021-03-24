#pragma once

#include "CoreMinimal.h"
#include "IKTypes.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AnimNode_HumanoidFootRotationController.generated.h"

USTRUCT()
struct RTIK_API FAnimNode_HumanoidFootRotationController : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UHumanoidLegChain_Wrapper* Leg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UHumanoidIKTraceData_Wrapper* TraceData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinHiddenByDefault))
	float RotationSlerpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bInterpolateRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEnableDebugDraw;
   
public:
	FAnimNode_HumanoidFootRotationController() : Super()
	{
		LastRotationOffset = FQuat::Identity;
		RotationSlerpSpeed = 20.0f;
		bEnableDebugDraw = false;
		bInterpolateRotation = true;
		DeltaTime = 0.0f;
	}

	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		DeltaTime = Context.GetDeltaTime();
	}

	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr || TraceData == nullptr)
		{
			return false;
		}
		const bool bValid = Leg->InitIfInvalid(RequiredBones);
		return bValid;
	}

	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr || TraceData == nullptr)
		{
			return;
		}
		if (!Leg->InitBoneReferences(RequiredBones))
		{
			//
		}
	}

	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;

protected:
	float DeltaTime;
	FQuat LastRotationOffset;
};
