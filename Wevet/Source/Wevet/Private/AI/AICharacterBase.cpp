// Copyright 2018 wevet works All Rights Reserved.

#include "AI/AICharacterBase.h"
#include "AI/AIUserWidgetBase.h"
#include "AI/AIControllerBase.h"
#include "Weapon/WeaponBase.h"
#include "Player/MockCharacter.h"
#include "Perception/AiPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"

using namespace Wevet;

AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	SenseTimeOut(8.f),
	bSeeTarget(false),
	bHearTarget(false),
	bWasVisibility(true)
{
	BulletDelay = 0.3f;
	AttackTraceForwardDistance = 1000.f;
	AttackTraceMiddleDistance = 2000.f;
	AttackTraceLongDistance   = 3000.f;
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = ObjectInitializer.CreateDefaultSubobject<UPawnSensingComponent>(this, TEXT("PawnSensingComponent"));
	PawnSensingComponent->SetPeripheralVisionAngle(60.f);
	PawnSensingComponent->SightRadius = 2000.f;
	PawnSensingComponent->HearingThreshold = 600;
	PawnSensingComponent->LOSHearingThreshold = 1200;

	GetMovementComponent()->NavAgentProps.AgentRadius = 42;
	GetMovementComponent()->NavAgentProps.AgentRadius = 192;
}

void AAICharacterBase::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
}

void AAICharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (ComponentExtension::HasValid(PawnSensingComponent))
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAICharacterBase::OnSeePawnRecieve);
		PawnSensingComponent->OnHearNoise.AddDynamic(this, &AAICharacterBase::OnHearNoiseRecieve);
	}
	AIController = Cast<AAIControllerBase>(GetController());
	CreateUIController();
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IDamageInstigator::Execute_IsDeath(this))
	{
		return;
	}
	MainLoop(DeltaTime);
}

void AAICharacterBase::MainLoop(float DeltaTime)
{
	//@NOTE
	//Subclass Extend
}

void AAICharacterBase::Die_Implementation()
{
	if (Super::bWasDied)
	{
		return;
	}
	if (UIController->IsValidLowLevel())
	{
		UIController->RemoveFromParent();
	}
	if (GetController())
	{
		auto ControllerRef = GetController();
		ControllerRef->UnPossess();
		ControllerRef->Destroy();
	}

	TargetCharacter = nullptr;
	// not spawned WeaponActor
	if (Super::InventoryComponent)
	{
		Super::InventoryComponent->RemoveAllInventory();
	}

	if (ComponentExtension::HasValid(PawnSensingComponent))
	{
		PawnSensingComponent->OnSeePawn.RemoveDynamic(this, &AAICharacterBase::OnSeePawnRecieve);
		PawnSensingComponent->OnHearNoise.RemoveDynamic(this, &AAICharacterBase::OnHearNoiseRecieve);
	}
	Super::GetMesh()->SetRenderCustomDepth(false);
	Super::Die_Implementation();
}

void AAICharacterBase::Equipment_Implementation()
{
	if (!HasEquipWeapon())
	{
		if (!CurrentWeapon.IsValid())
		{
			return;
		}
		const FName SocketName(CurrentWeapon.Get()->WeaponItemInfo.EquipSocketName);
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		Super::Equipment_Implementation();
		CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
	}
}

void AAICharacterBase::UnEquipment_Implementation()
{
	if (bSeeTarget || bHearTarget)
	{
		return;
	}
	if (!CurrentWeapon.IsValid())
	{
		return;
	}
	Super::UnEquipment_Implementation();
	const FName SocketName(CurrentWeapon.Get()->WeaponItemInfo.UnEquipSocketName);
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
}

void AAICharacterBase::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor, bool& bDied)
{
	Super::OnTakeDamage_Implementation(BoneName, Damage, Actor, bDied);
}

#pragma region AIPawnOwner
bool AAICharacterBase::IsSeeTarget_Implementation() const
{
	return bSeeTarget;
}

bool AAICharacterBase::IsHearTarget_Implementation() const
{
	return bHearTarget;
}

float AAICharacterBase::GetAttackTraceForwardDistance_Implementation() const
{
	return AttackTraceForwardDistance;
}

