#pragma once

#include "CoreMinimal.h"
#include "RTIK.h"
#include "HumanoidIK.h"


// Range-limited FABRIK solvers and related functions. Does not need to be in the context of a Skeleton; 
// this solver is designed to work with generic transforms. Make sure all transforms are in the same space, though!
//
// See in FABRIK algorithm.
// ttp://www.andreasaristidou.com/FABRIK.html
// 2つのノイズの多いエフェクターを含む3点閉ループ。
// これは非常に特殊なタイプのIK「チェーン」であり、胴体の上半身と下半身の三角形に使用される
struct RTIK_API FNoisyThreePointClosedLoop
{
public:

	FNoisyThreePointClosedLoop(
		const FTransform InEffectorATransform, const FTransform InEffectorBTransform, const FTransform InRootTransform,
		float InTargetRootADistance, float InTargetRootBDistance, float InTargetABDistance)
	{
		RootTransform = InRootTransform;
		EffectorATransform = InEffectorATransform;
		EffectorBTransform = InEffectorBTransform;
		TargetRootADistance = InTargetRootADistance;
		TargetRootBDistance = InTargetRootBDistance;
		TargetABDistance = InTargetABDistance;
	}

	FNoisyThreePointClosedLoop()
	{ }

public:
	FTransform EffectorATransform;
	FTransform EffectorBTransform;
	FTransform RootTransform;

	float TargetRootADistance;
	float TargetRootBDistance;
	float TargetABDistance;
};


struct RTIK_API FRangeLimitedFABRIK
{
public:

