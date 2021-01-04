#pragma once

#include "CoreMinimal.h"
#include "RTIK.h"
#include "Constraints.h"


//	Range-limited FABRIK solvers and related functions. Does not need to be in the context of a Skeleton; 
//	this solver is designed to work with generic transforms. Make sure all transforms are in the same space, though!
//
//	See www.andreasaristidou.com/FABRIK.html for details of the FABRIK algorithm.
// A three-point closed loop, containing two noisy effectors.
// This is a very specific type of IK 'chain', used for the torso upper and lower body triangles.

struct RTIK_API FNoisyThreePointClosedLoop
{
public:

	FNoisyThreePointClosedLoop(
		const FTransform& InEffectorATransform,
		const FTransform& InEffectorBTransform,
		const FTransform& InRootTransform,
		float InTargetRootADistance,
		float InTargetRootBDistance,
		float InTargetABDistance)
		: EffectorATransform(InEffectorATransform),
		EffectorBTransform(InEffectorBTransform),
		RootTransform(InRootTransform),
		TargetRootADistance(InTargetRootADistance),
		TargetRootBDistance(InTargetRootBDistance),
		TargetABDistance(InTargetABDistance)
	{
		//
	}

	FNoisyThreePointClosedLoop()
	{ }

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

	// Uses the FABRIK algorithm to solve the IK problem on a chain of rigidly-connected points.	
	// InTransforms is a list of transforms representing the starting position of each point. The 0th element
	// of this array is the ROOT. The last element is the EFFECTOR (sometimes called the 'tip' in code).
	//
	// The root point represents the start of the chain; its displacement from its starting position is limited.
	// The effector represents the end of the chain, FABRIK will attempt to move this to EffectorTargetLocation,
	// while maintaining inter-point distances.
	//
	// Each chain point, except the effector, has a CHILD - this is simply the next point in the chain. Correspondingly,
	// each point except the root has a PARENT, which is the previous point. The displacement between a parent and its child 
	// is loosely referred to as a BONE.	
	// 
	// FABRIK is an iterative algorithm. It will adjust each point in the chain in a forward-and-backward manner,
	// until either the distance between the effector and EffectorTargetLocation is less than Precision, or MaxIterations
	// iterations (up and down the chain) have been performed.
	// In principal, FABRIK pays attention only to the location of each chain point, not their rotations. However, 
	// this is impractical when working with skeletal bones. Once the final locations of each chain point are 
	// determined, the rotation of each transform is updated as follows:
	// 
	// - Let P be a chain point which is not effector, and let C be its child, before FABRIK
	// - Let P' and C' be the corresponding chain points after FABRIK is applied
	// - Let Q be the shortest rotation from (C - P) to (C' - P')
	// - Add the rotation Q to the rotation of P'	
	//
	// In other words, the rotation of each point transform is updated with the smallest rotation to make it 
	// 'point toward' the newly adjusted child point.	
	// The effector's rotation is NOT updated (unlike in the UE4 FABRIK implementation), you will need to do this
	// yourself after running FABRIK.
	// Parameter descriptions follow:
	// 	
	// @param InTransforms - The starting transforms of each chain point. Not modified. Must contain at least 2 transforms.
	// @param Constraints - Constraints corresponding to each chain point; entries should be set to nullptr for points that don't need a constraint. Constraints are enforced each time a point moves. Strong constraints may degrade the results of FABRIK, it's up to you to figure out what works.
	// @param EffectorTargetLocation - Where you want the effector to go. FABRIK will attempt to move the effector as close as possible to this point.
	// @param OutTransforms - The updated transforms for each chain point after FABRIK runs. Should be an empty array. Will be emptied and filled with new transforms.  
	// @param MaxRootDragDistance - How far the root may move from its original position. Set to 0 for no movement.
	// @param RootDragStiffness - How much the root will resist being moved from the original position. 1.0 means no resistance increase for more resistance. Settings less than 1.0 will make it move more.
	// @param Precision - Iteration will terminate when the effector is within this distance from the target. Decrease for possibly better results but possibly worse performance.
	// @param MaxIterations - The maximum number of iterations to run. Increase for possibly better results but possibly worse performance.
	// @param Character - Character pointer whcih may be used for debug drawing. May safely be set to nullptr or ignored.
	// @return - True if any transforms in OutTransforms were updated; otherwise, false. If false, the contents of OutTransforms is identical to InTransforms.
	static bool SolveRangeLimitedFABRIK(
		const TArray<FTransform>& InTransforms, 
		const TArray<FIKBoneConstraint*>& Constraints, 
		const FVector& EffectorTargetLocation, 
		TArray<FTransform>& OutTransforms,
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
		int32 NumPoints = InTransforms.Num();
		OutTransforms.Reserve(NumPoints);
		for (const FTransform& Transform : InTransforms)
		{
			OutTransforms.Add(Transform);
		}

		if (NumPoints < 2)
		{
			// Need at least one bone to do IK!
			return false;
		}

		// Gather bone lengths. BoneLengths contains the length of the bone ENDING at this point,
		// i.e., BoneLengths[i] contains the distance between point i-1 and point i
		TArray<float> BoneLengths;
		float MaximumReach = ComputeBoneLengths(InTransforms, BoneLengths);

		bool bBoneLocationUpdated = false;
		int32 EffectorIndex = NumPoints - 1;

		// Check distance between tip location and effector location
		float Slop = FVector::Dist(OutTransforms[EffectorIndex].GetLocation(), EffectorTargetLocation);
		if (Slop > Precision)
		{
			// Set tip bone at end effector location.
			OutTransforms[EffectorIndex].SetLocation(EffectorTargetLocation);
			int32 IterationCount = 0;
			while ((Slop > Precision) && (IterationCount++ < MaxIterations))
			{
				// "Forward Reaching" stage - adjust bones from end effector.
				FABRIKForwardPass(InTransforms, Constraints, BoneLengths, OutTransforms, Character);

				// Drag the root if enabled
				DragPointTethered(InTransforms[0], OutTransforms[1], BoneLengths[1], MaxRootDragDistance, RootDragStiffness, OutTransforms[0]);

				// "Backward Reaching" stage - adjust bones from root.
				FABRIKBackwardPass(InTransforms, Constraints, BoneLengths, OutTransforms, Character);
				Slop = FMath::Abs(BoneLengths[EffectorIndex] - FVector::Dist(OutTransforms[EffectorIndex - 1].GetLocation(), EffectorTargetLocation));
			}

			// Place effector based on how close we got to the target
			FVector EffectorLocation = OutTransforms[EffectorIndex].GetLocation();
			FVector EffectorParentLocation = OutTransforms[EffectorIndex - 1].GetLocation();
			EffectorLocation = EffectorParentLocation + (EffectorLocation - EffectorParentLocation).GetUnsafeNormal() * BoneLengths[EffectorIndex];
			OutTransforms[EffectorIndex].SetLocation(EffectorLocation);
			bBoneLocationUpdated = true;
		}

		// Update bone rotations
		if (bBoneLocationUpdated)
		{
			for (int32 PointIndex = 0; PointIndex < NumPoints - 1; ++PointIndex)
			{
				if (!FMath::IsNearlyZero(BoneLengths[PointIndex + 1]))
				{
					UpdateParentRotation(OutTransforms[PointIndex], InTransforms[PointIndex], OutTransforms[PointIndex + 1], InTransforms[PointIndex + 1]);
				}
			}
		}
		return bBoneLocationUpdated;
	}

