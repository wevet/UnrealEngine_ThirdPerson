#pragma once

#include "CoreMinimal.h"
#include "IKTypes.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_HumanoidLegIKKneeCorrection.generated.h"


USTRUCT()
struct RTIK_API FAnimNode_HumanoidLegIKKneeCorrection : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
	FComponentSpacePoseLink BaseComponentPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UHumanoidLegChain_Wrapper* Leg;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEnableDebugDraw;

public:
	FAnimNode_HumanoidLegIKKneeCorrection() : Super()
	{
		bEnableDebugDraw = false;
		DeltaTime = 0.0f;
	}


	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override
	{
		Super::Initialize_AnyThread(Context);
		BaseComponentPose.Initialize(Context);
	}


	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override
	{
		Super::CacheBones_AnyThread(Context);
		BaseComponentPose.CacheBones(Context);
	}


	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		Super::UpdateInternal(Context);
		BaseComponentPose.Update(Context);
		DeltaTime = Context.GetDeltaTime();
	}
	

	virtual void EvaluateComponentSpaceInternal(FComponentSpacePoseContext& Output) override
	{
		Super::EvaluateComponentSpaceInternal(Output);
	}


	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr)
		{
			return false;
		}
		return Leg->InitIfInvalid(RequiredBones);
	}
	

	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		Super::InitializeBoneReferences(RequiredBones);

		if (Leg == nullptr)
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

};