	// Use the FABRIK algorithm to solve IK problems on a chain of tightly connected points.
	// InTransforms is a list of transformations that represent the starting position of each point. 0th element
	// ROOT in this array is ROOT. The last element is EFFECTOR (sometimes called a "tip" in the code).
	// 
	// The root point represents the start of the chain. Displacement from the starting position is limited.
	// The effector represents the end of the chain and FABRIK will try to move it to the EffectorTargetLocation.
	// While maintaining the distance between points.
	// 
	// Each chain point except the effector has a CHILD-this is just the next point in the chain. Correspondingly
	// Each point except the root has a previous point, PARENT. Displacement between parent and child
	// Roughly called BONE.
	// 
	// FABRIK is an iterative algorithm. Adjust each point in the chain back and forth.
	// Until the distance between the effector and the EffectorTargetLocation is less than Precision or MaxIterations
	// Iteration (up and down the chain) has been performed.
	// As a general rule, FABRIK pays attention only to the position of each chain point, not to rotation. However,
	// This is not practical when dealing with skeletal bones. The final location of each chain point
	// Determined, the rotation of each transformation is updated as follows:
	// 
	// Before -FABRIK, let P be a non-effector chainpoint and C be its child.
	// After -FABRIK is applied, make P'and C'the corresponding chain points
	// Let -Q be the shortest rotation from (C-P) to (C'-P')
	// -Add rotation Q to rotation of P'
	//
	// That is, the rotation of each point conversion is updated with the minimum rotation,
	// Refers to the'newly adjusted child point'.
	// Effector rotation is not updated (unlike the implementation of UE4 FABRIK). You need to do this
	// The parameter description is as follows:
	//
	// @param OutTransforms-Updated transformations for each chainpoint after running FABRIK. Must be an empty array.
	// @param InTransforms-Start transformation of each chain point. It has not changed. At least two transformations must be included.
	// @param Constraints-Constraints for each chain point. For points that do not require constraints, the entry must be set to nullptr. The constraint is applied each time the point moves.
	// @param EffectorTargetLocation-Effector destination. FABRIK tries to bring the effector as close to this point as possible.
	// @param MaxRootDragDistance-How far can the root move from its original position? If it doesn't work, set it to 0.
	// @param RootDragStiffness-How much resistance the root will move from its original position. 1.0 means that the resistance does not increase to increase the resistance. Settings less than 1.0 will move more.
	// @param Precision-If the effector is within this distance of the target, the iteration ends. Probably reduced for better results, but probably for worse performance.
	// @param MaxIterations-Maximum number of iterations to perform. Probably increased for better results, but probably for worse performance.
	// @param Character-A character pointer that can be used for debug drawing. Set it safely to nullptr or ignore it.
	// @return True if the OutTransforms transforms have been updated. Otherwise false. If false, the contents of OutTransforms are the same as InTransforms.
	static const bool SolveRangeLimitedFABRIK(
		TArray<FTransform>& OutTransforms,
		const TArray<FTransform> InTransforms, 
		const TArray<FIKBoneConstraint*> Constraints, 
		const FVector EffectorTargetLocation, 
		float MaxRootDragDistance = 0.0f, 
		float RootDragStiffness = 1.0f,
		float Precision = 0.01f,
		int32 MaxIterations = 20, 
		ACharacter * Character = nullptr)
	{
		if (Character == nullptr)
		{
			return false;
		}

		OutTransforms.Empty();
		// Number of points in the chain. Number of bones = NumPoints - 1
		const int32 NumPoints = InTransforms.Num();
		const int32 EffectorIndex = (NumPoints - 1);

		// Gather bone transforms
		OutTransforms.Reserve(NumPoints);
		for (const FTransform& Transform : InTransforms)
		{
			OutTransforms.Add(Transform);
		}

		// IKを実行するには少なくとも1つのボーンが必要
		if (NumPoints < 2)
		{
			return false;
		}

		// Collect bone lengths. BoneLengths contains the lengths of bones ending at this point
		// BoneLengths [i] contains the distance between point i-1 and point i
		TArray<float> BoneLengths;
		const float MaximumReach = ComputeBoneLengths(InTransforms, BoneLengths);
		bool bBoneLocationUpdated = false;

		// Check distance between tip location and effector location
		float Slop = (OutTransforms[EffectorIndex].GetLocation() - EffectorTargetLocation).Size();
		if (Slop > Precision)
		{
			OutTransforms[EffectorIndex].SetLocation(EffectorTargetLocation);
			int32 IterationCount = 0;
			while ((Slop > Precision) && (IterationCount++ < MaxIterations))
			{
				FABRIKForwardPass(InTransforms, Constraints, BoneLengths, OutTransforms, Character);
				DragPointTethered(InTransforms[0], OutTransforms[1], BoneLengths[1], MaxRootDragDistance, RootDragStiffness, OutTransforms[0]);
				FABRIKBackwardPass(InTransforms, Constraints, BoneLengths, OutTransforms, Character);
				const float Distance = (OutTransforms[EffectorIndex - 1].GetLocation() - EffectorTargetLocation).Size();
				Slop = FMath::Abs(BoneLengths[EffectorIndex] - Distance);
			}

			// Place effector based on how close we got to the target
			FVector EffectorLocation = OutTransforms[EffectorIndex].GetLocation();
			const FVector EffectorParentLocation = OutTransforms[EffectorIndex - 1].GetLocation();
			EffectorLocation = EffectorParentLocation + (EffectorLocation - EffectorParentLocation).GetUnsafeNormal() * BoneLengths[EffectorIndex];
			OutTransforms[EffectorIndex].SetLocation(EffectorLocation);
			bBoneLocationUpdated = true;
		}

		// Update bone rotations
		if (bBoneLocationUpdated)
		{
			for (int32 Index = 0; Index < EffectorIndex; ++Index)
			{
				if (!FMath::IsNearlyZero(BoneLengths[Index + 1]))
				{
					UpdateParentRotation(OutTransforms[Index], InTransforms[Index], OutTransforms[Index + 1], InTransforms[Index + 1]);
				}
			}
		}
		return bBoneLocationUpdated;
	}


