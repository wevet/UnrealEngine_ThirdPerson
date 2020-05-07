#pragma once

#include "CoreMinimal.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_HumanoidPelvisHeightAdjustment.generated.h"

USTRUCT()
struct RTIK_API FAnimNode_HumanoidPelvisHeightAdjustment : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UHumanoidLegChain_Wrapper* LeftLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UHumanoidLegChain_Wrapper* RightLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Trace, meta = (PinShownByDefault))
	UHumanoidIKTraceData_Wrapper* LeftLegTraceData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Trace, meta = (PinShownByDefault))
	UHumanoidIKTraceData_Wrapper* RightLegTraceData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UIKBoneWrapper* PelvisBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	float PelvisAdjustVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	float MaxPelvisAdjustSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEnableDebugDraw;

public:
	FAnimNode_HumanoidPelvisHeightAdjustment()
	{
		DeltaTime = 0.0f;
		PelvisAdjustVelocity = 150.f;
		MaxPelvisAdjustSize = 50.f;
		bEnableDebugDraw = false;
		LastPelvisOffset = FVector::ZeroVector;
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
		if (LeftLeg == nullptr || RightLeg == nullptr || PelvisBone == nullptr)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}	
		if (!LeftLeg->InitIfInvalid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize LeftLeg : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		if (!RightLeg->InitIfInvalid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize RightLeg : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		if (!PelvisBone->InitIfInvalid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize PelvisBone : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return LeftLeg->InitIfInvalid(RequiredBones) && RightLeg->InitIfInvalid(RequiredBones) && PelvisBone->InitIfInvalid(RequiredBones);
	}

	// ボーンの参照の初期化
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (LeftLeg == nullptr || RightLeg == nullptr || PelvisBone == nullptr)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
			return;
		}
		if (!RightLeg->InitBoneReferences(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize RightLeg : %s"), *FString(__FUNCTION__));
#endif
		}
		if (!LeftLeg->InitBoneReferences(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize LeftLeg : %s"), *FString(__FUNCTION__));
#endif
		}
		if (!PelvisBone->Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize PelvisBone : %s"), *FString(__FUNCTION__));
#endif
		}
	}

protected:
	float DeltaTime;
	FVector LastPelvisOffset;
};
