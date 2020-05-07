#pragma once

#include "CoreMinimal.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_HumanoidFootRotationController.generated.h"

USTRUCT()
struct RTIK_API FAnimNode_HumanoidFootRotationController : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
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
	FAnimNode_HumanoidFootRotationController()
	{
		RotationSlerpSpeed = 20.f;
		bEnableDebugDraw = false;
		bInterpolateRotation = true;
		DeltaTime = 0.0f;
		LastRotationOffset = FQuat::Identity;
	}

	// 更新処理
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		DeltaTime = Context.GetDeltaTime();
	}

	// 影響を受けるボーンの新しいコンポーネント空間変換を評価
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;

	// ノードの処理を行うかどうかの判定
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr || TraceData == nullptr)
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
		if (Leg == nullptr || TraceData == nullptr)
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
	FQuat LastRotationOffset;
};