	// Solves FABRIK on a CLOSED LOOP, that is, a chain where the effector is assumed to be connected to the root.
	// Note that you will probably HAVE to use root dragging if you want this solver to work! If the root is not allowed to drag,
	// the solution degenerates and this solver will behave like normal FABRIK.	
	// I'm not sure how well constraints will work with this solver, but they are nonetheless supported for the sake of
	// keeping a consistent interface. You're welcome to try them out but results may be bad.	
	//
	// @param OutTransforms - The updated transforms for each chain point after FABRIK runs. Should be an empty array. Will be emptied and filled with new transforms.  
	// @param InTransforms - The starting transforms of each chain point. Not modified. Must contain at least 2 transforms. Because this is a closed loop, the last transform is assumed to be conneted to the first one.	
	// @param Constraints - Constraints corresponding to each chain point; entries should be set to nullptr for points that don't need a constraint. Constraints are enforced each time a point moves. Strong constraints may degrade the results of FABRIK, it's up to you to figure out what works.
	// @param EffectorTargetLocation - Where you want the effector to go. FABRIK will attempt to move the effector as close as possible to this point.
	// @param MaxRootDragDistance - How far the root may move from its original position. Set to 0 for no movement.
	// @param RootDragStiffness - How much the root will resist being moved from the original position. 1.0 means no resistance increase for more resistance. Settings less than 1.0 will make it move more.
	// @param Precision - Iteration will terminate when the effector is within this distance from the target. Decrease for possibly better results but possibly worse performance.
	// @param MaxIterations - The maximum number of iterations to run. Increase for possibly better results but possibly worse performance.
	// @param Character - Character pointer whcih may be used for debug drawing. May safely be set to nullptr or ignored.
	// @return - True if any transforms in OutTransforms were updated; otherwise, false. If false, the contents of OutTransforms is identical to InTransforms.
	static const bool SolveClosedLoopFABRIK(
		TArray<FTransform>& OutTransforms,
		const TArray<FTransform> InTransforms, 
		const TArray<FIKBoneConstraint*> Constraints,
		const FVector EffectorTargetLocation, 
		float MaxRootDragDistance = 10.0f, 
		float RootDragStiffness = 1.0f, 
		float Precision = 0.01f, 
		int32 MaxIterations = 20, 
		ACharacter* Character = nullptr)
	{
		if (Character == nullptr)
		{
			return false;
		}

		OutTransforms.Empty();
		// Number of points in the chain. Number of bones = NumPoints - 1
		const int32 NumPoints = InTransforms.Num();
		const int32 EffectorIndex = (NumPoints - 1);

		// Gather bone transforms
		OutTransforms.Reserve(NumPoints);
		for (const FTransform& Transform : InTransforms)
		{
			OutTransforms.Add(Transform);
		}

		// IKを実行するには少なくとも1つのボーンが必要
		if (NumPoints < 2)
		{
			return false;
		}

		// Gather bone lengths. BoneLengths contains the length of the bone ENDING at this point,
		// i.e., BoneLengths[i] contains the distance between point i-1 and point i
		TArray<float> BoneLengths;
		const float MaximumReach = ComputeBoneLengths(InTransforms, BoneLengths);
		const float RootToEffectorLength = (InTransforms[0].GetLocation() - InTransforms[EffectorIndex].GetLocation()).Size();

		bool bBoneLocationUpdated = false;
		float Slop = (OutTransforms[EffectorIndex].GetLocation() - EffectorTargetLocation).Size();
		if (Slop > Precision)
		{
			// The closed loop method is identical, except the root is dragged a second time to maintain
			// distance with the effector.		
			// Set tip bone at end effector location.
			OutTransforms[EffectorIndex].SetLocation(EffectorTargetLocation);

			int32 IterationCount = 0;
			while ((Slop > Precision) && (IterationCount++ < MaxIterations))
			{
				FABRIKForwardPass(InTransforms, Constraints, BoneLengths, OutTransforms, Character);
				DragPointTethered(InTransforms[0], OutTransforms[1], BoneLengths[1], MaxRootDragDistance, RootDragStiffness, OutTransforms[0]);
				DragPointTethered(InTransforms[0], OutTransforms[EffectorIndex], RootToEffectorLength, MaxRootDragDistance, RootDragStiffness, OutTransforms[0]);
				FABRIKBackwardPass(InTransforms, Constraints, BoneLengths, OutTransforms, Character);
				Slop = FVector::Dist(OutTransforms[EffectorIndex].GetLocation(), EffectorTargetLocation);
			}
			bBoneLocationUpdated = true;
		}

		if (bBoneLocationUpdated)
		{
			for (int32 Index = 0; Index < EffectorIndex; ++Index)
			{
				if (!FMath::IsNearlyZero(BoneLengths[Index + 1]))
				{
					UpdateParentRotation(OutTransforms[Index], InTransforms[Index], OutTransforms[Index + 1], InTransforms[Index + 1]);
				}
			}

			// Update the last bone's rotation. Unlike normal fabrik, it's assumed to point toward the root bone,
			// so it's rotation must be updated
			if (!FMath::IsNearlyZero(RootToEffectorLength))
			{
				UpdateParentRotation(OutTransforms[EffectorIndex], InTransforms[EffectorIndex], OutTransforms[0], InTransforms[0]);
			}
		}
		return bBoneLocationUpdated;
	}


