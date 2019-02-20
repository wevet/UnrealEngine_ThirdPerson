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

	if (ensure(PawnSensingComponent && PawnSensingComponent->IsValidLowLevel())) 
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAICharacterBase::OnSeePawnRecieve);
		PawnSensingComponent->OnHearNoise.AddDynamic(this, &AAICharacterBase::OnHearNoiseRecieve);
	}

	if (ensure(WidgetComponent && WidgetComponent->IsValidLowLevel())) 
	{
		if (UAIUserWidgetBase* AIWidget = Cast<UAIUserWidgetBase>(WidgetComponent->GetUserWidgetObject()))
		{
			AIWidget->Initializer(this);
		}
	}
	if (AAIControllerBase* Controller = Cast<AAIControllerBase>(GetController()))
	{
		AIController = Controller;
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
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if ((Super::GetSelectedWeapon() && Super::GetSelectedWeapon()->bEmpty)
		|| (Super::GetSelectedWeapon() && Super::GetSelectedWeapon()->bReload))
	{
		return;
	}

	if (bSeeTarget && (World->TimeSeconds - LastSeenTime) > SenseTimeOut)
	{
		bSeeTarget = false;
		SetSeeTargetActor(nullptr);
	}

	if (bHearTarget && (World->TimeSeconds - LastHeardTime) > SenseTimeOut)
	{
		bHearTarget = false;
		SetHearTargetActor(nullptr);
	}

	// only see target
	if (HasEnemyFound())
	{
		if (!ICombatExecuter::Execute_IsDeath(TargetCharacter))
		{
			if (!HasEquipWeapon())
			{
				return;
			}
			BulletInterval += DeltaTime;
			if (BulletInterval >= BulletDelay)
			{
				BP_FirePressReceive();
				BulletInterval = 0.f;
			}
		}
		else
		{
			bSeeTarget = false;
			SetSeeTargetActor(nullptr);
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
	// not spawned WeaponActor
	WeaponList.Empty();
	GetController()->UnPossess();

	if (ensure(PawnSensingComponent && PawnSensingComponent->IsValidLowLevel()))
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
		check(CurrentWeapon.IsValid());
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
	check(CurrentWeapon.IsValid());
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
	return (TargetCharacter && bSeeTarget) ? true : false;
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
	return FVector::ZeroVector;
}

FVector AAICharacterBase::BulletTraceForwardLocation() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get()->GetMuzzleTransform().GetRotation().GetForwardVector();
	}
	return FVector::ForwardVector;
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

	if (Super::WeaponList.Find(CurrentWeapon.Get()) == INDEX_NONE)
	{
		Super::WeaponList.Emplace(CurrentWeapon.Get());
	}
}

void AAICharacterBase::CreateWayPointList(TArray<AWayPointBase*>& OutWayPointList)
{
	UWorld* const World = GetWorld();

	if (World)
	{
		for (TActorIterator<AWayPointBase> ActorIterator(World); ActorIterator; ++ActorIterator)
		{
			if (AWayPointBase* WayPoint = *ActorIterator)
			{
				OutWayPointList.Emplace(WayPoint);
			}
		}
	}
}

void AAICharacterBase::OnSeePawnRecieve(APawn* OtherPawn)
{
	if (ICombatExecuter::Execute_IsDeath(this))
	{
		return;
	}

	if (AMockCharacter* Character = Cast<AMockCharacter>(OtherPawn))
	{
		if (!ICombatExecuter::Execute_IsDeath(Character))
		{
			LastSeenTime = GetWorld()->GetTimeSeconds();
			//UE_LOG(LogWevetClient, Warning, TEXT("See\n from : %s \n to : %s \n"), *GetName(), *OtherPawn->GetName());

			if (!bSeeTarget)
			{
				bSeeTarget = true;
				SetSeeTargetActor(Character);
			}
		}
	}
}

void AAICharacterBase::SetSeeTargetActor(ACharacterBase* NewCharacter)
{
	TargetCharacter = NewCharacter;
	AIController->SetBlackboardSeeActor(HasEnemyFound());
	AIController->SetTargetEnemy(TargetCharacter);
	if (!NewCharacter)
	{
		BP_FireReleaseReceive();
		Super::UnEquipmentActionMontage();
		return;
	}
	Super::EquipmentActionMontage();
}

void AAICharacterBase::OnHearNoiseRecieve(APawn* OtherActor, const FVector& Location, float Volume)
{
	if (ICombatExecuter::Execute_IsDeath(this))
	{
		return;
	}

	LastHeardTime = GetWorld()->GetTimeSeconds();
	//UE_LOG(LogWevetClient, Warning, TEXT("Heard\n from : %s \n to : %s \n Vol : %f"), *GetName(), *OtherActor->GetName(), Volume);

	if (!bHearTarget)
	{
		bHearTarget = true;
		SetHearTargetActor(OtherActor);
	}
}

void AAICharacterBase::SetHearTargetActor(AActor* OtherActor)
{
	AIController->SetBlackboardHearActor(bHearTarget);
	if (OtherActor)
	{
		const FVector Start  = GetActorLocation();
		const FVector Target = OtherActor->GetActorLocation();
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
		SetActorRotation(LookAtRotation);
		AIController->SetBlackboardPatrolLocation(Target);
		Super::EquipmentActionMontage();
	}
	//else
	//{
	//	Super::UnEquipmentActionMontage();
	//}
}
