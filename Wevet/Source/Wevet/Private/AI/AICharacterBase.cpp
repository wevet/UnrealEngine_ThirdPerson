// Fill out your copyright notice in the Description page of Project Settings.

#include "AICharacterBase.h"
#include "AIUserWidgetBase.h"
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

void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensingComponent && PawnSensingComponent->IsValidLowLevel())
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAICharacterBase::OnSeePawnRecieve);
		PawnSensingComponent->OnHearNoise.AddDynamic(this, &AAICharacterBase::OnHearNoiseRecieve);
	}

	if (WidgetComponent && WidgetComponent->IsValidLowLevel())
	{
		UAIUserWidgetBase* AIWidget = Cast<UAIUserWidgetBase>(WidgetComponent->GetUserWidgetObject());
		check(AIWidget);
		AIWidget->Init(this);
	}
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Super::SelectedWeapon)
	{
		if (this->IsEnemyFound && Super::IsEquipWeapon)
		{
			this->BulletInterval += DeltaTime;
			if (this->BulletInterval >= this->BulletDelay)
			{
				BP_FirePressReceive();
				this->BulletInterval = 0.f;
			}
		}
		else
		{
			//BP_FireReleaseReceive();
		}
	}
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
	// stopfireevent
	if (Super::SelectedWeapon)
	{
		Super::SelectedWeapon->OnFireRelease_Implementation();
	}
	SetTargetActor(nullptr);
	Super::Die_Implementation();
}

void AAICharacterBase::NotifyEquip_Implementation()
{
	if (Super::SelectedWeapon)
	{
		if (this->IsEnemyFound)
		{
			Super::SelectedWeapon->AttachToComponent(
				Super::GetMesh(), 
				{ EAttachmentRule::SnapToTarget, true }, 
				Super::SelectedWeapon->WeaponItemInfo.EquipSocketName);
			Super::Equipment_Implementation();
		}
		else 
		{
			Super::SelectedWeapon->AttachToComponent(
				Super::GetMesh(), 
				{ EAttachmentRule::SnapToTarget, true }, 
				Super::SelectedWeapon->WeaponItemInfo.UnEquipSocketName);
			Super::UnEquipment_Implementation();
		}
		Super::NotifyEquip_Implementation();
	}
}

void AAICharacterBase::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor)
{
	Super::OnTakeDamage_Implementation(BoneName, Damage, Actor);
	if (Super::IsDeath_Implementation())
	{
		Die_Implementation();
		OnFireReleaseInternal();
	}
}

void AAICharacterBase::SetTargetActor(AActor* Actor)
{
	this->Target = Actor;
}

void AAICharacterBase::SetEnemyFound(bool EnemyFound)
{
	bool InEnemyFound = this->IsEnemyFound;

	if (InEnemyFound == EnemyFound)
	{
		return;
	}
	this->IsEnemyFound = EnemyFound;
	PlayAnimMontage(this->EquipMontage, 1.6f);
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

void AAICharacterBase::OnFirePressedInternal()
{
	Super::SelectedWeapon->OnFirePressedInternal();
}

void AAICharacterBase::OnFireReleaseInternal()
{
	Super::SelectedWeapon->OnFireReleaseInternal();
}

FVector AAICharacterBase::BulletTraceRelativeLocation() const
{
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
	const FTransform Transform = FTransform::Identity;
	AActor* SpawningObject = World->SpawnActor<AActor>(this->SpawnWeapon, Transform, SpawnParams);
	Super::SelectedWeapon = Cast<AWeaponBase>(SpawningObject);

	check(Super::SelectedWeapon);

	// setup assets
	Super::SelectedWeapon->SetCharacterOwner(this);
	Super::SelectedWeapon->SetFireSoundAsset(Super::FireSound);
	Super::SelectedWeapon->SetFireAnimMontageAsset(Super::FireMontage);
	Super::SelectedWeapon->SetReloadAnimMontageAsset(Super::ReloadMontage);
	Super::SelectedWeapon->GetSphereComponent()->DestroyComponent();
	Super::SelectedWeapon->OffVisible_Implementation();

	FName SocketName = Super::SelectedWeapon->WeaponItemInfo.UnEquipSocketName;
	Super::SelectedWeapon->AttachToComponent(Super::GetMesh(), { EAttachmentRule::SnapToTarget, true }, SocketName);

	if (!Super::WeaponList.Contains(Super::SelectedWeapon))
	{
		Super::WeaponList.Add(Super::SelectedWeapon);
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