	// Runs closed-loop FABRIK multiple times, attempting to move both 'noisy effectors' to their targets.
	// Via ttp://www.andreasaristidou.com/publications/papers/FABRIK.pdf
	//
	// @param OutClosedLoop - Adjusted transforms of the closed loop points will be output here. 
	// @param InClosedLoop - Input closed loop; describes starting positions of the  closed loop
	// @param EffectorATarget - Target location for noisy effector A
	// @param EffectorBTarget - Target location for noisy effector B
	// @param MaxRootDragDistance - How far the root point may be dragged from its starting position
	// @param RootDragStiffness - How much the root resists being dragged. Set to 1.0 for no resistance; higher will resist dragging, lower will enhance dragging
	// @param Precision - Solver will stop iterating when both Effector A and Effector B moved less than this amount on the last iteration.
	// @param MaxIterations - The maximum number of iterations the solver may run
	// @param Character - Optional character pointer, used for debug drawing. 
	// @result True if at least on transform changed. This algorithm always changes the transforms, so it always returns true.
	static const bool SolveNoisyThreePoint(
		FNoisyThreePointClosedLoop& OutClosedLoop,
		const FNoisyThreePointClosedLoop InClosedLoop, 
		const FTransform EffectorAReference, 
		const FTransform EffectorBReference,
		float MaxRootDragDistance = 0.0f, 
		float RootDragStiffness = 1.0f, 
		float Precision = 0.01f, 
		int32 MaxIterations = 20, 
		ACharacter * Character = nullptr)
	{
		// Temporary transforms for each point
		FTransform A = InClosedLoop.EffectorATransform;
		FTransform B = InClosedLoop.EffectorBTransform;
		FTransform Root = InClosedLoop.RootTransform;

		// Compute bone lengths
		const float DistAToRoot = InClosedLoop.TargetRootADistance;
		const float DistBToRoot = InClosedLoop.TargetRootBDistance;
		const float DistAToB = InClosedLoop.TargetABDistance;
		const float DistARef = (A.GetLocation() - EffectorAReference.GetLocation()).Size();
		const float DistBRef = (B.GetLocation() - EffectorBReference.GetLocation()).Size();

		// Now start the noisy solver method. The idea here is that A, B, and Root are out of whack;
		// move them so inter-joint distances are satisfied again. Keep doing this until things settle down.
		// See www.andreasaristidou.com/publications/papers/Extending_FABRIK_with_Model_Cοnstraints.pdf Figure 9 for
		// description of each phase. Hopefully I'm implementing this right; unfortunatley the paper is vague
		FVector LastA = A.GetLocation();
		FVector LastB = B.GetLocation();
		int32 IterationCount = 0;

		// Phase 1 (Fig. 9 b): go around the loop
		DragPoint(Root, DistAToRoot, A);
		DragPoint(A, DistAToB, B);
		DragPointTethered(InClosedLoop.RootTransform, B, DistBToRoot, MaxRootDragDistance, RootDragStiffness, Root);
		DragPoint(Root, DistAToRoot, A);

		// Phase 2 (Fig. 9 c): Reset root and go other way
		Root.SetLocation(InClosedLoop.RootTransform.GetLocation());
		DragPoint(Root, DistBToRoot, B);
		DragPoint(B, DistAToB, A);

		// Phase 3 (Fig. 9 d): Drag both effectors such that their distances to reference points (outside the closed loop)
		// and distances from root are maintained
		DragPoint(Root, DistAToRoot, A);
		DragPoint(EffectorAReference, DistARef, A);
		DragPoint(Root, DistBToRoot, B);
		DragPoint(EffectorBReference, DistBRef, B);

		// Phase 4 (Fig. 9 b): Same as phase 1
		DragPoint(Root, DistAToRoot, A);
		DragPoint(A, DistAToB, B);
		DragPointTethered(InClosedLoop.RootTransform, B, DistBToRoot, MaxRootDragDistance, RootDragStiffness, Root);
		DragPoint(Root, DistAToRoot, A);

		// Phase 5 (Fig. 9 c): Same as phase 2, but don't reset root
		DragPoint(Root, DistBToRoot, B);
		DragPoint(B, DistAToB, A);

		const float PrecisionSq = Precision * Precision;
		float Delta = FMath::Max(FVector::DistSquared(A.GetLocation(), LastA), FVector::Dist(B.GetLocation(), LastB));
		LastA = A.GetLocation();
		LastB = B.GetLocation();

		// Iterate phases 3-5 only
		while ((Delta > PrecisionSq) && (IterationCount++ < MaxIterations))
		{
			// Phase 3
			DragPoint(Root, DistAToRoot, A);
			DragPoint(EffectorAReference, DistARef, A);
			DragPoint(Root, DistBToRoot, B);
			DragPoint(EffectorBReference, DistBRef, B);

			// Phase 4
			DragPoint(Root, DistAToRoot, A);
			DragPoint(A, DistAToB, B);
			DragPointTethered(InClosedLoop.RootTransform, B, DistBToRoot, MaxRootDragDistance, RootDragStiffness, Root);
			DragPoint(Root, DistAToRoot, A);

			// Phase 5
			DragPoint(Root, DistBToRoot, B);
			DragPoint(B, DistAToB, A);
			Delta = FMath::Max(FVector::DistSquared(A.GetLocation(), LastA), (B.GetLocation() - LastB).Size());
			LastA = A.GetLocation();
			LastB = B.GetLocation();
		}

		if (!FMath::IsNearlyZero(DistAToRoot))
		{
			UpdateParentRotation(Root, InClosedLoop.RootTransform, A, InClosedLoop.EffectorATransform);
		}
		if (!FMath::IsNearlyZero(DistAToB))
		{
			UpdateParentRotation(A, InClosedLoop.EffectorATransform, B, InClosedLoop.EffectorBTransform);
		}
		if (!FMath::IsNearlyZero(DistBToRoot))
		{
			UpdateParentRotation(B, InClosedLoop.EffectorBTransform, Root, InClosedLoop.RootTransform);
		}

		OutClosedLoop.EffectorATransform = A;
		OutClosedLoop.EffectorBTransform = B;
		OutClosedLoop.RootTransform = Root;
		return true;
	}

protected:
	static void FABRIKForwardPass(const TArray<FTransform> InTransforms, const TArray<FIKBoneConstraint*> Constraints, const TArray<float> BoneLengths, TArray<FTransform>& OutTransforms, ACharacter* Character)
	{
		const int32 NumPoints = InTransforms.Num();
		const int32 EffectorIndex = (NumPoints - 1);

		for (int32 Index = EffectorIndex - 1; Index > 0; --Index)
		{
			FTransform& CurrentPoint = OutTransforms[Index];
			FTransform& ChildPoint = OutTransforms[Index + 1];
			DragPoint(ChildPoint, BoneLengths[Index + 1], CurrentPoint);

			FIKBoneConstraint* CurrentConstraint = Constraints[Index - 1];
			if (CurrentConstraint == nullptr || !CurrentConstraint->WasEnabled())
			{
				continue;
			}
			//CurrentConstraint->SetupFn(Index - 1, InTransforms, Constraints, OutTransforms);
			CurrentConstraint->EnforceConstraint(Index - 1, InTransforms, Constraints, OutTransforms, Character);
		}
	}


