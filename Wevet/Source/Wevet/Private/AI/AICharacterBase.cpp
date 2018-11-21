// Copyright 2018 wevet works All Rights Reserved.

#include "AICharacterBase.h"
#include "AIUserWidgetBase.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AIControllerBase.h"


AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), 
	SenseTimeOut(4.f)
{
	PrimaryActorTick.bCanEverTick = true;

	// create pawnsensing
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

	if (!ensure(PawnSensingComponent)) 
	{
		return;
	}
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAICharacterBase::OnSeePawnRecieve);
	PawnSensingComponent->OnHearNoise.AddDynamic(this, &AAICharacterBase::OnHearNoiseRecieve);

	if (!ensure(WidgetComponent)) 
	{
		return;
	}
	UAIUserWidgetBase* AIWidget = Cast<UAIUserWidgetBase>(WidgetComponent->GetUserWidgetObject());
	check(AIWidget);
	AIWidget->Init(this);
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	bool bReload = false;
	if (bSensedTarget)
	{
		if (Super::SelectedWeapon && Super::SelectedWeapon->bReload)
		{
			bReload = true;
		}
	}

	if (bReload)
	{
		// weapon reload
		return;
	}

	// target died ?
	if (TargetCharacter && TargetCharacter->IsDeath_Implementation())
	{
		bSensedTarget = false;
		SetTargetActor(nullptr);
		Super::EquipmentActionMontage();
	}

	if (bSensedTarget
		&& (World->TimeSeconds - LastSeenTime) > SenseTimeOut
		&& (World->TimeSeconds - LastHeardTime) > SenseTimeOut)
	{
		bSensedTarget = false;
		SetTargetActor(nullptr);
		Super::EquipmentActionMontage();
	}
	else
	{
		if (HasEquipWeapon() && HasEnemyFound())
		{
			BulletInterval += DeltaTime;
			if (BulletInterval >= BulletDelay)
			{
				BP_FirePressReceive();
				BulletInterval = 0.f;
				// repeat sense target
				LastSeenTime = World->GetTimeSeconds();
				LastHeardTime = World->GetTimeSeconds();
				bSensedTarget = true;
			}
		}
	}
}

void AAICharacterBase::Die_Implementation()
{
	if (Super::bDied)
	{
		return;
	}

	if (WidgetComponent)
	{
		WidgetComponent->SetVisibility(false);
	}
	this->TargetCharacter = nullptr;
	Super::Die_Implementation();

}

void AAICharacterBase::NotifyEquip_Implementation()
{
	if (Super::SelectedWeapon)
	{
		if (HasEnemyFound())
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
	}
}

void AAICharacterBase::SetTargetActor(ACharacterBase* NewCharacter)
{
	this->TargetCharacter = NewCharacter;
	AAIControllerBase* AIController = Cast<AAIControllerBase>(GetController());
	if (AIController)
	{
		AIController->SetBlackboardSeeActor(HasEnemyFound());
	}
	UE_LOG(LogTemp, Warning, TEXT("SeeActor : %s"), HasEnemyFound() ? TEXT("true") : TEXT("false"));
}

void AAICharacterBase::InitializePosses()
{
	UpdateWeaponEvent();
}

FVector AAICharacterBase::BulletTraceRelativeLocation() const
{
	if (Super::SelectedWeapon)
	{
		return Super::SelectedWeapon->GetMuzzleTransform().GetLocation();
	}
	return FVector::ZeroVector;
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
	Super::SelectedWeapon->SetEquip(false);
	Super::SelectedWeapon->SetCharacterOwner(this);
	Super::SelectedWeapon->GetSphereComponent()->DestroyComponent();
	Super::SelectedWeapon->OffVisible_Implementation();

	FName SocketName = Super::SelectedWeapon->WeaponItemInfo.UnEquipSocketName;
	Super::SelectedWeapon->AttachToComponent(Super::GetMesh(), { EAttachmentRule::SnapToTarget, true }, SocketName);

	if (!Super::WeaponList.Contains(Super::SelectedWeapon))
	{
		Super::WeaponList.Emplace(Super::SelectedWeapon);
	}
}

void AAICharacterBase::CreateWayPointList(TArray<AWayPointBase*>& OutWayPointList)
{
	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWayPointBase::StaticClass(), FoundActor);

	for (TActorIterator<AWayPointBase> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		AWayPointBase* WayPoint = *ActorIterator;
		if (WayPoint)
		{
			OutWayPointList.Emplace(WayPoint);
		}
	}
}

void AAICharacterBase::OnSeePawnRecieve(APawn* OtherPawn)
{
	if (IsDeath_Implementation())
	{
		return;
	}

	if (!bSensedTarget)
	{
		//
	}
	else
	{
		// bAlready Sense Target
		return;
	}

	LastSeenTime = GetWorld()->GetTimeSeconds();
	bSensedTarget = true;

	auto Player = Cast<AMockCharacter>(OtherPawn);
	if (Player && !Player->IsDeath_Implementation())
	{
		SetTargetActor(Player);
		Super::EquipmentActionMontage();
	}
}

void AAICharacterBase::OnHearNoiseRecieve(APawn* OtherActor, const FVector & Location, float Volume)
{
	if (IsDeath_Implementation() || FMath::IsNearlyZero(Volume))
	{
		return;
	}

	if (!bSensedTarget)
	{
		//
	}
	else
	{
		// bAlready Sense Target
		return;
	}

	LastHeardTime = GetWorld()->GetTimeSeconds();
	bSensedTarget = true;

	auto Player = Cast<AMockCharacter>(OtherActor);
	if (Player && !Player->IsDeath_Implementation())
	{
		SetTargetActor(Player);
		Super::EquipmentActionMontage();
	}
}

