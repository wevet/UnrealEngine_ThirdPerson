// Fill out your copyright notice in the Description page of Project Settings.

#include "AIControllerBase.h"
#include "Engine.h"


AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	this->FunctionName = FString(TEXT("CheckEnemySighting"));
	this->WayPointIndex = 0;
	this->IsWalkBack = false;
}

void AAIControllerBase::CreateTimerFunc()
{
	//this->TimerFunc = SetTimerByFunctionName();
	//GetWorld()->GetTimerManager().SetTimer();
}

ACharacterBase* AAIControllerBase::GetTargetCharacter() const
{
	return this->CharacterRef->GetPlayerCharacter();
}

void AAIControllerBase::Patrolling_Implementation()
{
	if (this->CharacterRef->IsDeath_Implementation()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("StopAlternateFunc"));
		GetWorld()->GetTimerManager().ClearTimer(this->AlternateFunc);
	}
	else 
	{
		// @TODO
		//if (this->CharacterRef->IsEnemyFound) 
		//{
		//	return;
		//}

		//AWayPointBase* Current = NULL;
		//
		//if (this->IsWalkBack) 
		//{
		//	--this->WayPointIndex;
		//}
		//else 
		//{
		//	++this->WayPointIndex;
		//}
		//Current = this->WayPointArray[this->WayPointIndex];

		//EPathFollowingRequestResult::Type RequsetType = MoveToActor(Current, -1.f, true, true, true, DefaultNavigationFilterClass, true);

		//if (RequsetType == EPathFollowingRequestResult::Type::RequestSuccessful)
		//{
		//	if (this->WayPointIndex <= 0) 
		//	{
		//		this->IsWalkBack = false;
		//	}
		//	if (this->WayPointIndex >= this->WayPointArray.Num() - 1)
		//	{
		//		this->IsWalkBack = true;
		//	}
		//}
	}
}

void AAIControllerBase::CheckEnemySighting_Implementation()
{
	// Pawn Death
	if (this->CharacterRef->IsDeath_Implementation())
	{
		GetWorld()->GetTimerManager().ClearTimer(this->TimerFunc);
		Super::StopMovement();
		return;
	}
	// Player Death
	if (this->GetTargetCharacter()->IsDeath_Implementation()) 
	{
		return;
	}
	// has enemy found
	if (this->CharacterRef->GetEnemyFound())
	{
		Hunting_Implementation();
	}
	else 
	{

	}
}

void AAIControllerBase::Hunting_Implementation()
{
	Super::Hunting_Implementation();
}

void AAIControllerBase::SetupAI()
{
	auto Pawn = Super::GetPawn();
	if (Pawn)
	{
		this->CharacterRef  = Cast<AAICharacterBase>(Pawn);
		this->CharacterRef->UpdateWayPointEvent();
		this->WayPointArray = this->CharacterRef->GetWayPointList();
		this->AcceptanceRadius = this->CharacterRef->GetAcceptanceRadius();
	}
	GetWorld()->GetTimerManager().SetTimer(this->AlternateFunc, this, &AAIControllerBase::Patrolling_Implementation, 0.5f, true);
}

void AAIControllerBase::OnFirePress()
{
	UWorld* World = GetWorld();

	if (!this->CharacterRef->GetActivate() || World == nullptr) 
	{
		return;
	}
	if (this->CharacterRef->GetSelectedWeapon() == nullptr) 
	{
		return;
	}
	AWeaponBase* Weapon = this->CharacterRef->GetSelectedWeapon();
	const FVector Start = Weapon->GetSkeletalMeshComponent()->GetSocketLocation(Weapon->GetMuzzleSocket());
	const FVector End = Start + (GetControlRotation().Vector() * 15000);

	FHitResult HitData(ForceInit);
	FCollisionQueryParams fCollisionQueryParams;
	fCollisionQueryParams.TraceTag = FName("");
	fCollisionQueryParams.OwnerTag = FName("");
	fCollisionQueryParams.bTraceAsyncScene = false;
	fCollisionQueryParams.bTraceComplex = true;
	fCollisionQueryParams.bFindInitialOverlaps = false;
	fCollisionQueryParams.bReturnFaceIndex = false;
	fCollisionQueryParams.bReturnPhysicalMaterial = false;
	fCollisionQueryParams.bIgnoreBlocks = false;
	fCollisionQueryParams.IgnoreMask = 0;
	fCollisionQueryParams.AddIgnoredActor(this);

	bool bSuccess = World->LineTraceSingleByChannel(HitData, Start, End, ECollisionChannel::ECC_Visibility, fCollisionQueryParams);

	FName Socket = Weapon->GetMuzzleSocket();
	FVector FireLocation = Weapon->GetSkeletalMeshComponent()->GetSocketTransform(Socket).GetLocation();
	UGameplayStatics::PlaySoundAtLocation(World, Weapon->GetFireSoundAsset(), FireLocation, 1.f, 1.f, 0.f, nullptr, nullptr);
	this->CharacterRef->PlayAnimMontage(Weapon->GetFireAnimMontageAsset(), 1.0f);

	--Weapon->WeaponItemInfo.CurrentAmmo;

	if (!bSuccess)
	{
		return;
	}
	UGameplayStatics::PlaySoundAtLocation(World, Weapon->GetFireImpactSoundAsset(), HitData.Location, 1.f, 1.f, 0.f, nullptr, nullptr);
	const FVector StartPoint = FireLocation;
	const FVector EndPoint   = HitData.ImpactPoint;
	const FRotator Rotation  = UKismetMathLibrary::FindLookAtRotation(StartPoint, EndPoint);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	ABulletBase* const Bullet = World->SpawnActor<ABulletBase>(this->BulletsBP, Transform, SpawnParams);

	ICombat* CombatInterface = Cast<ICombat>(HitData.Actor);
	if (CombatInterface)
	{	
		float Damage = (FMath::FRandRange(20.f, 35.f) / 1000.f);
		CombatInterface->OnTakeDamage_Implementation(HitData.BoneName, Damage, this);
	}
}

void AAIControllerBase::OnFireRelease()
{

}