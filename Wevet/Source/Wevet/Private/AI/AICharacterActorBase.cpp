// Fill out your copyright notice in the Description page of Project Settings.

#include "AICharacterActorBase.h"
#include "AIControllerBase.h"
#include "Engine.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"


AAICharacterActorBase::AAICharacterActorBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

void AAICharacterActorBase::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
}

void AAICharacterActorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAICharacterActorBase::Die_Implementation()
{
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (WidgetComponent)
	{
		WidgetComponent->SetVisibility(false);
	}
	this->SetTargetActor(nullptr);
	Super::Die_Implementation();
}

void AAICharacterActorBase::SetTargetActor(AActor * Actor)
{
	this->Target = Actor;
}

void AAICharacterActorBase::SetEnemyFound(bool EnemyFound)
{
	this->IsEnemyFound = EnemyFound;
}

AMockCharacter * AAICharacterActorBase::GetPlayerCharacter() const
{
	if (!GetWorld())
	{
		return nullptr;
	}
	return Cast<AMockCharacter>(GetTarget());
}

void AAICharacterActorBase::InitializePosses()
{
	UpdateWeaponEvent();
	UpdateWayPointEvent();
}

void AAICharacterActorBase::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = Super::MovementSpeed;

	AAIControllerBase* AIController = Cast<AAIControllerBase>(Controller);

	// runtime
	if (PawnSensingComponent && PawnSensingComponent->IsValidLowLevel())
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAICharacterActorBase::OnSeePawnRecieve);
		PawnSensingComponent->OnHearNoise.AddDynamic(this, &AAICharacterActorBase::OnHearNoiseRecieve);
	}
}

void AAICharacterActorBase::UpdateWeaponEvent()
{
	if (this->SpawnWeapon == nullptr)
	{
		return;
	}
	if (!GetWorld())
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	AActor* const SpawningObject = GetWorld()->SpawnActor<AActor>(this->SpawnWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	Super::SelectedWeapon = Cast<AWeaponBase>(SpawningObject);

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

void AAICharacterActorBase::UpdateWayPointEvent()
{
	if (GetWorld())
	{
		TArray<AActor*> FoundActor;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWayPointBase::StaticClass(), FoundActor);

		for (TActorIterator<AWayPointBase> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
		{
			AWayPointBase* WayPoint = *ActorIterator;
			if (WayPoint)
			{
				this->WayPointList.Add(WayPoint);
			}
		}
	}
}

void AAICharacterActorBase::Scanning()
{
}

void AAICharacterActorBase::OnSeePawnRecieve(APawn * OtherPawn)
{
}

void AAICharacterActorBase::OnHearNoiseRecieve(APawn * OtherActor, const FVector & Location, float Volume)
{
}

