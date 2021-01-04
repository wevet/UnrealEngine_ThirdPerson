// Copyright 2018 wevet works All Rights Reserved.

#include "HumanoidIK.h"
#include "RTIK.h"
#include "AnimUtil.h"
#include "TraceUtil.h"


void FHumanoidIK::HumanoidIKLegTrace(ACharacter* Character, FCSPose<FCompactPose>& MeshBases, FHumanoidLegChain& LegChain, FIKBone& PelvisBone, float MaxPelvisAdjustHeight, FHumanoidIKTraceData& OutTraceData, bool bEnableDebugDraw)
{
	if (Character == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkelComp = Character->GetMesh();
	UWorld* World = Character->GetWorld();
	const FBoneContainer& RequiredBones = MeshBases.GetPose().GetBoneContainer();
	const FVector TraceDirection = -1 * Character->GetActorUpVector();

	const FVector PelvisLocation = FAnimUtil::GetBoneCSLocation(*SkelComp, MeshBases, PelvisBone.BoneIndex);
	const FVector FootLocation = FAnimUtil::GetBoneCSLocation(*SkelComp, MeshBases, LegChain.ShinBone.BoneIndex);
	const FVector ToeLocation = FAnimUtil::GetBoneCSLocation(*SkelComp, MeshBases, LegChain.FootBone.BoneIndex);
	const float TraceStartHeight = FMath::Max3(FootLocation.Z + LegChain.FootRadius, ToeLocation.Z + LegChain.ToeRadius, PelvisLocation.Z);
	const float TraceEndHeight = PelvisLocation.Z - (LegChain.GetTotalChainLength() + LegChain.FootRadius + LegChain.ToeRadius + MaxPelvisAdjustHeight);
	FVector FootTraceStart(FootLocation.X, FootLocation.Y, TraceStartHeight);
	FVector FootTraceEnd(FootLocation.X, FootLocation.Y, TraceEndHeight);
	FVector ToeTraceStart(ToeLocation.X, ToeLocation.Y, TraceStartHeight);
	FVector ToeTraceEnd(ToeLocation.X, ToeLocation.Y, TraceEndHeight);

	const FTransform ComponentToWorld = SkelComp->GetComponentToWorld();
	FootTraceStart = ComponentToWorld.TransformPosition(FootTraceStart);
	FootTraceEnd = ComponentToWorld.TransformPosition(FootTraceEnd);
	ToeTraceStart = ComponentToWorld.TransformPosition(ToeTraceStart);
	ToeTraceEnd = ComponentToWorld.TransformPosition(ToeTraceEnd);
	UTraceUtil::LineTrace(World, Character, FootTraceStart, FootTraceEnd, OutTraceData.FootHitResult, ECC_Pawn, false, bEnableDebugDraw);
	UTraceUtil::LineTrace(World, Character, ToeTraceStart, ToeTraceEnd, OutTraceData.ToeHitResult, ECC_Pawn, false, bEnableDebugDraw);
}

