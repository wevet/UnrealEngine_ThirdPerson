#pragma once

#include "CoreMinimal.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_HumanoidLegIKKneeCorrection.generated.h"


USTRUCT()
struct RTIK_API FAnimNode_HumanoidLegIKKneeCorrection : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
	// Pose before any IK or IK pre-processing (e.g., pelvis adjustment) is applied
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
	FComponentSpacePoseLink BaseComponentPose;

	// The leg on which IK is applied
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UHumanoidLegChain_Wrapper* Leg;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEnableDebugDraw;

public:
	FAnimNode_HumanoidLegIKKneeCorrection()
	{
		bEnableDebugDraw = false;
		DeltaTime = 0.0f;
	}

	// Node初期化
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override
	{
		Super::Initialize_AnyThread(Context);
		BaseComponentPose.Initialize(Context);
	}

	// すべてのボーンをキャッシュするために呼び出す
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override
	{
		Super::CacheBones_AnyThread(Context);
		BaseComponentPose.CacheBones(Context);
	}

	// 更新処理
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		Super::UpdateInternal(Context);
		BaseComponentPose.Update(Context);
		DeltaTime = Context.GetDeltaTime();
	}
	
	// 影響を受けるボーンの新しいコンポーネント空間変換を評価
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;

	// 骨格制御ベースを評価
	virtual void EvaluateComponentSpaceInternal(FComponentSpacePoseContext& Output) override
	{
		Super::EvaluateComponentSpaceInternal(Output);
	}

	// ノードの処理を行うかどうかの判定
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		bool bValid = Leg->InitIfInvalid(RequiredBones);
#if ENABLE_IK_DEBUG_VERBOSE
		if (!bValid)
		{
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize : %s"), *FString(__FUNCTION__));
		}
#endif 
		return bValid;
	}
	
	// ボーンの参照の初期化
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
			return;
		}
		if (!Leg->InitBoneReferences(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize : %s"), *FString(__FUNCTION__));
#endif
		}
	}

protected:
	float DeltaTime;

};