// Copyright 2018 wevet works All Rights Reserved.

#include "AI/AICharacterBase.h"
#include "AI/AIControllerBase.h"
#include "Perception/AiPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"
#include "Widget/AIHealthController.h"


AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	SenseTimeOut(8.f),
	MeleeAttackTimeOut(6.f),
	bSeeTarget(false),
	bHearTarget(false),
	bWasVisibility(true)
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = ObjectInitializer.CreateDefaultSubobject<UPawnSensingComponent>(this, TEXT("PawnSensingComponent"));
	PawnSensingComponent->SetPeripheralVisionAngle(60.f);
	PawnSensingComponent->SightRadius = 2000.f;
	PawnSensingComponent->HearingThreshold = 600;
	PawnSensingComponent->LOSHearingThreshold = 1200;

	GetMovementComponent()->NavAgentProps.AgentRadius = 42;
	GetMovementComponent()->NavAgentProps.AgentRadius = 192;

	BulletDelay = 0.3f;
	WalkingSpeed = 300.f;
	WidgetViewPortOffset = FVector2D(0.5f, 0.0f);
	AttackTraceForwardDistance = 1000.f;
	AttackTraceMiddleDistance = 2000.f;
	AttackTraceLongDistance = 3000.f;

	Tags.Add(FName(TEXT("DamageInstigator")));
}

void AAICharacterBase::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
}

void AAICharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAICharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UIHealthController)
	{
		UIHealthController->ResetCharacterOwner();
		UIHealthController->RemoveFromParent();
		UIHealthController->ConditionalBeginDestroy();
		UIHealthController = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (Wevet::ComponentExtension::HasValid(PawnSensingComponent))
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAICharacterBase::OnSeePawnRecieve);
		PawnSensingComponent->OnHearNoise.AddDynamic(this, &AAICharacterBase::OnHearNoiseRecieve);
	}
	Super::CreateWeaponInstance(DefaultWeapon, true);
	CreateHealthController();
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MainLoop(DeltaTime);
}

void AAICharacterBase::MainLoop(float DeltaTime)
{
}

void AAICharacterBase::InitializePosses()
{
	AIController = Cast<AAIControllerBase>(GetController());
}

#pragma region Interface
float AAICharacterBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (!IDamageInstigator::Execute_IsDeath(this))
	{
		OnSeePawnRecieve(EventInstigator->GetPawn());
	}
	return ActualDamage;
}

void AAICharacterBase::Die_Implementation()
{
	if (Super::bWasDied)
	{
		return;
	}

	Super::SetActorTickEnabled(false);
	DetachFromControllerPendingDestroy();

	if (UIHealthController)
	{
		UIHealthController->ResetCharacterOwner();
		UIHealthController->RemoveFromParent();
		UIHealthController->ConditionalBeginDestroy();
		UIHealthController = nullptr;
	}

	TargetCharacter = nullptr;
	if (Super::InventoryComponent)
	{
		Super::InventoryComponent->RemoveAllInventory();
	}

	if (Wevet::ComponentExtension::HasValid(PawnSensingComponent))
	{
		PawnSensingComponent->OnSeePawn.RemoveDynamic(this, &AAICharacterBase::OnSeePawnRecieve);
		PawnSensingComponent->OnHearNoise.RemoveDynamic(this, &AAICharacterBase::OnHearNoiseRecieve);
	}
	Super::GetMesh()->SetRenderCustomDepth(false);
	Super::Die_Implementation();
}

void AAICharacterBase::Equipment_Implementation()
{
	if (!CurrentWeapon.IsValid())
	{
		return;
	}

	if (!Super::HasEquipWeapon())
	{
		const FName SocketName(CurrentWeapon.Get()->WeaponItemInfo.EquipSocketName);
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
		Super::Equipment_Implementation();
	}
}

void AAICharacterBase::UnEquipment_Implementation()
{
	if (!CurrentWeapon.IsValid())
	{
		return;
	}

	if (Super::HasEquipWeapon())
	{
		const FName SocketName(CurrentWeapon.Get()->WeaponItemInfo.UnEquipSocketName);
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
		Super::UnEquipment_Implementation();
	}
}

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

AActor* AAICharacterBase::GetTarget_Implementation() const
{
	return TargetCharacter;
}

void AAICharacterBase::StateChange_Implementation(const EAIActionState NewAIActionState)
{
	if (AIController)
	{
		AIController->SetBlackboardActionState(NewAIActionState);
	}
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

#pragma region PawnSense
void AAICharacterBase::OnSeePawnRecieve(APawn* OtherPawn)
{
}

void AAICharacterBase::OnHearNoiseRecieve(APawn* OtherActor, const FVector& Location, float Volume)
{
}

void AAICharacterBase::SetSeeTargetActor(ACharacterBase* const NewCharacter)
{
	TargetCharacter = NewCharacter;
	bSeeTarget = (TargetCharacter != nullptr);
	if (TargetCharacter)
	{
		const FVector StartLocation  = GetActorLocation();
		const FVector TargetLocation = TargetCharacter->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		SetActorRotation(LookAtRotation);
	}

	if (TargetCharacter)
	{
		Super::EquipmentActionMontage();
	}
	else
	{
		Super::FireReleassed();
		Super::UnEquipmentActionMontage();
	}
	AIController->SetBlackboardTarget(TargetCharacter);
	AIController->SetBlackboardSeeActor(bSeeTarget);
}

void AAICharacterBase::SetHearTargetActor(AActor* const OtherActor)
{
	bHearTarget = (OtherActor != nullptr);
	const FVector TargetLocation = bHearTarget ? OtherActor->GetActorLocation() : FVector::ZeroVector;
	if (OtherActor)
	{
		const FVector StartLocation   = GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		FRotator Rot = FRotator::ZeroRotator;
		Rot.Yaw = LookAtRotation.Yaw;
		SetActorRotation(Rot);
	}

	if (OtherActor)
	{
		Super::EquipmentActionMontage();
	}
	else
	{
		Super::UnEquipmentActionMontage();
	}
	AIController->SetBlackboardPatrolLocation(TargetLocation);
	AIController->SetBlackboardHearActor(bHearTarget);
}
#pragma endregion


void AAICharacterBase::DoCoverAI()
{
	TArray<class AAICharacterBase*> Characters;

	// PawnÇÃÇ›ëŒè€
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { EObjectTypeQuery::ObjectTypeQuery3 };

	//const bool bResult = UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), AttackTraceForwardDistance);
}

void AAICharacterBase::ForceSprint()
{
	if (!bSprint)
	{
		bSprint = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
}

void AAICharacterBase::UnForceSprint()
{
	if (bSprint)
	{
		bSprint = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	}
}

void AAICharacterBase::CreateHealthController()
{
	if (UIHealthController)
	{
		return;
	}

	if (UIHealthControllerTemplate == nullptr)
	{
		return;
	}

	if (APlayerController* PC = Wevet::ControllerExtension::GetPlayer(GetWorld()))
	{
		UIHealthController = CreateWidget<UAIHealthController>(PC, UIHealthControllerTemplate);
	}

	if (UIHealthController)
	{
		UIHealthController->AddToViewport((int32)EUMGLayerType::Main);
		UIHealthController->SetViewPortOffset(WidgetViewPortOffset);
		UIHealthController->Initializer(this);
	}

}
