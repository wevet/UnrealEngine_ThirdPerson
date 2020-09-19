// Copyright 2018 wevet works All Rights Reserved.

#include "AI/AICharacterBase.h"
#include "AI/AIControllerBase.h"
#include "AnimInstance/AICharacterAnimInstanceBase.h"
#include "Perception/AiPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"
#include "Widget/AIHealthController.h"


AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer),
	SenseTimeOut(8.f),
	HearTimeOut(20.f),
	MeleeAttackTimeOut(6.f),
	bSeeTarget(false),
	bHearTarget(false),
	bAttackInitialized(false)
{
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComponent = ObjectInitializer.CreateDefaultSubobject<UPawnSensingComponent>(this, TEXT("PawnSensingComponent"));
	PawnSensingComponent->SetPeripheralVisionAngle(60.f);
	PawnSensingComponent->SightRadius = 2000.f;
	PawnSensingComponent->HearingThreshold = 600.f;
	PawnSensingComponent->LOSHearingThreshold = 1200.f;

	GetMovementComponent()->NavAgentProps.AgentRadius = 42;
	GetMovementComponent()->NavAgentProps.AgentRadius = 192;

	BulletDelay = 0.3f;
	WalkingSpeed = 300.f;
	WidgetViewPortOffset = FVector2D(0.5f, 0.0f);
	Tags.Add(DAMAGE_TAG);
	Tags.Add(WATER_TAG);

	GetMesh()->ComponentTags.Add(WATER_LOCAL_TAG);
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
	DestroyHealthController();
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

	Super::CreateWeaponInstance(PrimaryWeapon, [&](AAbstractWeapon* Weapon)
	{
		if (Weapon)
		{
			CurrentWeapon = MakeWeakObjectPtr<AAbstractWeapon>(Weapon);
		}
	});

	Super::CreateWeaponInstance(SecondaryWeapon, [&](AAbstractWeapon* Weapon)
	{
		//
	});

	//CreateHealthController();
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MainLoop(DeltaTime);
}

void AAICharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	ALSMovementMode = GetPawnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

void AAICharacterBase::MainLoop(float DeltaTime)
{
}

void AAICharacterBase::InitializePosses()
{
	AIController = Cast<AAIControllerBase>(GetController());
}

#pragma region Ragdoll
void AAICharacterBase::StartRagdollAction()
{
	Super::StartRagdollAction();
	if (AIController)
	{
		AIController->StopTree();
	}

	SetStanning(true);
	SeePawnRecieveCallback(nullptr);
	HearNoiseRecieveCallback(nullptr);
	if (HasEquipWeapon())
	{
		CurrentWeapon.Get()->SetEquip(false);
	}
}

void AAICharacterBase::RagdollToWakeUpAction()
{
	if (IDamageInstigator::Execute_IsDeath(this))
	{
		return;
	}

	Super::RagdollToWakeUpAction();
	if (AIController)
	{
		AIController->ResumeTree();
	}
	SetStanning(false);
}
#pragma endregion

#pragma region Interface
float AAICharacterBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (!IDamageInstigator::Execute_IsDeath(this))
	{
		OnSeePawnRecieve(EventInstigator->GetPawn());
	}
	else
	{
		//
	}
	return ActualDamage;
}

void AAICharacterBase::Die_Implementation()
{
	if (Super::bWasDied)
	{
		return;
	}

	Tags.Reset(0);
	Tags.Add(FName(TEXT("DeadBody")));
	UWevetBlueprintFunctionLibrary::DrawDebugString(this, FString(TEXT("DeadBody")), FLinearColor::Red);

	Super::SetActorTickEnabled(false);
	DetachFromControllerPendingDestroy();
	DestroyHealthController();
	TargetCharacter = nullptr;

	if (Wevet::ComponentExtension::HasValid(PawnSensingComponent))
	{
		PawnSensingComponent->OnSeePawn.RemoveDynamic(this, &AAICharacterBase::OnSeePawnRecieve);
		PawnSensingComponent->OnHearNoise.RemoveDynamic(this, &AAICharacterBase::OnHearNoiseRecieve);
	}

	Super::GetMesh()->SetRenderCustomDepth(false);
	Super::Die_Implementation();

	if (DeathDelegate.IsBound())
	{
		DeathDelegate.Broadcast();
	}
}

void AAICharacterBase::Equipment_Implementation()
{
	if (!Super::HasEquipWeapon())
	{
		Super::Equipment_Implementation();
	}
}

void AAICharacterBase::UnEquipment_Implementation()
{
	if (Super::HasEquipWeapon())
	{
		Super::UnEquipment_Implementation();
	}
}

float AAICharacterBase::GetMeleeDistance_Implementation() const
{
	if (!CurrentWeapon.IsValid())
	{
		return ZERO_VALUE;
	}
	return CurrentWeapon.Get()->GetWeaponItemInfo().MeleeDistance;
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
	if (!TargetCharacter || !CurrentWeapon.IsValid() || IDamageInstigator::Execute_IsDeath(TargetCharacter))
	{
		return false;
	}

	if (!CurrentWeapon.Get()->WasEquip())
	{
		return false;
	}

	const FVector TargetLocation = TargetCharacter->GetActorLocation();
	const float Distance = (GetActorLocation() - TargetLocation).Size();
	const bool bWasResult = (GetMeleeDistance_Implementation() > Distance);
	return bWasResult;
}
#pragma endregion

