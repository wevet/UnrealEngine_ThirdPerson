#pragma once
#include "Engine/EngineTypes.h"
#include "IKTypes.generated.h"

#define ENABLE_IK_DEBUG (1 && !(UE_BUILD_SHIPPING || UE_BUILD_TEST))
#define ENABLE_IK_DEBUG_VERBOSE (0 && !(UE_BUILD_SHIPPING || UE_BUILD_TEST))

UENUM(BlueprintType)
enum class ERangeLimitedFABRIKSolverMode : uint8
{
	RLF_Normal UMETA(DisplayName = "Normal Chain solver"),
	RLF_ClosedLoop UMETA(DisplayName = "Closed Loop")
};

UENUM(BlueprintType)
enum class EHumanoidArmTorsoIKMode : uint8
{
	IK_Human_ArmTorso_Disabled UMETA(DisplayName = "Disabled"),
	IK_Human_ArmTorso_LeftArmOnly UMETA(DisplayName = "IKLeftArmOnly"),
	IK_Human_ArmTorso_RightArmOnly UMETA(DisplayName = "IKRightArmOnly"),
	IK_Human_ArmTorso_BothArms UMETA(DisplayName = "BothArms"),
};

UENUM(BlueprintType)
enum class EHumanoidLegIKMode : uint8
{
	IK_Human_Leg_Locomotion UMETA(DisplayName = "Locomotion"),
	IK_Human_Leg_WorldLocation UMETA(DisplayName = "WorldLocation")
};


UENUM(BlueprintType)
enum class EHumanoidLegIKSolver : uint8
{
	IK_Human_Leg_Solver_FABRIK UMETA(DisplayName = "FABRIK"),
	IK_Human_Leg_Solver_TwoBone UMETA(DisplayName = "TwoBone")
};

UENUM(BlueprintType)
enum class EIKUnreachableRule : uint8
{
	IK_Abort UMETA(DisplayName = "Abort"),
	IK_Reach UMETA(DisplayName = "ReachForTarget"),
	IK_DragRoot UMETA(DisplayName = "DragChainRoot")
};

UENUM(BlueprintType)
enum class EIKROMConstraintMode : uint8
{
	IKROM_Pitch_And_Yaw UMETA(DisplayName = "Constrain Pitch and Yaw"),
	IKROM_Pitch_Only UMETA(DisplayName = "Constrain Pitch Only"),
	IKROM_Yaw_Only UMETA(DisplayName = "Constrain Yaw Only"),
	//IKROM_Twist_Only UMETA(DisplayName = "Constrain Twist Only"),
	IKROM_No_Constraint UMETA(DisplayName = "No Constraint")
};


UENUM(BlueprintType)
enum class EIKBoneAxis : uint8
{
	IKBA_X UMETA(DisplayName = "X"),
	IKBA_Y UMETA(DisplayName = "Y"),
	IKBA_Z UMETA(DisplayName = "Z"),
	IKBA_XNeg UMETA(DisplayName = "X Negative"),
	IKBA_YNeg UMETA(DisplayName = "Y Negative"),
	IKBA_ZNeg UMETA(DisplayName = "Z Negative")
};