	// Solves FABRIK on a CLOSED LOOP, that is, a chain where the effector is assumed to be connected to the root.
	//
	// Note that you will probably HAVE to use root dragging if you want this solver to work! If the root is not allowed to drag,
	// the solution degenerates and this solver will behave like normal FABRIK.	
	// I'm not sure how well constraints will work with this solver, but they are nonetheless supported for the sake of
	// keeping a consistent interface. You're welcome to try them out but results may be bad.	
	//
	// @param InTransforms - The starting transforms of each chain point. Not modified. Must contain at least 2 transforms. Because this is a closed loop, the last transform is assumed to be conneted to the first one.	
	// @param Constraints - Constraints corresponding to each chain point; entries should be set to nullptr for points that don't need a constraint. Constraints are enforced each time a point moves. Strong constraints may degrade the results of FABRIK, it's up to you to figure out what works.
	// @param EffectorTargetLocation - Where you want the effector to go. FABRIK will attempt to move the effector as close as possible to this point.
	// @param OutTransforms - The updated transforms for each chain point after FABRIK runs. Should be an empty array. Will be emptied and filled with new transforms.  
	// @param MaxRootDragDistance - How far the root may move from its original position. Set to 0 for no movement.
	// @param RootDragStiffness - How much the root will resist being moved from the original position. 1.0 means no resistance increase for more resistance. Settings less than 1.0 will make it move more.
	// @param Precision - Iteration will terminate when the effector is within this distance from the target. Decrease for possibly better results but possibly worse performance.
	// @param MaxIterations - The maximum number of iterations to run. Increase for possibly better results but possibly worse performance.
	// @param Character - Character pointer whcih may be used for debug drawing. May safely be set to nullptr or ignored.
	// @return - True if any transforms in OutTransforms were updated; otherwise, false. If false, the contents of OutTransforms is identical to InTransforms.
	static bool SolveClosedLoopFABRIK(
		const TArray<FTransform>& InTransforms, 
		const TArray<FIKBoneConstraint*>& Constraints,
		const FVector& EffectorTargetLocation, 
		TArray<FTransform>& OutTransforms, 
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
		int32 NumPoints = InTransforms.Num();
		int32 EffectorIndex = NumPoints - 1;
		// Gather bone transforms
		OutTransforms.Reserve(NumPoints);
		for (const FTransform& Transform : InTransforms)
		{
			OutTransforms.Add(Transform);
		}
		if (NumPoints < 2)
		{
			// Need at least one bone to do IK!
			return false;
		}

		// Gather bone lengths. BoneLengths contains the length of the bone ENDING at this point,
		// i.e., BoneLengths[i] contains the distance between point i-1 and point i
		TArray<float> BoneLengths;
		float MaximumReach = ComputeBoneLengths(InTransforms, BoneLengths);
		float RootToEffectorLength = FVector::Dist(InTransforms[0].GetLocation(), InTransforms[EffectorIndex].GetLocation());

		bool bBoneLocationUpdated = false;
		// Check distance between tip location and effector location
		float Slop = FVector::Dist(OutTransforms[EffectorIndex].GetLocation(), EffectorTargetLocation);
		if (Slop > Precision)
		{
			// The closed loop method is identical, except the root is dragged a second time to maintain
			// distance with the effector.		
			// Set tip bone at end effector location.
			OutTransforms[EffectorIndex].SetLocation(EffectorTargetLocation);

			int32 IterationCount = 0;
			while ((Slop > Precision) && (IterationCount++ < MaxIterations))
			{
				// "Forward Reaching" stage - adjust bones from end effector.
				FABRIKForwardPass(InTransforms, Constraints, BoneLengths, OutTransforms, Character);

				// Drag the root if enabled
				DragPointTethered(InTransforms[0], OutTransforms[1], BoneLengths[1], MaxRootDragDistance, RootDragStiffness, OutTransforms[0]);

				// Drag the root again, toward the effector (since they're connected in a closed loop)
				DragPointTethered(InTransforms[0], OutTransforms[EffectorIndex], RootToEffectorLength, MaxRootDragDistance, RootDragStiffness, OutTransforms[0]);

				// "Backward Reaching" stage - adjust bones from root.
				FABRIKBackwardPass(InTransforms, Constraints, BoneLengths, OutTransforms, Character);
				Slop = FVector::Dist(OutTransforms[EffectorIndex].GetLocation(), EffectorTargetLocation);
			}
			bBoneLocationUpdated = true;
		}

		// Update bone rotations
		if (bBoneLocationUpdated)
		{
			for (int32 PointIndex = 0; PointIndex < NumPoints - 1; ++PointIndex)
			{
				if (!FMath::IsNearlyZero(BoneLengths[PointIndex + 1]))
				{
					UpdateParentRotation(OutTransforms[PointIndex], InTransforms[PointIndex], OutTransforms[PointIndex + 1], InTransforms[PointIndex + 1]);
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
	// See www.andreasaristidou.com/publications/papers/Extending_FABRIK_with_Model_Cοnstraints.pdf
	//
	// @param InClosedLoop - Input closed loop; describes starting positions of the  closed loop
	// @param EffectorATarget - Target location for noisy effector A
	// @param EffectorBTarget - Target location for noisy effector B
	// @param OutClosedLoop - Adjusted transforms of the closed loop points will be output here. 
	// @param MaxRootDragDistance - How far the root point may be dragged from its starting position
	// @param RootDragStiffness - How much the root resists being dragged. Set to 1.0 for no resistance; higher will resist dragging, lower will enhance dragging
	// @param Precision - Solver will stop iterating when both Effector A and Effector B moved less than this amount on the last iteration.
	// @param MaxIterations - The maximum number of iterations the solver may run
	// @param Character - Optional character pointer, used for debug drawing. 
	// @result True if at least on transform changed. This algorithm always changes the transforms, so it always returns true.
	static bool SolveNoisyThreePoint(
		const FNoisyThreePointClosedLoop& InClosedLoop, 
		const FTransform& EffectorAReference, 
		const FTransform& EffectorBReference,
		FNoisyThreePointClosedLoop& OutClosedLoop, 
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
		float DistAToRoot = InClosedLoop.TargetRootADistance;
		float DistBToRoot = InClosedLoop.TargetRootBDistance;
		float DistAToB = InClosedLoop.TargetABDistance;
		float DistARef = FVector::Dist(A.GetLocation(), EffectorAReference.GetLocation());
		float DistBRef = FVector::Dist(B.GetLocation(), EffectorBReference.GetLocation());

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

		float PrecisionSq = Precision * Precision;
		float Delta = FMath::Max(FVector::DistSquared(A.GetLocation(), LastA), FVector::Dist(B.GetLocation(), LastB));
		LastA = A.GetLocation();
		LastB = B.GetLocation();

		while ((Delta > PrecisionSq) && (IterationCount++ < MaxIterations))
		{
			// Iterate phases 3-5 only
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

			Delta = FMath::Max(FVector::DistSquared(A.GetLocation(), LastA), FVector::Dist(B.GetLocation(), LastB));
			LastA = A.GetLocation();
			LastB = B.GetLocation();
		}
		// Update rotations
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
		// Copy transforms to output
		OutClosedLoop.EffectorATransform = A;
		OutClosedLoop.EffectorBTransform = B;
		OutClosedLoop.RootTransform = Root;
		return true;
	}

protected:
	// Iterate from effector to root
	static void FABRIKForwardPass(
		const TArray<FTransform>& InTransforms,
		const TArray<FIKBoneConstraint*>& Constraints,
		const TArray<float>& BoneLengths, 
		TArray<FTransform>& OutTransforms, 
		ACharacter* Character = nullptr)
	{
		int32 NumPoints = InTransforms.Num();
		int32 EffectorIndex = NumPoints - 1;

		for (int32 PointIndex = EffectorIndex - 1; PointIndex > 0; --PointIndex)
		{
			FTransform& CurrentPoint = OutTransforms[PointIndex];
			FTransform& ChildPoint = OutTransforms[PointIndex + 1];

			// Move the parent to maintain starting bone lengths
			DragPoint(ChildPoint, BoneLengths[PointIndex + 1], CurrentPoint);

			// Enforce parent's constraint any time child is moved
			FIKBoneConstraint* CurrentConstraint = Constraints[PointIndex - 1];
			if (CurrentConstraint != nullptr && CurrentConstraint->bEnabled)
			{
				CurrentConstraint->SetupFn(PointIndex - 1, InTransforms, Constraints, OutTransforms);
				CurrentConstraint->EnforceConstraint(PointIndex - 1, InTransforms, Constraints, OutTransforms, Character);
			}
		}
	}

	// Iterate from root to effector
	static void FABRIKBackwardPass(
		const TArray<FTransform>& InTransforms, 
		const TArray<FIKBoneConstraint*>& Constraints, 
		const TArray<float>& BoneLengths, 
		TArray<FTransform>& OutTransforms, 
		ACharacter* Character = nullptr)
	{
		int32 NumPoints = InTransforms.Num();
		int32 EffectorIndex = NumPoints - 1;

		for (int32 PointIndex = 1; PointIndex < EffectorIndex; PointIndex++)
		{
			FTransform& ParentPoint = OutTransforms[PointIndex - 1];
			FTransform& CurrentPoint = OutTransforms[PointIndex];

			// Move the child to maintain starting bone lengths
			DragPoint(ParentPoint, BoneLengths[PointIndex], CurrentPoint);

			// Enforce parent's constraint any time child is moved
			FIKBoneConstraint* CurrentConstraint = Constraints[PointIndex - 1];
			if (CurrentConstraint != nullptr && CurrentConstraint->bEnabled)
			{
				CurrentConstraint->SetupFn(PointIndex - 1, InTransforms, Constraints, OutTransforms);
				CurrentConstraint->EnforceConstraint(PointIndex - 1, InTransforms, Constraints, OutTransforms, Character);
			}
		}
	}

	// Updates the rotation of the parent to point toward the child, using the shortest rotation
	static void UpdateParentRotation(
		FTransform& NewParentTransform, 
		const FTransform& OldParentTransform, 
		const FTransform& NewChildTransform, 
		const FTransform& OldChildTransform)
	{
		FVector OldDir = (OldChildTransform.GetLocation() - OldParentTransform.GetLocation()).GetUnsafeNormal();
		FVector NewDir = (NewChildTransform.GetLocation() - NewParentTransform.GetLocation()).GetUnsafeNormal();
		FVector RotationAxis = FVector::CrossProduct(OldDir, NewDir).GetSafeNormal();
		float RotationAngle = FMath::Acos(FVector::DotProduct(OldDir, NewDir));
		FQuat DeltaRotation = FQuat(RotationAxis, RotationAngle);
		NewParentTransform.SetRotation(DeltaRotation * OldParentTransform.GetRotation());
		NewParentTransform.NormalizeRotation();
	}

	// The core FABRIK method. Projects PointToMove onto the vector between itself and MaintainDistancePoint, 
	// such that the distance between them is BoneLength. This enforces the core FABRIK constraint, that inter-point
	// distances don't change. Thus, PointToMove is 'dragged' toward MaintainDistancePoint and the original interpoint
	// distance is maintained.
	static FORCEINLINE void DragPoint(const FTransform& MaintainDistancePoint, float BoneLength, FTransform& PointToMove)
	{
		PointToMove.SetLocation(MaintainDistancePoint.GetLocation() + (PointToMove.GetLocation() - MaintainDistancePoint.GetLocation()).GetUnsafeNormal() * BoneLength);
	}

	// Drags PointToDrag relative to MaintainDistancePoint; that is, PointToDrag is moved so that it attempts
	// to maintain the distance BoneLength between itself and MaintainDistancePoint. However, PointToDrag is 'tethered'
	// to TetherPoint; it cannot ever be dragged father than MaxDragDistance from TetherPoint. Additionally, the 
	// tether acts like a spring, instantaneously pulling MaintainDistancePoint back toward TetherPoint by an amount dictated by DragStiffnes;
	// specifically, the required displacement (before clamping to MaxDragDisplacement) is divided by DragStiffness.
	// Basically, this is like EnforcePointDistance, but with the additional stronger constraint that PointToDrag
	// can never be moved father than MaxDragDistance from StartingTransform.
	static void DragPointTethered(
		const FTransform& StartingTransform, 
		const FTransform& MaintainDistancePoint,
		float BoneLength, 
		float MaxDragDistance, 
		float DragStiffness, 
		FTransform& PointToDrag)
	{
		if (MaxDragDistance < KINDA_SMALL_NUMBER || DragStiffness < KINDA_SMALL_NUMBER)
		{
			PointToDrag = StartingTransform;
			return;
		}

		FVector Target = FMath::IsNearlyZero(BoneLength) ? 
			Target = MaintainDistancePoint.GetLocation() : 
			MaintainDistancePoint.GetLocation() + (PointToDrag.GetLocation() - MaintainDistancePoint.GetLocation()).GetUnsafeNormal() * BoneLength;

		FVector Displacement = Target - StartingTransform.GetLocation();
		// Root drag stiffness 'pulls' the root back (set to 1.0 to disable)
		Displacement /= DragStiffness;
		// limit root displacement to drag length
		FVector LimitedDisplacement = Displacement.GetClampedToMaxSize(MaxDragDistance);
		PointToDrag.SetLocation(StartingTransform.GetLocation() + LimitedDisplacement);
	}

	// Compute bone lengths and store in BoneLengths. BoneLengths will be emptied and refilled.
	// Each entry contains the length of bone ending at point i, i.e., OutBoneLengths[i] contains the starting distance 
	// between point i and point i-1.
	// Returns the maximum reach.
	static float ComputeBoneLengths(const TArray<FTransform>& InTransforms, TArray<float>& OutBoneLengths)
	{
		int32 NumPoints = InTransforms.Num();
		float MaximumReach = 0.0f;
		OutBoneLengths.Empty();
		OutBoneLengths.Reserve(NumPoints);

		// Root always has zero length
		OutBoneLengths.Add(0.0f);
		for (int32 i = 1; i < NumPoints; ++i)
		{
			OutBoneLengths.Add(FVector::Dist(InTransforms[i - 1].GetLocation(), InTransforms[i].GetLocation()));
			MaximumReach += OutBoneLengths[i];
		}
		return MaximumReach;
	}
};