void AAICharacterBase::OnSeePawnRecieve(APawn* OtherPawn)
{
}

void AAICharacterBase::OnHearNoiseRecieve(APawn* OtherActor, const FVector& Location, float Volume)
{
}

void AAICharacterBase::SeePawnRecieveCallback(ACharacterBase* const NewCharacter)
{
	if (!AIController)
	{
		return;
	}

	TargetCharacter = NewCharacter;
	bSeeTarget = (TargetCharacter != nullptr);
	if (TargetCharacter)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetCharacter->GetActorLocation());
		SetActorRotation(LookAtRotation);
	}

	if (TargetCharacter)
	{
		EquipmentActionMontage();
	}
	else
	{
		UnEquipmentActionMontage();
	}
	AIController->SetBlackboardTarget(TargetCharacter);
	AIController->SetBlackboardSeeActor(bSeeTarget);
}

void AAICharacterBase::HearNoiseRecieveCallback(AActor* const OtherActor, FVector Location)
{
	if (!AIController)
	{
		return;
	}

	bHearTarget = (OtherActor != nullptr);
	const FVector HearLocation = (Location != FVector::ZeroVector) ? Location : GetActorLocation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HearLocation);
	SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f));

	if (bHearTarget)
	{
		EquipmentActionMontage();
	}
	else
	{
		UnEquipmentActionMontage();
	}
	AIController->SetBlackboardPatrolLocation(HearLocation);
	AIController->SetBlackboardHearActor(bHearTarget);

	if (bDebugTrace)
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Location, 100.f, 12, FLinearColor::Yellow, 4.f, 2.f);
	}
}

void AAICharacterBase::EquipmentActionMontage()
{
	if (ALSMovementMode != ELSMovementMode::Grounded)
	{
		return;
	}

	if (!HasEquipWeapon())
	{
		Super::EquipmentActionMontage();
	}
}

void AAICharacterBase::UnEquipmentActionMontage()
{
	if (ALSMovementMode != ELSMovementMode::Grounded)
	{
		return;
	}

	if (HasEquipWeapon())
	{
		Super::UnEquipmentActionMontage();
	}
}

#pragma region Attack
void AAICharacterBase::AttackInitialize(const float InInterval, const float InTimeOut)
{
	if (!bAttackInitialized)
	{
		BulletInterval = InInterval;
		MeleeAttackTimeOut = InTimeOut;
		bAttackInitialized = true;
	}
}

void AAICharacterBase::AttackUnInitialize()
{
	MeleeAttackTimeOut = ZERO_VALUE;
	bAttackInitialized = false;
}
#pragma endregion

#pragma region Cover
void AAICharacterBase::FindFollowCharacter()
{
	TArray<class AAICharacterBase*> Characters;

	// @NOTE
	// PawnÇÃÇ›ëŒè€Ç∆Ç∑ÇÈ
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { EObjectTypeQuery::ObjectTypeQuery3 };

	//const bool bResult = UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), AttackTraceForwardDistance);
}
#pragma endregion

FVector AAICharacterBase::BulletTraceForwardLocation() const
{
	if (CurrentWeapon.IsValid())
	{
		const FTransform MuzzleTransform = CurrentWeapon.Get()->GetMuzzleTransform();
		FRotator MuzzleRotation = FRotator(MuzzleTransform.GetRotation());
		if (TargetCharacter)
		{
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
				MuzzleTransform.GetLocation(), 
				TargetCharacter->GetActorLocation());
			MuzzleRotation.Yaw = LookAtRotation.Yaw;
		}
		return BulletTraceRelativeLocation() + (MuzzleRotation.Vector() * CurrentWeapon.Get()->GetTraceDistance());
	}
	return GetActorForwardVector();
}

void AAICharacterBase::Sprint()
{
	Super::Sprint();
	if (bSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
}

void AAICharacterBase::StopSprint()
{
	Super::StopSprint();
	if (!bSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	}
}

void AAICharacterBase::CreateHealthController()
{
	if (UIHealthController || !UIHealthControllerTemplate)
	{
		return;
	}

	UIHealthController = CreateWidget<UAIHealthController>(GetWorld(), UIHealthControllerTemplate);
	if (UIHealthController)
	{
		UIHealthController->AddToViewport((int32)EUMGLayerType::WorldScreen);
		UIHealthController->SetViewPortOffset(WidgetViewPortOffset);
		UIHealthController->Initializer(this);
	}

}

void AAICharacterBase::DestroyHealthController()
{
	if (UIHealthController)
	{
		UIHealthController->ResetCharacterOwner();
		UIHealthController->RemoveFromParent();
		UIHealthController->ConditionalBeginDestroy();
		UIHealthController = nullptr;
	}
}
