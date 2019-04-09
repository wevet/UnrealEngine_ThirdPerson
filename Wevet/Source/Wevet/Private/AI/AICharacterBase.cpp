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

AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), 
	BulletDelay(1.4f),
	SenseTimeOut(8.f),
	bSeeTarget(false),
	bHearTarget(false)
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = ObjectInitializer.CreateDefaultSubobject<UPawnSensingComponent>(this, TEXT("PawnSensingComponent"));
	PawnSensingComponent->SetPeripheralVisionAngle(60.f);
	PawnSensingComponent->SightRadius = 2000.f;
	PawnSensingComponent->HearingThreshold = 600;
	PawnSensingComponent->LOSHearingThreshold = 1200;

	GetMovementComponent()->NavAgentProps.AgentRadius = 42;
	GetMovementComponent()->NavAgentProps.AgentRadius = 192;

	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetMesh());
	WidgetComponent->SetDrawSize(FVector2D(200.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 230.f));
	WidgetComponent->SetWorldRotation(FRotator::MakeFromEuler(FVector(0.f, 0.f, 90.f)));
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

	if (ComponentExtension::HasValid(WidgetComponent))
	{
		if (UAIUserWidgetBase* AIWidget = Cast<UAIUserWidgetBase>(WidgetComponent->GetUserWidgetObject()))
		{
			AIWidget->Initializer(this);
		}
	}
	AIController = Cast<AAIControllerBase>(GetController());

	auto RefSkeleton = GetMesh()->SkeletalMesh->Skeleton->GetReferenceSkeleton();
	for (int i = 0; i < RefSkeleton.GetRawBoneNum(); ++i)
	{
		auto Info = RefSkeleton.GetRawRefBoneInfo()[i];
		UE_LOG(LogWevetClient, Log, TEXT("BoneName : %s \n ParentIndex : %d"), *Info.Name.ToString(), Info.ParentIndex);
	}
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

const int AAICharacterBase::GetGenericTeamID()
{
	return PTG_TEAM_ID_ENEMY;
}

void AAICharacterBase::Die_Implementation()
{
	if (Super::bDied)
	{
		return;
	}

	WidgetComponent->SetVisibility(false);
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
	else 
	{
		//if (ACharacterBase* Character = Cast<ACharacterBase>(Actor))
		//{
		//	UE_LOG(LogWevetClient, Log, TEXT("Pained\n from : %s\n to : %s\n"), *Character->GetName(), *GetName());
		//}
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

	if (Super::InventoryComponent)
	{
		Super::InventoryComponent->AddWeaponInventory(CurrentWeapon.Get());
	}
	//if (Super::WeaponList.Find(CurrentWeapon.Get()) == INDEX_NONE)
	//{
	//	Super::WeaponList.Emplace(CurrentWeapon.Get());
	//}
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
	//AIController->SetBlackboardPatrolLocation(TargetCharacter->GetActorLocation());

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
		check(AIController);
		const FVector StartLocation   = GetActorLocation();
		const FVector TargetLocation  = OtherActor->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		//LookAtRotation.Roll  = 0.f;
		//LookAtRotation.Pitch = 0.f;
		SetActorRotation(LookAtRotation);
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

