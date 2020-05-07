#pragma once

#include "Engine/EngineTypes.h"
#include "Animation/AnimMontage.h"
#include "LocomotionSystemTypes.h"
#include "LocomotionSystemStructs.generated.h"


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FTurnMontages
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	class UAnimMontage* TurnLAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	class UAnimMontage* TurnRAnim;

public:
	FTurnMontages() : 
		TurnLAnim(nullptr),
		TurnRAnim(nullptr)
	{
	}

};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FPivotData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float PivotDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	ELSMovementDirection CompletedMovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float CompletedStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	ELSMovementDirection InterruptedMovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float InterruptedStartTime;

public:
	FPivotData() : 
		CompletedMovementDirection(ELSMovementDirection::Forwards),
		InterruptedMovementDirection(ELSMovementDirection::Forwards)
	{
		PivotDirection = 0.0f;
		CompletedStartTime = 0.0f;
		InterruptedStartTime = 0.0f;
	}

};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FCameraSettings
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float TargetArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float CameraLagSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FVector SocketOffset;

public:
	FCameraSettings() : 
		TargetArmLength(0.0f),
		CameraLagSpeed(0.0f)
	{
		SocketOffset = FVector::ZeroVector;
	}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FCameraSettingsGait
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Run;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Sprint;

public:
	FCameraSettingsGait()
	{}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FCameraSettingsStance
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettingsGait Standing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Crouching;

public:
	FCameraSettingsStance()
	{}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FCameraSettingsTarget
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettingsStance VelocityDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettingsStance LookingDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Aiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Ragdoll;

public:
	FCameraSettingsTarget()
	{}
};

