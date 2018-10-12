// Fill out your copyright notice in the Description page of Project Settings.

#include "AICharacterBase.h"
#include "AIControllerBase.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"


AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), 
	IsEnemyFound(false)
{
	PrimaryActorTick.bCanEverTick = true;

	// create pawnsensing
	PawnSensingComponent = ObjectInitializer.CreateDefaultSubobject<UPawnSensingComponent>(this, TEXT("PawnSensingComponent"));
	PawnSensingComponent->SetPeripheralVisionAngle(90.f);
	PawnSensingComponent->SensingInterval = .1f;
	PawnSensingComponent->bOnlySensePlayers = false;
	PawnSensingComponent->bHearNoises = true;
	PawnSensingComponent->bSeePawns = true;

	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetMesh());
	WidgetComponent->SetDrawSize(FVector2D(200.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 230.f));
	const FRotator Rot = FRotator::MakeFromEuler(FVector(0.f, 0.f, 90.f));
	WidgetComponent->SetWorldRotation(Rot);
}

void AAICharacterBase::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
}

void AAICharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAICharacterBase::Die_Implementation()
{
	if (Super::DieSuccessCalled)
	{
		return;
	}
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (WidgetComponent)
	{
		WidgetComponent->SetVisibility(false);
	}
	this->SetTargetActor(nullptr);
	Super::Die_Implementation();
}

void AAICharacterBase::SetTargetActor(AActor * Actor)
{
	this->Target = Actor;
}

void AAICharacterBase::SetEnemyFound(bool EnemyFound)
{
	this->IsEnemyFound = EnemyFound;
}

AMockCharacter* AAICharacterBase::GetPlayerCharacter() const
{
	return Cast<AMockCharacter>(GetTarget());
}

void AAICharacterBase::InitializePosses()
{
	UpdateWeaponEvent();
	UpdateWayPointEvent();
}

void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = Super::MovementSpeed;

	AAIControllerBase* AIController = Cast<AAIControllerBase>(Controller);

	// runtime
	if (PawnSensingComponent && PawnSensingComponent->IsValidLowLevel())
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAICharacterBase::OnSeePawnRecieve);
		PawnSensingComponent->OnHearNoise.AddDynamic(this, &AAICharacterBase::OnHearNoiseRecieve);
	}
}

FVector AAICharacterBase::BulletTraceRelativeLocation() const
{
	if (Super::GetSelectedWeapon() == nullptr)
	{
		return FVector::ZeroVector;
	}
	return Super::GetSelectedWeapon()->GetMuzzleTransform().GetLocation();
}

FVector AAICharacterBase::BulletTraceForwardLocation() const
{
	return GetControlRotation().Vector();
}

void AAICharacterBase::UpdateWeaponEvent()
{
	UWorld* World = GetWorld();

	if (this->SpawnWeapon == nullptr || World == nullptr)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	AActor* const SpawningObject = World->SpawnActor<AActor>(this->SpawnWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	Super::SelectedWeapon = Cast<AWeaponBase>(SpawningObject);

	check(Super::SelectedWeapon);

	// setup assets
	Super::SelectedWeapon->SetFireSoundAsset(Super::FireSound);
	Super::SelectedWeapon->SetFireAnimMontageAsset(Super::FireMontage);
	Super::SelectedWeapon->SetReloadAnimMontageAsset(Super::ReloadMontage);
	if (Super::SelectedWeapon->GetSphereComponent())
	{
		Super::SelectedWeapon->GetSphereComponent()->DestroyComponent();
	}
	Super::SelectedWeapon->OffVisible_Implementation();

	if (!Super::WeaponList.Contains(Super::SelectedWeapon))
	{
		Super::WeaponList.Add(Super::SelectedWeapon);
	}

	if (Super::SelectedWeapon)
	{
		FName SocketName = Super::SelectedWeapon->WeaponItemInfo.UnEquipSocketName;
		Super::SelectedWeapon->AttachToComponent(Super::GetMesh(), { EAttachmentRule::SnapToTarget, true }, SocketName);
	}
}

void AAICharacterBase::UpdateWayPointEvent()
{
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsOfClass(World, AWayPointBase::StaticClass(), FoundActor);

	for (TActorIterator<AWayPointBase> ActorIterator(World); ActorIterator; ++ActorIterator)
	{
		AWayPointBase* WayPoint = *ActorIterator;
		if (WayPoint)
		{
			this->WayPointList.Add(WayPoint);
		}
	}
}

void AAICharacterBase::Scanning()
{
}

void AAICharacterBase::OnSeePawnRecieve(APawn* OtherPawn)
{
}

void AAICharacterBase::OnHearNoiseRecieve(APawn* OtherActor, const FVector & Location, float Volume)
{
}

