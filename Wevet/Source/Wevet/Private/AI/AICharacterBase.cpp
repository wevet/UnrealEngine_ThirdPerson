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

	UWorld* World = GetWorld();
	if (World)
	{
		bool bReload = false;
		if (bSensedTarget)
		{
			if (SelectedWeapon->IsReload)
			{
				bReload = true;
			}
		}

		if (bReload)
		{
			// weapon reload
			return;
		}

		if (bSensedTarget
			&& (World->TimeSeconds - LastSeenTime) > SenseTimeOut
			&& (World->TimeSeconds - LastHeardTime) > SenseTimeOut)
		{
			bSensedTarget = false;
			SetTargetActor(nullptr);
			Super::EquipmentMontage();
		}
		else
		{
			if (Super::IsEquipWeapon && HasEnemyFound())
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
}

void AAICharacterBase::Die_Implementation()
{
	if (Super::DieSuccessCalled)
	{
		return;
	}

	if (WidgetComponent)
	{
		WidgetComponent->SetVisibility(false);
	}
	this->Target = nullptr;
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

void AAICharacterBase::SetTargetActor(AActor* Actor)
{
	this->Target = Actor;
	AAIControllerBase* AIController = Cast<AAIControllerBase>(GetController());
	if (AIController)
	{
		AIController->SetBlackboardSeeActor(HasEnemyFound());
	}
	UE_LOG(LogTemp, Warning, TEXT("SeeActor : %s"), HasEnemyFound() ? TEXT("true") : TEXT("false"));
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
			this->WayPointList.Emplace(WayPoint);
		}
	}
}

void AAICharacterBase::UpdateSensing()
{
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

	LastSeenTime = GetWorld()->GetTimeSeconds();
	bSensedTarget = true;

	auto Player = Cast<AMockCharacter>(OtherPawn);
	if (Player && !Player->IsDeath_Implementation())
	{
		SetTargetActor(Player);
		Super::EquipmentMontage();
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

	LastHeardTime = GetWorld()->GetTimeSeconds();
	bSensedTarget = true;

	auto Player = Cast<AMockCharacter>(OtherActor);
	if (Player && !Player->IsDeath_Implementation())
	{
		SetTargetActor(Player);
		Super::EquipmentMontage();
	}
}

