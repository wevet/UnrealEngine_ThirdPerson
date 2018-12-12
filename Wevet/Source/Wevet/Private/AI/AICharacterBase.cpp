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

AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), 
	SenseTimeOut(4.f)
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

	if (ensure(PawnSensingComponent)) 
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAICharacterBase::OnSeePawnRecieve);
		PawnSensingComponent->OnHearNoise.AddDynamic(this, &AAICharacterBase::OnHearNoiseRecieve);
	}

	if (ensure(WidgetComponent)) 
	{
		UAIUserWidgetBase* AIWidget = Cast<UAIUserWidgetBase>(WidgetComponent->GetUserWidgetObject());
		AIWidget->Init(this);
	}
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDeath_Implementation())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	// has weapon empty
	bool bEmpty = false;
	if (bSensedTarget)
	{
		if (Super::SelectedWeapon && Super::SelectedWeapon->bEmpty)
		{
			bEmpty = true;
		}
	}
	if (bEmpty)
	{
		SetTargetActor(nullptr);
		BP_FireReleaseReceive();
		return;
	}

	// has weapon reload
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
		return;
	}

	// attack timer finished
	bool bStopped = false;
	if (bSensedTarget 
		&& (World->TimeSeconds - LastSeenTime) > SenseTimeOut 
		&& (World->TimeSeconds - LastHeardTime) > SenseTimeOut)
	{
		bStopped = true;
	}

	if (bStopped)
	{
		SetTargetActor(nullptr);
		BP_FireReleaseReceive();
		return;
	}

	// found target
	if (TargetCharacter)
	{
		if (TargetCharacter->IsDeath_Implementation())
		{
			SetTargetActor(nullptr);
			BP_FireReleaseReceive();
			return;
		}
		else 
		{
			BulletInterval += DeltaTime;
			if (BulletInterval >= BulletDelay)
			{
				BP_FirePressReceive();
				BulletInterval = 0.f;
				// repeat sense target
				LastSeenTime = World->GetTimeSeconds();
				LastHeardTime = World->GetTimeSeconds();
				//bSensedTarget = true;
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

	WidgetComponent->SetVisibility(false);
	TargetCharacter = nullptr;
	Super::Die_Implementation();

}

void AAICharacterBase::NotifyEquip_Implementation()
{
	if (Super::SelectedWeapon == nullptr)
	{
		return;
	}

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

void AAICharacterBase::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor)
{
	Super::OnTakeDamage_Implementation(BoneName, Damage, Actor);
	if (Super::IsDeath_Implementation())
	{
		Die_Implementation();
	}
}

UClass* AAICharacterBase::GetOwnerClass_Implementation() const
{
	return GetClass()->StaticClass();
}

void AAICharacterBase::SetTargetActor(ACharacterBase* NewCharacter)
{
	TargetCharacter = NewCharacter;
	AAIControllerBase* AIController = Cast<AAIControllerBase>(GetController());
	if (AIController)
	{
		AIController->SetBlackboardSeeActor(HasEnemyFound());
	}
	Super::EquipmentActionMontage();
	bSensedTarget = TargetCharacter == nullptr ? false : true;
	//UE_LOG(LogTemp, Warning, TEXT("SeeActor : %s"), HasEnemyFound() ? TEXT("true") : TEXT("false"));
}

bool AAICharacterBase::HasEnemyFound() const
{
	if (TargetCharacter)
	{
		return true;
	}
	return false;
}

const bool AAICharacterBase::HasEquipWeapon()
{
	return Super::HasEquipWeapon() && HasEnemyFound();
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
	UWorld* const World = GetWorld();

	if (SpawnWeapon == nullptr || World == nullptr)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	const FTransform Transform = FTransform::Identity;
	AWeaponBase* const SpawningObject = World->SpawnActor<AWeaponBase>(SpawnWeapon, Transform, SpawnParams);
	Super::SelectedWeapon = SpawningObject;

	check(Super::SelectedWeapon);

	// setup assets
	Super::SelectedWeapon->SetEquip(false);
	Super::SelectedWeapon->SetCharacterOwner(this);
	Super::SelectedWeapon->OffVisible_Implementation();
	Super::SelectedWeapon->GetSphereComponent()->DestroyComponent();

	FName SocketName = Super::SelectedWeapon->WeaponItemInfo.UnEquipSocketName;
	Super::SelectedWeapon->AttachToComponent(Super::GetMesh(), { EAttachmentRule::SnapToTarget, true }, SocketName);

	if (Super::WeaponList.Find(Super::SelectedWeapon) == INDEX_NONE)
	{
		Super::WeaponList.Emplace(Super::SelectedWeapon);
	}
}

void AAICharacterBase::CreateWayPointList(TArray<AWayPointBase*>& OutWayPointList)
{
	UWorld* World = GetWorld();
	
	if (World == nullptr)
	{
		return;
	}

	for (TActorIterator<AWayPointBase> ActorIterator(World); ActorIterator; ++ActorIterator)
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
	auto Player = Cast<AMockCharacter>(OtherPawn);
	if (Player && !Player->IsDeath_Implementation())
	{
		SetTargetActor(Player);
	}
}

void AAICharacterBase::OnHearNoiseRecieve(APawn* OtherActor, const FVector& Location, float Volume)
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
	auto Player = Cast<AMockCharacter>(OtherActor);
	if (Player && !Player->IsDeath_Implementation())
	{
		SetTargetActor(Player);
	}
}

