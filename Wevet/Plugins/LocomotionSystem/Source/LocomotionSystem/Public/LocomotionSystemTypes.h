// Copyright 2018 wevet works All Rights Reserved.
#pragma once

#include "Engine/EngineTypes.h"
#include "LocomotionSystemTypes.generated.h"


UENUM(BlueprintType)
enum class ELSGait : uint8
{
	Walking   UMETA(DisplayName = "Walking"),
	Running   UMETA(DisplayName = "Running"),
	Sprinting UMETA(DisplayName = "Sprinting"),
};

UENUM(BlueprintType)
enum class ELSMovementMode : uint8
{
	None     UMETA(DisplayName = "None"),
	Grounded UMETA(DisplayName = "Grounded"),
	Falling  UMETA(DisplayName = "Falling"),
	Ragdoll  UMETA(DisplayName = "Ragdoll"),
	Swimming UMETA(DisplayName = "Swimming"),
	Mantling UMETA(DisplayName = "Mantling"),
};

UENUM(BlueprintType)
enum class ELSRotationMode : uint8
{
	VelocityDirection UMETA(DisplayName = "VelocityDirection"),
	LookingDirection  UMETA(DisplayName = "LookingDirection"),
};

UENUM(BlueprintType)
enum class ELSStance : uint8
{
	Standing UMETA(DisplayName = "Standing"),
	Crouching  UMETA(DisplayName = "Crouching"),
};

UENUM(BlueprintType)
enum class ELSViewMode : uint8
{
	ThirdPerson UMETA(DisplayName = "ThirdPerson"),
	FirstPerson  UMETA(DisplayName = "FirstPerson"),
};

UENUM(BlueprintType)
enum class ELSCardinalDirection : uint8
{
	North UMETA(DisplayName = "North"),
	East  UMETA(DisplayName = "East"),
	West  UMETA(DisplayName = "West"),
	South UMETA(DisplayName = "South"),
};

UENUM(BlueprintType)
enum class ELSFootStepType : uint8
{
	Step   UMETA(DisplayName = "Step"),
	Walk   UMETA(DisplayName = "Walk"),
	Run    UMETA(DisplayName = "Run"),
	Sprint UMETA(DisplayName = "Sprint"),
	Pivot  UMETA(DisplayName = "Pivot"),
	Jump   UMETA(DisplayName = "Jump"),
	Land   UMETA(DisplayName = "Land"),
};

UENUM(BlueprintType)
enum class ELSIdleEntryState : uint8
{
	N_Idle   UMETA(DisplayName = "N_Idle"),
	LF_Idle  UMETA(DisplayName = "LF_Idle"),
	RF_Idle  UMETA(DisplayName = "RF_Idle"),
	CLF_Idle UMETA(DisplayName = "CLF_Idle"),
	CRF_Idle UMETA(DisplayName = "CRF_Idle"),
};

UENUM(BlueprintType)
enum class ELSLocomotionState : uint8
{
	None      UMETA(DisplayName = "None"),
	NotMoving UMETA(DisplayName = "NotMoving"),
	Moving    UMETA(DisplayName = "Moving"),
	Pivot     UMETA(DisplayName = "Pivot"),
	Stopping  UMETA(DisplayName = "Stopping"),
};

UENUM(BlueprintType)
enum class ELSMovementDirection : uint8
{
	Forwards UMETA(DisplayName = "Forwards"),
	Backwards  UMETA(DisplayName = "Backwards"),
};

UENUM(BlueprintType)
enum class ELSMovementAction : uint8
{
	None       UMETA(DisplayName = "None"),
	LowMantle  UMETA(DisplayName = "LowMantle"),
	HighMantle UMETA(DisplayName = "HighMantle"),
	Rolling    UMETA(DisplayName = "Rolling"),
	GettingUp  UMETA(DisplayName = "GettingUp"),
};

UENUM(BlueprintType)
enum class EMantleType : uint8
{
	HighMantle   UMETA(DisplayName = "HighMantle"),
	LowMantle    UMETA(DisplayName = "LowMantle"),
	FallingCatch UMETA(DisplayName = "FallingCatch"),
};

