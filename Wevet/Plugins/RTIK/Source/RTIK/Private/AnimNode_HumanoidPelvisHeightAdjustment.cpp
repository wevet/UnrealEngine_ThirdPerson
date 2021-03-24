
#include "AnimNode_HumanoidPelvisHeightAdjustment.h"
#include "RTIK.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationRuntime.h"
#include "IKFunctionLibrary.h"
#include "HumanoidIK.h"


// Profiler‚É“o˜^‚·‚é
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
		return;
	}

	if (LeftLegTraceData == nullptr || RightLegTraceData == nullptr)
	{
		return;
	}


	USkeletalMeshComponent* Component = Output.AnimInstanceProxy->GetSkelMeshComponent();
	ACharacter* Character = Cast<ACharacter>(Component->GetOwner());
	if (Character == nullptr)
	{
		return;
	}

	UWorld* World = Character->GetWorld();
	bool bReturnToCenter = false;
	float TargetPelvisDelta = 0.0f;
	if (LeftLegTraceData->GetTraceData().FootHitResult.GetActor() == nullptr && 
		RightLegTraceData->GetTraceData().FootHitResult.GetActor() == nullptr)
	{
		bReturnToCenter = true;
	}
	else
	{
		FVector LeftFootFloorCS;
		FVector RightFootFloorCS;
		const FVector ToCS = -1 * Component->GetComponentLocation();
		const FVector LeftFootCS = UIKFunctionLibrary::GetBoneCSLocation(Output.Pose, LeftLeg->GetChain().ShinBone.BoneIndex);
		const FVector RightFootCS = UIKFunctionLibrary::GetBoneCSLocation(Output.Pose, RightLeg->GetChain().ShinBone.BoneIndex);
		const FVector RootCS = UIKFunctionLibrary::GetBoneCSLocation(Output.Pose, FCompactPoseBoneIndex(0));
		LeftLeg->GetChain().GetIKFloorPointCS(*Component, LeftLegTraceData->GetTraceData(), LeftFootFloorCS);
		RightLeg->GetChain().GetIKFloorPointCS(*Component, RightLegTraceData->GetTraceData(), RightFootFloorCS);
		TargetPelvisDelta = (LeftFootFloorCS.Z < RightFootFloorCS.Z) ? LeftFootFloorCS.Z - RootCS.Z : RightFootFloorCS.Z - RootCS.Z;

		//
		if (FMath::Abs(TargetPelvisDelta) > MaxPelvisAdjustSize)
		{
			bReturnToCenter = true;
			TargetPelvisDelta = 0.0f;
		}
	}

	FTransform PelvisTransformCS = UIKFunctionLibrary::GetBoneCSTransform(Output.Pose, PelvisBone->Bone.BoneIndex);
	const FVector TargetPelvisDeltaVec = FVector(0.0f, 0.0f, TargetPelvisDelta);
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
		const FVector PelvisLocWorld = UIKFunctionLibrary::GetBoneWorldLocation(*Component, Output.Pose, PelvisBone->Bone.BoneIndex);
		FTransform PelvisTarget(PelvisTransformCS);
		FAnimationRuntime::ConvertCSTransformToBoneSpace(Component->GetComponentTransform(), Output.Pose, PelvisTarget, PelvisBone->Bone.BoneIndex, BCS_WorldSpace);
		UIKFunctionLibrary::DrawSphere(World, PelvisLocWorld, FColor(255, 0, 0), 20.0f);

		const FColor BaseColor = bReturnToCenter ? FColor(255, 255, 0) : FColor(0, 0, 255);
		UIKFunctionLibrary::DrawSphere(World, PelvisTarget.GetLocation(), BaseColor, 20.0f);
		UIKFunctionLibrary::DrawLine(World, PelvisLocWorld, PelvisTarget.GetLocation(), BaseColor);

		const FVector TextOffset = FVector(0.0f, 0.0f, 100.0f);
		const float AdjustHeight = (PelvisLocWorld - PelvisTarget.GetLocation()).Size();
		FString AdjustStr = FString::Printf(TEXT("Pelvis Offset: %f"), AdjustHeight);
		UIKFunctionLibrary::DrawString(World, TextOffset, AdjustStr, Character, BaseColor);

		const FVector LeftTraceWorld = LeftLegTraceData->GetTraceData().FootHitResult.ImpactPoint;
		const FVector RightTraceWorld = RightLegTraceData->GetTraceData().FootHitResult.ImpactPoint;
		UIKFunctionLibrary::DrawSphere(World, LeftTraceWorld, FColor(0, 255, 0), 20.0f);
		UIKFunctionLibrary::DrawSphere(World, RightTraceWorld, FColor(255, 0, 0), 20.0f);

	}
#endif
}

