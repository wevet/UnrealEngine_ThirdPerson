#pragma once

#include "CoreMinimal.h"
#include "IKTypes.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_HumanoidArmTorsoAdjust.generated.h"


USTRUCT()
struct RTIK_API FAnimNode_HumanoidArmTorsoAdjust : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links, meta = (PinShownByDefault))
	class URangeLimitedIKChainWrapper* LeftArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links, meta = (PinShownByDefault))
	class URangeLimitedIKChainWrapper* RightArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	EHumanoidArmTorsoIKMode Mode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEnableDebugDraw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	float Precision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	int32 MaxIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin = 0.0f))
	float MaxShoulderDragDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin=0.01f))
	float ShoulderDragStiffness;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin=0.0f, UIMax = 180.0f))
	float MaxPitchForwardDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin = 0.0f, UIMax = 180.0f))
	float MaxPitchBackwardDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin = 0.0f, UIMax = 180.0f))
	float MaxTwistDegreesLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin = 0.0f, UIMax = 180.0f))
	float MaxTwistDegreesRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin = 0.0f, UIMax = 1.0f))
	float ArmTwistRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso)
	EIKBoneAxis SkeletonForwardAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso)
	EIKBoneAxis SkeletonUpAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso)
	struct FIKBone WaistBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso)
	float TorsoRotationSlerpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	FTransform LeftArmWorldTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	FTransform RightArmWorldTarget;

public:
	FAnimNode_HumanoidArmTorsoAdjust() : Super()
	{
		Mode = EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_Disabled;
		SkeletonForwardAxis = EIKBoneAxis::IKBA_X;
		SkeletonUpAxis = EIKBoneAxis::IKBA_Z;
		LastEffectorOffset = FVector::ZeroVector;
		LeftArmWorldTarget = FTransform::Identity;
		RightArmWorldTarget = FTransform::Identity;
		LastRotationOffset = FQuat::Identity;
		MaxIterations = 10;
		MaxTwistDegreesLeft = 30.0f;
		MaxTwistDegreesRight = 30.0f;
		MaxPitchForwardDegrees = 60.0f;
		MaxPitchBackwardDegrees = 10.0f;
		MaxShoulderDragDistance = 50.f;
		ShoulderDragStiffness = 1.0f;
		Precision = 0.001f;
		ArmTwistRatio = 0.5f;
		TorsoRotationSlerpSpeed = 10.0f;
		bEnableDebugDraw = false;
		DeltaTime = 0.0f;
	}


	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		DeltaTime = Context.GetDeltaTime();
		Super::UpdateInternal(Context);
	}


	virtual void EvaluateComponentSpaceInternal(FComponentSpacePoseContext& Output) override
	{
		Super::EvaluateComponentSpaceInternal(Output);
	}


	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (LeftArm == nullptr || RightArm == nullptr)
		{
			return false;
		}
		if (!LeftArm->IsValid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize LeftArm : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		if (!RightArm->IsValid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize RightArm : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		if (!WaistBone.IsValid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize Waist : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return true;
	}


	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		Super::InitializeBoneReferences(RequiredBones);

		if (LeftArm == nullptr || RightArm == nullptr)
		{
			return;
		}

		if (!LeftArm->InitBoneReferences(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize LeftArm : %s"), *FString(__FUNCTION__));
#endif
			return;
		}
		if (!RightArm->InitBoneReferences(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize RightArm : %s"), *FString(__FUNCTION__));
#endif
			return;
		}
		if (!WaistBone.Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Could not initialize Waist : %s"), *FString(__FUNCTION__));
#endif
			return;
		}
	}


	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;

protected:
	float DeltaTime;
	FVector LastEffectorOffset;
	FQuat LastRotationOffset;
};
