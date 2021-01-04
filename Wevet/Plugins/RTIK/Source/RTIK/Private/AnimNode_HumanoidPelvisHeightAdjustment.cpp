
#include "AnimNode_HumanoidPelvisHeightAdjustment.h"
#include "RTIK.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationRuntime.h"
#include "AnimUtil.h"
#include "HumanoidIK.h"

#if WITH_EDITOR
#include "DebugDrawUtil.h"
#endif

DECLARE_CYCLE_STAT(TEXT("IK Humanoid Pelvis Height Adjust Eval"), STAT_HumanoidPelvisHeightAdjust_Eval, STATGROUP_Anim);

void FAnimNode_HumanoidPelvisHeightAdjustment::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_HumanoidPelvisHeightAdjust_Eval);

#if ENABLE_ANIM_DEBUG
	check(Output.AnimInstanceProxy->GetSkelMeshComponent());
#endif
	check(OutBoneTransforms.Num() == 0);

	if (LeftLeg == nullptr || RightLeg == nullptr || PelvisBone == nullptr)
	{
#if ENABLE_IK_DEBUG_VERBOSE
		UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
		return;
	}

	if (LeftLegTraceData == nullptr || RightLegTraceData == nullptr)
	{
#if ENABLE_IK_DEBUG_VERBOSE
		UE_LOG(LogRTIK, Error, TEXT("An input trace data object was null : %s"), *FString(__FUNCTION__));
#endif
		return;
	}

	USkeletalMeshComponent* SkelComp = Output.AnimInstanceProxy->GetSkelMeshComponent();
	ACharacter* Character = Cast<ACharacter>(SkelComp->GetOwner());
	if (Character == nullptr)
	{
#if ENABLE_IK_DEBUG_VERBOSE
		UE_LOG(LogRTIK, Error, TEXT("ACharacter classes object was null : %s"), *FString(__FUNCTION__));
#endif
		return;
	}

	UWorld* World = Character->GetWorld();

	bool bReturnToCenter = false;
	float TargetPelvisDelta = 0.0f;

	if (LeftLegTraceData->GetTraceData().FootHitResult.GetActor() == nullptr &&  RightLegTraceData->GetTraceData().FootHitResult.GetActor() == nullptr)
	{
		bReturnToCenter = true;
	}
	else
	{
		const FVector ToCS = -1 * SkelComp->GetComponentLocation();
		FVector LeftFootFloorCS;
		FVector RightFootFloorCS;

		const FVector LeftFootCS = FAnimUtil::GetBoneCSLocation(*SkelComp, Output.Pose, LeftLeg->Chain.ShinBone.BoneIndex);
		const FVector RightFootCS = FAnimUtil::GetBoneCSLocation(*SkelComp, Output.Pose, RightLeg->Chain.ShinBone.BoneIndex);
		const FVector RootCS = FAnimUtil::GetBoneCSLocation(*SkelComp, Output.Pose, FCompactPoseBoneIndex(0));

		LeftLeg->Chain.GetIKFloorPointCS(*SkelComp, LeftLegTraceData->GetTraceData(), LeftFootFloorCS);
		RightLeg->Chain.GetIKFloorPointCS(*SkelComp, RightLegTraceData->GetTraceData(), RightFootFloorCS);

		// Move so the lowest floor point is in reach
		TargetPelvisDelta = LeftFootFloorCS.Z < RightFootFloorCS.Z ? LeftFootFloorCS.Z - RootCS.Z : RightFootFloorCS.Z - RootCS.Z;

		if (FMath::Abs(TargetPelvisDelta) > MaxPelvisAdjustSize)
		{
			bReturnToCenter = true;
			TargetPelvisDelta = 0.0f;
		}
	}

	const FVector TargetPelvisDeltaVec(0.0f, 0.0f, TargetPelvisDelta);
	FTransform PelvisTransformCS = FAnimUtil::GetBoneCSTransform(*SkelComp, Output.Pose, PelvisBone->Bone.BoneIndex);
	const FVector PelvisTargetCS = PelvisTransformCS.GetLocation() + TargetPelvisDeltaVec;
	const FVector PreviousPelvisLoc = PelvisTransformCS.GetLocation() + LastPelvisOffset;
	const FVector PelvisAdjustVec = LastPelvisOffset + (PelvisTargetCS - PreviousPelvisLoc).GetClampedToMaxSize(PelvisAdjustVelocity * DeltaTime);
	const FVector NewPelvisLoc = PelvisTransformCS.GetLocation() + PelvisAdjustVec;
	LastPelvisOffset = PelvisAdjustVec;
	PelvisTransformCS.SetLocation(NewPelvisLoc);
	OutBoneTransforms.Add(FBoneTransform(PelvisBone->Bone.BoneIndex, PelvisTransformCS));

#if WITH_EDITOR
	if (bEnableDebugDraw)
	{
		FVector PelvisLocWorld = FAnimUtil::GetBoneWorldLocation(*SkelComp, Output.Pose, PelvisBone->Bone.BoneIndex);
		FTransform PelvisTarget(PelvisTransformCS);
		FAnimationRuntime::ConvertCSTransformToBoneSpace(SkelComp->GetComponentTransform(), Output.Pose, PelvisTarget, PelvisBone->Bone.BoneIndex, BCS_WorldSpace);

		FDebugDrawUtil::DrawSphere(World, PelvisLocWorld, FColor(255, 0, 0), 20.0f);

		if (bReturnToCenter)
		{
			FDebugDrawUtil::DrawSphere(World, PelvisTarget.GetLocation(), FColor(255, 255, 0), 20.0f);
			FDebugDrawUtil::DrawLine(World, PelvisLocWorld, PelvisTarget.GetLocation(), FColor(255, 255, 0));
			FVector TextOffset = FVector(0.0f, 0.0f, 100.0f);
			float AdjustHeight = (PelvisLocWorld - PelvisTarget.GetLocation()).Size();
			FString AdjustStr = FString::Printf(TEXT("Pelvis Offset: %f"), AdjustHeight);
			FDebugDrawUtil::DrawString(World, TextOffset, AdjustStr, Character, FColor(255, 255, 0));
		}
		else
		{
			FDebugDrawUtil::DrawSphere(World, PelvisTarget.GetLocation(), FColor(0, 0, 255), 20.0f);
			FDebugDrawUtil::DrawLine(World, PelvisLocWorld, PelvisTarget.GetLocation(), FColor(0, 0, 255));
			FVector TextOffset = FVector(0.0f, 0.0f, 100.0f);
			float AdjustHeight = (PelvisLocWorld - PelvisTarget.GetLocation()).Size();
			FString AdjustStr = FString::Printf(TEXT("Pelvis Offset: %f"), AdjustHeight);
			FDebugDrawUtil::DrawString(World, TextOffset, AdjustStr, Character, FColor(0, 0, 255));
		}

		FVector LeftTraceWorld = LeftLegTraceData->GetTraceData().FootHitResult.ImpactPoint;
		FDebugDrawUtil::DrawSphere(World, LeftTraceWorld, FColor(0, 255, 0), 20.0f);

		FVector RightTraceWorld = RightLegTraceData->GetTraceData().FootHitResult.ImpactPoint;
		FDebugDrawUtil::DrawSphere(World, RightTraceWorld, FColor(255, 0, 0), 20.0f);

	}
#endif
}