float AAICharacterBase::GetAttackTraceLongDistance_Implementation() const
{
	return AttackTraceLongDistance;
}

float AAICharacterBase::GetAttackTraceMiddleDistance_Implementation() const
{
	return AttackTraceMiddleDistance;
}

AActor* AAICharacterBase::GetTarget_Implementation()
{
	return TargetCharacter;
}

void AAICharacterBase::StateChange_Implementation(const EAIActionState NewAIActionState)
{
}

bool AAICharacterBase::CanMeleeStrike_Implementation() const
{
	if (TargetCharacter)
	{
		const FVector TargetLocation = TargetCharacter->GetActorLocation();
		const float AttackDistance = AttackTraceForwardDistance;
		const float Distance = (GetActorLocation() - TargetLocation).Size();
		return (AttackDistance > Distance);
	}
	return false;
}
#pragma endregion

bool AAICharacterBase::HasEnemyFound() const
{
	return (TargetCharacter && IsSeeTarget_Implementation());
}

void AAICharacterBase::InitializePosses()
{
	Super::CreateWeaponInstance(WeaponTemplate, true);
}

FVector AAICharacterBase::BulletTraceRelativeLocation() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get()->GetMuzzleTransform().GetLocation();
	}
	return Super::BulletTraceRelativeLocation();
}

FVector AAICharacterBase::BulletTraceForwardLocation() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get()->GetMuzzleTransform().GetRotation().GetForwardVector();
	}
	return Super::BulletTraceForwardLocation();
}

void AAICharacterBase::OnSeePawnRecieve(APawn* OtherPawn)
{
	//
}

void AAICharacterBase::SetSeeTargetActor(ACharacterBase* const NewCharacter)
{
	TargetCharacter = NewCharacter;

	const bool bWasSeeTarget = (TargetCharacter != nullptr);
	if (TargetCharacter)
	{
		const FVector StartLocation  = GetActorLocation();
		const FVector TargetLocation = TargetCharacter->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		SetActorRotation(LookAtRotation);
	}
	AIController->SetBlackboardTarget(NewCharacter);
	AIController->SetBlackboardSeeActor(bWasSeeTarget);

	if (NewCharacter)
	{
		Super::EquipmentActionMontage();
	}
	else
	{
		Super::FireReleassed();
		Super::UnEquipmentActionMontage();
	}
}

void AAICharacterBase::OnHearNoiseRecieve(APawn* OtherActor, const FVector& Location, float Volume)
{
	//
}

void AAICharacterBase::SetHearTargetActor(AActor* const OtherActor)
{
	if (OtherActor)
	{
		const FVector StartLocation   = GetActorLocation();
		const FVector TargetLocation  = OtherActor->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);

		FRotator Rot = FRotator::ZeroRotator;
		Rot.Yaw = LookAtRotation.Yaw;
		SetActorRotation(Rot);
		AIController->SetBlackboardPatrolLocation(TargetLocation);
		Super::EquipmentActionMontage();
	}
	else
	{
		Super::UnEquipmentActionMontage();
	}
	AIController->SetBlackboardHearActor(bHearTarget);
}

bool AAICharacterBase::CanShotup() const
{
	if (!CurrentWeapon.IsValid())
	{
		return false;
	}
	if (!CurrentWeapon.Get()->CanMeleeStrike_Implementation())
	{
		return false;
	}
	if (CurrentWeapon.Get()->WasReload())
	{
		return false;
	}
	return true;
}

void AAICharacterBase::ForceSprint()
{
	bSprint = true;
	MovementSpeed = DefaultMaxSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AAICharacterBase::UnForceSprint()
{
	bSprint = false;
	MovementSpeed = DefaultMaxSpeed * HALF_WEIGHT;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AAICharacterBase::CreateUIController()
{
	UWorld* World = GetWorld();
	if (UIControllerTemplate && World)
	{
		if (APlayerController * PC = Wevet::ControllerExtension::GetPlayer(World, 0))
		{
			UIController = CreateWidget<UAIUserWidgetBase>(PC, UIControllerTemplate);
			if (UIController)
			{
				UIController->AddToViewport((int32)EUMGLayerType::Main);
				UIController->Initializer(this);
			}
		}
	}
}
