// Copyright 2018 wevet works All Rights Reserved.

#include "AICharacterBase.h"
#include "AIUserWidgetBase.h"
#include "WeaponBase.h"
#include "WayPointBase.h"
#include "MockCharacter.h"
#include "AIControllerBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AiPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Component/CharacterInventoryComponent.h"
#include "AI/AIUserWidgetBase.h"

AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), 
	BulletDelay(1.4f),
	SenseTimeOut(8.f),
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
	if (ICombatExecuter::Execute_IsDeath(this))
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
	if (Super::bDied)
	{
		return;
	}
	if (UIController->IsValidLowLevel())
	{
		UIController->RemoveFromParent();
	}
	TargetCharacter = nullptr;
	// not spawned WeaponActor
	if (Super::InventoryComponent)
	{
		Super::InventoryComponent->RemoveAllInventory();
	}
	GetController()->UnPossess();

	if (ComponentExtension::HasValid(PawnSensingComponent))
	{
		PawnSensingComponent->OnSeePawn.RemoveDynamic(this, &AAICharacterBase::OnSeePawnRecieve);
		PawnSensingComponent->OnHearNoise.RemoveDynamic(this, &AAICharacterBase::OnHearNoiseRecieve);
	}

	USkeletalMeshComponent* SkeltalMesh = Super::GetMesh();
	if (SkeltalMesh && SkeltalMesh->bRenderCustomDepth)
	{
		SkeltalMesh->SetRenderCustomDepth(false);
	}
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

void AAICharacterBase::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor)
{
	Super::OnTakeDamage_Implementation(BoneName, Damage, Actor);
	if (ICombatExecuter::Execute_IsDeath(this))
	{
		Die_Implementation();
	}
}

bool AAICharacterBase::HasEnemyFound() const
{
	return (TargetCharacter && bSeeTarget);
}

void AAICharacterBase::InitializePosses()
{
	CreateWeaponInstance();
}

ACharacterBase* AAICharacterBase::GetTargetCharacter() const
{
	return TargetCharacter;
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

void AAICharacterBase::CreateWeaponInstance()
{
	UWorld* const World = GetWorld();

	if (SpawnWeapon == nullptr)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	const FTransform Transform = FTransform::Identity;
	AWeaponBase* const SpawningObject = World->SpawnActor<AWeaponBase>(SpawnWeapon, Transform, SpawnParams);
	
	CurrentWeapon = MakeWeakObjectPtr<AWeaponBase>(SpawningObject);
	const FName SocketName(CurrentWeapon.Get()->WeaponItemInfo.UnEquipSocketName);
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	CurrentWeapon.Get()->AttachToComponent(Super::GetMesh(), Rules, SocketName);
	CurrentWeapon.Get()->Take(this);
	CurrentWeapon.Get()->GetSphereComponent()->DestroyComponent();
	Super::InventoryComponent->AddWeaponInventory(CurrentWeapon.Get());
}

void AAICharacterBase::CreateWayPointList(TArray<AWayPointBase*>& OutWayPointList)
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	for (TActorIterator<AWayPointBase> ActorIterator(World); ActorIterator; ++ActorIterator)
	{
		if (AWayPointBase* WayPoint = *ActorIterator)
		{
			OutWayPointList.Emplace(WayPoint);
		}
	}
}

void AAICharacterBase::OnSeePawnRecieve(APawn* OtherPawn)
{
	//
}

void AAICharacterBase::SetSeeTargetActor(ACharacterBase* const NewCharacter)
{
	TargetCharacter = NewCharacter;

	if (TargetCharacter)
	{
		const FVector StartLocation  = GetActorLocation();
		const FVector TargetLocation = TargetCharacter->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		SetActorRotation(LookAtRotation);
	}
	AIController->SetBlackboardTarget(NewCharacter);
	AIController->SetBlackboardSeeActor(HasEnemyFound());

	if (NewCharacter)
	{
		//ForceSprint();
		Super::EquipmentActionMontage();
	}
	else
	{
		//UnForceSprint();
		BP_FireReleaseReceive();
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
	if ((CurrentWeapon.Get()->bEmpty) || (CurrentWeapon.Get()->bReload))
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
