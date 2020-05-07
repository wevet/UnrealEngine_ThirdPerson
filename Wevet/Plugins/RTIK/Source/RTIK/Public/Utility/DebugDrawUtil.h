#pragma once

#include "Math/Color.h"
#include "Math/Vector.h"
#include "GameFramework/Actor.h"
#include "BonePose.h"
#include "DebugDrawUtil.generated.h"

USTRUCT()
struct FDebugDrawUtil
{
	GENERATED_USTRUCT_BODY()

public: 
	static void DrawLine(UWorld* World, const FVector& Start, const FVector& Finish, const FLinearColor& Color= FColor(0, 255, 0), float Duration = -1.0f, float Thickness = 1.0f);
	static void DrawSphere(UWorld* World, const FVector& Center, const FLinearColor& Color = FColor(0, 255, 0), float Radius = 15.0f, int32 Segments = 12, float Duration = -1.0f, float Thickness = 0.75f);
	static void DrawString(UWorld* World, const FVector& Location, const FString& Text, AActor* BaseActor, const FColor& Color, float Duration = 0.0f);
	//static void DrawPlane(UWorld * World, const FVector& PlaneBase, const FVector& PlaneNormal, float Size = 100.0f, const FLinearColor& Color = FColor(255, 0, 255, 90), bool bDrawNormal = true, float Duration = -1.0f);

	static void DrawVector(
		UWorld* World, 
		const FVector& Base, 
		FVector Direction,
		const FLinearColor& Color = FColor(255, 0, 0), 
		float Length = 50.0f, 
		float Duration = -1.0f, 
		float Thickness = 1.0f);

	static void DrawBone(
		UWorld * World, 
		USkeletalMeshComponent& SkelComp, 
		FCSPose<FCompactPose>& Pose,
		const FCompactPoseBoneIndex& ChildBone, 
		const FLinearColor& Color = FColor(0, 255, 255), 
		float Duration = -1.0f, 
		float Thickness = 1.0f);

	static void DrawBoneChain(
		UWorld* World, 
		USkeletalMeshComponent& SkelComp, 
		FCSPose<FCompactPose>& Pose,
		const FCompactPoseBoneIndex & ChainStartChild, 
		const FCompactPoseBoneIndex& ChainEndParent,
		const FLinearColor& Color = FColor(0, 255, 255), 
		float Duration = -1.0f, 
		float Thickness = 1.0f);
};
