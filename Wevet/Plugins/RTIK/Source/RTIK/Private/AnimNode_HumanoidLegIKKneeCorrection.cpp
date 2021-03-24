
#include "AnimNode_HumanoidLegIKKneeCorrection.h"
#include "RTIK.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationRuntime.h"
#include "IKFunctionLibrary.h"


// ProfilerÇ…ìoò^Ç∑ÇÈ
DECLARE_CYCLE_STAT(TEXT("IK Humanoid Knee Correction Eval"), STAT_HumanoidLegIKKneeCorrection_Eval, STATGROUP_Anim);

// ïGÇÃäpìxÇä«óùÇ∑ÇÈIK Class
void FAnimNode_HumanoidLegIKKneeCorrection::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_HumanoidLegIKKneeCorrection_Eval);

#if ENABLE_ANIM_DEBUG
	check(Output.AnimInstanceProxy->GetSkelMeshComponent());
#endif
	check(OutBoneTransforms.Num() == 0);

	USkeletalMeshComponent* SkelComp = Output.AnimInstanceProxy->GetSkelMeshComponent();
	FComponentSpacePoseContext BasePose(Output);
	BaseComponentPose.EvaluateComponentSpace(BasePose);

	const FVector HipCSPre = UIKFunctionLibrary::GetBoneCSLocation(BasePose.Pose, Leg->GetChain().HipBone.BoneIndex);
	const FVector KneeCSPre = UIKFunctionLibrary::GetBoneCSLocation(BasePose.Pose, Leg->GetChain().ThighBone.BoneIndex);
	const FVector FootCSPre = UIKFunctionLibrary::GetBoneCSLocation(BasePose.Pose, Leg->GetChain().ShinBone.BoneIndex);
	const FVector ToeCSPre = UIKFunctionLibrary::GetBoneCSLocation(BasePose.Pose, Leg->GetChain().FootBone.BoneIndex);
	const FVector HipCSPost = UIKFunctionLibrary::GetBoneCSLocation(Output.Pose, Leg->GetChain().HipBone.BoneIndex);
	const FVector KneeCSPost = UIKFunctionLibrary::GetBoneCSLocation(Output.Pose, Leg->GetChain().ThighBone.BoneIndex);
	const FVector FootCSPost = UIKFunctionLibrary::GetBoneCSLocation(Output.Pose, Leg->GetChain().ShinBone.BoneIndex);
	const FVector ToeCSPost = UIKFunctionLibrary::GetBoneCSLocation(Output.Pose, Leg->GetChain().FootBone.BoneIndex);
	const FVector OldThighVec = (KneeCSPost - HipCSPost).GetUnsafeNormal();
	const FVector OldShinVec = (FootCSPost - KneeCSPost).GetUnsafeNormal();


	if (FMath::IsNearlyEqual(FMath::Abs(FVector::DotProduct(OldThighVec, OldShinVec)), 1.0f))
	{
		return;
	}

	FVector HipFootAxisPre = FootCSPre - HipCSPre;
	if (!HipFootAxisPre.Normalize())
	{
		HipFootAxisPre = FVector(0.0f, 0.0f, 1.0f);
	}


	const FVector CenterPre = HipCSPre + (KneeCSPre - HipCSPre).ProjectOnToNormal(HipFootAxisPre);
	const FVector KneeDirectionPre = (KneeCSPre - CenterPre).GetUnsafeNormal();
	FVector HipFootAxisPost = FootCSPost - HipCSPost;
	if (!HipFootAxisPost.Normalize())
	{
		HipFootAxisPost = FVector(0.0f, 0.0f, 1.0f);
	}


	const FVector CenterPost = HipCSPost + (KneeCSPost - HipCSPost).ProjectOnToNormal(HipFootAxisPost);
	const FVector KneeDirectionPost = (KneeCSPost - CenterPost).GetUnsafeNormal();
	FVector FootToePre = FVector::VectorPlaneProject((ToeCSPre - FootCSPre), HipFootAxisPre);
	if (!FootToePre.Normalize())
	{
		FootToePre = KneeDirectionPre;
	}


	const float HipAxisRad = FMath::Acos(FVector::DotProduct(HipFootAxisPre, HipFootAxisPost));
	FVector FootToeRotationAxis = FVector::CrossProduct(HipFootAxisPre, HipFootAxisPost);
	FVector FootCSPostRotated = FootCSPost;
	FVector ToeCSPostRotated = ToeCSPost;
	if (FootToeRotationAxis.Normalize())
	{
		FQuat FootToeRotation(FootToeRotationAxis, HipAxisRad);
		const FVector FootDirection = FootCSPost - HipCSPost;
		const FVector ToeDirection = ToeCSPost - HipCSPost;
		FootCSPostRotated = HipCSPost + FootToeRotation.RotateVector(FootDirection);
		ToeCSPostRotated = HipCSPost + FootToeRotation.RotateVector(ToeDirection);
	}


	FVector FootToePost = FVector::VectorPlaneProject((ToeCSPostRotated - FootCSPostRotated), HipFootAxisPost);
	if (!FootToePost.Normalize())
	{
		FootToePost = KneeDirectionPost;
	}


	const FVector KneePre = (KneeCSPre - CenterPre).GetUnsafeNormal();
	float FootKneeRad = FMath::Acos(FVector::DotProduct(FootToePre, KneePre));
	FVector RotationAxis = FVector::CrossProduct(FootToePre, KneePre);
	if (!RotationAxis.Normalize())
	{
		RotationAxis = HipFootAxisPost;
		FootKneeRad = (FVector::DotProduct(FootToePre, KneePre) < 0.0f) ? 180.f : 0.0f;
	}

	const FQuat FootKneeRotPost = FQuat(RotationAxis, FootKneeRad);
	const FVector NewKneeDirection = FootKneeRotPost.RotateVector(FootToePost);
	const FVector NewKneeCS = CenterPost + (NewKneeDirection * (KneeCSPost - CenterPost).Size());
	const FVector NewThighVec = (NewKneeCS - HipCSPost).GetUnsafeNormal();
	const FVector NewShinVec = (FootCSPost - NewKneeCS).GetUnsafeNormal();
	const FQuat NewHipRotation = FQuat::FindBetweenNormals(OldThighVec, NewThighVec);
	const FQuat NewThighRotation = FQuat::FindBetweenNormals(OldShinVec, NewShinVec);
	FTransform NewHipTransform = UIKFunctionLibrary::GetBoneCSTransform(Output.Pose, Leg->GetChain().HipBone.BoneIndex);
	FTransform NewThighTransform = UIKFunctionLibrary::GetBoneCSTransform(Output.Pose, Leg->GetChain().ThighBone.BoneIndex);
	NewHipTransform.SetRotation(NewHipRotation * NewHipTransform.GetRotation());
	NewThighTransform.SetRotation(NewThighRotation * NewThighTransform.GetRotation());
	NewThighTransform.SetLocation(NewKneeCS);
	const FTransform NewShinTransform = UIKFunctionLibrary::GetBoneCSTransform(Output.Pose, Leg->GetChain().ShinBone.BoneIndex);

	OutBoneTransforms.Add(FBoneTransform(Leg->GetChain().HipBone.BoneIndex, NewHipTransform));
	OutBoneTransforms.Add(FBoneTransform(Leg->GetChain().ThighBone.BoneIndex, NewThighTransform));
	OutBoneTransforms.Add(FBoneTransform(Leg->GetChain().ShinBone.BoneIndex, NewShinTransform));