	static void FABRIKBackwardPass(const TArray<FTransform> InTransforms, const TArray<FIKBoneConstraint*> Constraints, const TArray<float> BoneLengths, TArray<FTransform>& OutTransforms, ACharacter* Character)
	{
		const int32 NumPoints = InTransforms.Num();
		const int32 EffectorIndex = (NumPoints - 1);

		for (int32 Index = 1; Index < EffectorIndex; Index++)
		{
			FTransform& ParentPoint = OutTransforms[Index - 1];
			FTransform& CurrentPoint = OutTransforms[Index];
			DragPoint(ParentPoint, BoneLengths[Index], CurrentPoint);

			FIKBoneConstraint* CurrentConstraint = Constraints[Index - 1];
			if (CurrentConstraint == nullptr || !CurrentConstraint->WasEnabled())
			{
				continue;
			}
			//CurrentConstraint->SetupFn(Index - 1, InTransforms, Constraints, OutTransforms);
			CurrentConstraint->EnforceConstraint(Index - 1, InTransforms, Constraints, OutTransforms, Character);
		}
	}


	static void UpdateParentRotation(FTransform& OutNewParent, const FTransform OldParent, const FTransform NewChild, const FTransform OldChild)
	{
		const FVector OldDir = (OldChild.GetLocation() - OldParent.GetLocation()).GetUnsafeNormal();
		const FVector NewDir = (NewChild.GetLocation() - OutNewParent.GetLocation()).GetUnsafeNormal();
		const FVector RotationAxis = FVector::CrossProduct(OldDir, NewDir).GetSafeNormal();
		const float RotationAngle = FMath::Acos(FVector::DotProduct(OldDir, NewDir));
		const FQuat DeltaRotation = FQuat(RotationAxis, RotationAngle);
		OutNewParent.SetRotation(DeltaRotation * OldParent.GetRotation());
		OutNewParent.NormalizeRotation();
	}


