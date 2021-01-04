
#include "AnimNode_HumanoidFootRotationController.h"
#include "RTIK.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationRuntime.h"
#include "AnimUtil.h"

#if WITH_EDITOR
#include "DebugDrawUtil.h"
#endif

DECLARE_CYCLE_STAT(TEXT("IK Humanoid Foot Rotation Controller  Eval"), STAT_HumanoidFootRotationController_Eval, STATGROUP_Anim);

void FAnimNode_HumanoidFootRotationController::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_HumanoidFootRotationController_Eval);

#if ENABLE_ANIM_DEBUG
	check(Output.AnimInstanceProxy->GetSkelMeshComponent());
#endif
	check(OutBoneTransforms.Num() == 0);

	// Input pin pointers are checked in IsValid -- don't need to check here
	USkeletalMeshComponent* SkelComp = Output.AnimInstanceProxy->GetSkelMeshComponent();

	float RequiredRad = 0.0f;
	FQuat TargetOffset = FQuat::Identity;

	const bool bTargetRotationWithinLimit = Leg->Chain.FindWithinFootRotationLimit(*SkelComp, TraceData->GetTraceData(), RequiredRad);

	if (bTargetRotationWithinLimit)
	{
		// Compute required rotation
		const FMatrix ToCS = SkelComp->GetComponentToWorld().ToMatrixNoScale().Inverse();

		const FVector FootFloor = TraceData->GetTraceData().FootHitResult.ImpactPoint;
		const FVector ToeFloor = TraceData->GetTraceData().ToeHitResult.ImpactPoint;
		const FVector FloorSlopeVec = ToCS.TransformVector(ToeFloor - FootFloor);

		FVector FloorFlatVec(FloorSlopeVec);
		FloorFlatVec.Z = 0.0f;

		const FVector KneeCS = FAnimUtil::GetBoneCSLocation(*SkelComp, Output.Pose, Leg->Chain.ThighBone.BoneIndex);
		const FVector FootCS = FAnimUtil::GetBoneCSLocation(*SkelComp, Output.Pose, Leg->Chain.ShinBone.BoneIndex);
		const FVector ToeCS = FAnimUtil::GetBoneCSLocation(*SkelComp, Output.Pose, Leg->Chain.FootBone.BoneIndex);
		const FVector ShinVec = KneeCS - FootCS;
		const FVector FootVec = ToeCS - FootCS;

		// Ç¬Ç‹êÊÇÃäOêœ
		FVector RotationAxis = FVector::CrossProduct(FootVec, ShinVec);
		const FVector CrossUnsafeNormal = FVector::CrossProduct(FloorFlatVec, FloorSlopeVec).GetUnsafeNormal();

		if (RotationAxis.Normalize())
		{
			// Ç¬Ç‹êÊÇÃì‡êœ
			if (FVector::DotProduct(RotationAxis, CrossUnsafeNormal) < 0.0f)
			{
				RotationAxis *= -1.0f;
			}
			TargetOffset = FQuat(RotationAxis, RequiredRad);
		}
	}

	// Interpolate to target rotation and apply 
	FTransform FootCSTransform = FAnimUtil::GetBoneCSTransform(*SkelComp, Output.Pose, Leg->Chain.ShinBone.BoneIndex);
	const FQuat Result = FQuat::Slerp(LastRotationOffset, TargetOffset, FMath::Clamp(RotationSlerpSpeed * DeltaTime, 0.0f, 1.0f));

	LastRotationOffset = bInterpolateRotation ? Result : TargetOffset;
	FootCSTransform.SetRotation(LastRotationOffset * FootCSTransform.GetRotation());
	OutBoneTransforms.Add(FBoneTransform(Leg->Chain.ShinBone.BoneIndex, FootCSTransform));

#if WITH_EDITOR
	if (bEnableDebugDraw)
	{
		UWorld* World = SkelComp->GetWorld();
		ACharacter* Character = Cast<ACharacter>(SkelComp->GetOwner());
		FMatrix ToWorld = SkelComp->GetComponentToWorld().ToMatrixNoScale();
		if (bTargetRotationWithinLimit)
		{
			FDebugDrawUtil::DrawLine(World, TraceData->GetTraceData().FootHitResult.ImpactPoint, TraceData->GetTraceData().ToeHitResult.ImpactPoint, FColor(0, 255, 0));
			FVector TextOffset(0.0f, 0.0f, 100.0f);
			FString AngleStr = FString::Printf(TEXT("Floor angle (deg): %f"), FMath::RadiansToDegrees(RequiredRad));
			FDebugDrawUtil::DrawString(World, TextOffset, AngleStr, Character, FColor(0, 255, 0));
		}
		else
		{
			FDebugDrawUtil::DrawLine(World, TraceData->GetTraceData().FootHitResult.ImpactPoint, TraceData->GetTraceData().ToeHitResult.ImpactPoint, FColor(255, 0, 0));
			FVector TextOffset(0.0f, 0.0f, 100.0f);
			FString AngleStr = FString::Printf(TEXT("Floor angle (deg): %f"), FMath::RadiansToDegrees(RequiredRad));
			FDebugDrawUtil::DrawString(World, TextOffset, AngleStr, Character, FColor(255, 0, 0));
		}
	}
#endif

}