#if WITH_EDITOR
	if (bEnableDebugDraw)
	{
		UWorld* World = SkelComp->GetWorld();
		const FMatrix ToWorld = SkelComp->GetComponentToWorld().ToMatrixNoScale();
		UIKFunctionLibrary::DrawBoneChain(World, *SkelComp, BasePose.Pose, Leg->GetChain().FootBone.BoneIndex, Leg->GetChain().HipBone.BoneIndex, FColor(255, 0, 0));

		const FVector PrePlaneBase = ToWorld.TransformPosition(CenterPre);
		const FVector PrePlaneNormal = ToWorld.TransformVector(HipFootAxisPre);
		UIKFunctionLibrary::DrawVector(World, PrePlaneBase, ToWorld.TransformVector(KneePre), FColor(255, 0, 255));
		UIKFunctionLibrary::DrawVector(World, PrePlaneBase, ToWorld.TransformVector(FootToePre), FColor(255, 0, 0));
		UIKFunctionLibrary::DrawLine(World, ToWorld.TransformPosition(HipCSPre), ToWorld.TransformPosition(HipCSPre) + (PrePlaneNormal * (FootCSPre - HipCSPre).Size()), FColor(255, 0, 0));
		UIKFunctionLibrary::DrawBoneChain(World, *SkelComp, Output.Pose, Leg->GetChain().FootBone.BoneIndex, Leg->GetChain().HipBone.BoneIndex, FColor(0, 0, 255));

		const FVector PostPlaneBase = ToWorld.TransformPosition(CenterPost);
		const FVector PostPlaneNormal = ToWorld.TransformVector(HipFootAxisPost);
		UIKFunctionLibrary::DrawLine(World, ToWorld.TransformPosition(HipCSPost), ToWorld.TransformPosition(HipCSPost) + (PostPlaneNormal * (FootCSPost - HipCSPost).Size()), FColor(0, 255, 0));
		FCSPose<FCompactPose> CopiedPose;
		CopiedPose.CopyPose(Output.Pose);
		const float BlendWeight = FMath::Clamp<float>(ActualAlpha, 0.f, 1.f);
		CopiedPose.LocalBlendCSBoneTransforms(OutBoneTransforms, BlendWeight);
		
		UIKFunctionLibrary::DrawBoneChain(World, *SkelComp, CopiedPose, Leg->GetChain().FootBone.BoneIndex, Leg->GetChain().HipBone.BoneIndex, FColor(0, 255, 0));
		UIKFunctionLibrary::DrawVector(World, PostPlaneBase, ToWorld.TransformVector(NewKneeDirection), FColor(0, 255, 255));
		UIKFunctionLibrary::DrawVector(World, PostPlaneBase, ToWorld.TransformVector(FootToePost), FColor(0, 0, 255));
	}
#endif
}