	// PointToMove onto the vector between itself and MaintainDistancePoint, 
	// such that the distance between them is BoneLength. This enforces the core FABRIK constraint, that inter-point distances don't change. 
	// Thus, PointToMove is 'dragged' toward MaintainDistancePoint and the original interpoint
	// distance is maintained.
	static void DragPoint(const FTransform MaintainDistancePoint, const float BoneLength, FTransform& OutTransform)
	{
		OutTransform.SetLocation(MaintainDistancePoint.GetLocation() + (OutTransform.GetLocation() - MaintainDistancePoint.GetLocation()).GetUnsafeNormal() * BoneLength);
	}


	// Drags PointToDrag relative to MaintainDistancePoint. that is, PointToDrag is moved so that it attempts to maintain the distance BoneLength between itself and MaintainDistancePoint. 
	// However, PointToDrag is 'tethered' to TetherPoint it cannot ever be dragged father than MaxDragDistance from TetherPoint. 
	// Additionally, the tether acts like a spring, 
	// instantaneously pulling MaintainDistancePoint back toward TetherPoint by an amount dictated by DragStiffness
	// specifically, the required displacement (before clamping to MaxDragDisplacement) is divided by DragStiffness.
	// Basically, this is like EnforcePointDistance, but with the additional stronger constraint that PointToDrag
	// can never be moved father than MaxDragDistance from StartingTransform.
	static void DragPointTethered(
		const FTransform StartingTransform, 
		const FTransform MaintainDistancePoint,
		const float BoneLength, 
		const float MaxDragDistance,
		const float DragStiffness,
		FTransform& OutTransform)
	{
		// 1.e-4f = 1×10−4 = 0.0001f
		if (MaxDragDistance < KINDA_SMALL_NUMBER || DragStiffness < KINDA_SMALL_NUMBER)
		{
			OutTransform = StartingTransform;
			return;
		}

		const FVector Normal = (OutTransform.GetLocation() - MaintainDistancePoint.GetLocation()).GetUnsafeNormal();
		const FVector Location = MaintainDistancePoint.GetLocation();
		const FVector Target = FMath::IsNearlyZero(BoneLength) ? Location : Location + Normal * BoneLength;

		FVector Displacement = Target - StartingTransform.GetLocation();
		Displacement /= DragStiffness;
		const FVector LimitedDisplacement = Displacement.GetClampedToMaxSize(MaxDragDistance);
		OutTransform.SetLocation(StartingTransform.GetLocation() + LimitedDisplacement);
	}


	static const float ComputeBoneLengths(const TArray<FTransform> InTransforms, TArray<float>& OutBoneLengths)
	{
		int32 NumPoints = InTransforms.Num();
		float MaximumReach = 0.0f;
		OutBoneLengths.Empty();
		OutBoneLengths.Reserve(NumPoints);
		OutBoneLengths.Add(0.0f);

		for (int32 i = 1; i < NumPoints; ++i)
		{
			OutBoneLengths.Add((InTransforms[i - 1].GetLocation() - InTransforms[i].GetLocation()).Size());
			MaximumReach += OutBoneLengths[i];
		}
		return MaximumReach;
	}
};
