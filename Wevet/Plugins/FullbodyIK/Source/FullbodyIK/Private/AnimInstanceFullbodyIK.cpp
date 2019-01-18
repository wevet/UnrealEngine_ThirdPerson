#include "AnimInstanceFullbodyIK.h"

UAnimInstanceFullbodyIK::UAnimInstanceFullbodyIK(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAnimInstanceFullbodyIK::InitializeBoneOffset_Implementation(int32 BoneIndex)
{
	if (!OffsetLocations.Contains(BoneIndex))
	{
		OffsetLocations.Add(BoneIndex, FVector::ZeroVector);
	}
	if (!OffsetRotations.Contains(BoneIndex))
	{
		OffsetRotations.Add(BoneIndex, FRotator::ZeroRotator);
	}
}

void UAnimInstanceFullbodyIK::SetBoneLocationOffset_Implementation(int32 BoneIndex, const FVector& Location)
{
	if (OffsetLocations.Contains(BoneIndex))
	{
		OffsetLocations[BoneIndex] = Location;
	}
}

FVector UAnimInstanceFullbodyIK::GetBoneLocationOffset_Implementation(int32 BoneIndex) const
{
	if (OffsetLocations.Contains(BoneIndex))
	{
		return OffsetLocations[BoneIndex];
	}
	return FVector::ZeroVector;
}

void UAnimInstanceFullbodyIK::SetBoneRotationOffset_Implementation(int32 BoneIndex, const FRotator& Rotation)
{
	if (OffsetRotations.Contains(BoneIndex))
	{
		OffsetRotations[BoneIndex] = Rotation;
	}
}

FRotator UAnimInstanceFullbodyIK::GetBoneRotationOffset_Implementation(int32 BoneIndex) const
{
	if (OffsetRotations.Contains(BoneIndex))
	{
		return OffsetRotations[BoneIndex];
	}
	return FRotator::ZeroRotator;
}
