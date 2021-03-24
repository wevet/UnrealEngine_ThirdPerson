#pragma once

#include "CoreMinimal.h"
#include "IKTypes.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AnimNode_IKHumanoidLegTrace.generated.h"

USTRUCT()
struct RTIK_API FAnimNode_IKHumanoidLegTrace : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

protected:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UHumanoidLegChain_Wrapper* Leg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UIKBoneWrapper* PelvisBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Trace, meta = (PinShownByDefault))
	UHumanoidIKTraceData_Wrapper* TraceData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	float MaxPelvisAdjustSize;
   
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEnableDebugDraw;

public:
	FAnimNode_IKHumanoidLegTrace() : Super()
	{
		MaxPelvisAdjustSize = 40.0f;
		bEnableDebugDraw = false;
	}

	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		if (TraceData)
		{
			TraceData->SetUpdatedThisTick(false);
		}
	}

	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr || PelvisBone == nullptr)
		{
			return false;
		}
		if (TraceData == nullptr)
		{
			return false;
		}
		return Leg->InitIfInvalid(RequiredBones);
	}

	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr)
		{
			return;
		}

		if (!Leg->InitBoneReferences(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize LeftLeg : %s"), *FString(__FUNCTION__));
#endif
		}
		Super::InitializeBoneReferences(RequiredBones);
	}

	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;

};
