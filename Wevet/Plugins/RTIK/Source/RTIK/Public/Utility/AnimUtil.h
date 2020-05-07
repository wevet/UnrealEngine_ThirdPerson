
#pragma once

#include "CoreMinimal.h"
#include "BonePose.h"
#include "AnimationRuntime.h"
#include "Components/SkeletalMeshComponent.h"
#include "AnimUtil.generated.h"

USTRUCT()
struct FAnimUtil
{
	GENERATED_BODY()

public:

	static FVector GetBoneWorldLocation(USkeletalMeshComponent& SkelComp, FCSPose<FCompactPose>& MeshBases, FCompactPoseBoneIndex BoneIndex)
	{
		FTransform BoneTransform = MeshBases.GetComponentSpaceTransform(BoneIndex);
		FAnimationRuntime::ConvertCSTransformToBoneSpace(SkelComp.GetComponentTransform(), MeshBases, BoneTransform, BoneIndex, BCS_WorldSpace);
		return BoneTransform.GetLocation();
	}

	static FTransform GetBoneWorldTransform(USkeletalMeshComponent& SkelComp, FCSPose<FCompactPose>& MeshBases, FCompactPoseBoneIndex BoneIndex)
	{
		FTransform BoneTransform = MeshBases.GetComponentSpaceTransform(BoneIndex);
		FAnimationRuntime::ConvertCSTransformToBoneSpace(SkelComp.GetComponentTransform(), MeshBases, BoneTransform, BoneIndex, BCS_WorldSpace);
		return BoneTransform;
	}

	static FVector GetBoneCSLocation(USkeletalMeshComponent& SkelComp, FCSPose<FCompactPose>& MeshBases, FCompactPoseBoneIndex BoneIndex)
	{
		return MeshBases.GetComponentSpaceTransform(BoneIndex).GetLocation();
	}

	static FTransform GetBoneCSTransform(USkeletalMeshComponent& SkelComp, FCSPose<FCompactPose>& MeshBases, FCompactPoseBoneIndex BoneIndex)
	{
		return MeshBases.GetComponentSpaceTransform(BoneIndex);
	}

};
