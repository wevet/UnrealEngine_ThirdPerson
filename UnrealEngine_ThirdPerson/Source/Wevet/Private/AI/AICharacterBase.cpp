#include "AICharacterBase.h"
#include "MockCharacter.h"
#include "Engine.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"


AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->AcceptanceRadius = 1500.f;
}

void AAICharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AAICharacterBase::Tick(float DeltaTime)
{
	if (Super::IsDeath_Implementation())
	{
		return;
	}
	Super::Tick(DeltaTime);

	if (Super::IsEnemyFound)
	{
		this->TickInterval += DeltaTime;
		if (this->TickInterval > this->TickWaitInterval) 
		{
			this->TickInterval = 0.f;
			Scanning();
		}

	}
}

void AAICharacterBase::Die_Implementation()
{
	if (Super::SelectedWeapon) 
	{
		Super::SelectedWeapon->OnEquip(false);
	}
	Super::Die_Implementation();
}

void AAICharacterBase::NotifyEquip_Implementation()
{
	if (Super::SelectedWeapon)
	{
		FName SocketName = Super::IsEquipWeapon ? Super::SelectedWeapon->WeaponItemInfo.EquipSocketName : Super::SelectedWeapon->WeaponItemInfo.UnEquipSocketName;
		Super::SelectedWeapon->AttachToComponent(Super::GetMesh(), { EAttachmentRule::SnapToTarget, true }, SocketName);
	}
	Super::IsEnemyFound = Super::IsEquipWeapon;
	Super::NotifyEquip_Implementation(); 
}


void AAICharacterBase::OnSeePawnRecieve(APawn* OtherPawn)
{
	AMockCharacter* Player = Cast<AMockCharacter>(OtherPawn);
	if (Player) 
	{
		if (Player->IsDeath_Implementation()) 
		{
			if (Super::IsEquipWeapon) 
			{
				Super::UnEquipment_Implementation();
				Super::PlayAnimMontage(this->EquipMontage, 1.6f, FName(TEXT("None")));
			}
			if (Super::IsEnemyFound)
			{
				Super::IsEnemyFound = false;
			}
			return;
		}
		else 
		{
			// target has not setting yet
			if (Super::GetTarget() == nullptr) 
			{
				Super::SetTargetActor(Player);
			}

			if (!Super::IsEnemyFound)
			{
				Super::IsEnemyFound = true;
				FString message = TEXT("Saw Actor ") + OtherPawn->GetName();
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, message);
				Super::Equipment_Implementation();
				Super::PlayAnimMontage(this->EquipMontage, 1.6f, FName(TEXT("None")));
			}
		}

	}
}

void AAICharacterBase::OnHearNoiseRecieve(APawn * OtherActor, const FVector & Location, float Volume)
{
	const FString VolumeDesc = FString::Printf(TEXT(" at volume %f"), Volume);
	FString message = TEXT("Heard Actor ") + OtherActor->GetName() + VolumeDesc;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, message);
}


// scan tracing
void AAICharacterBase::Scanning()
{
	// not found enemy
	if (Super::SelectedWeapon == nullptr) 
	{
		return;
	}

	ACharacterBase* Character = Super::GetPlayerCharacter();
	if (Character)
	{
		float Distance = FVector::Dist(Character->GetActorLocation(), GetActorLocation());
		//UE_LOG(LogTemp, Log, TEXT("Distance = %d"), Distance);
	}

	const FTransform Transform = Super::SelectedWeapon->GetSkeletalMeshComponent()->GetSocketTransform(Super::SelectedWeapon->GetMuzzleSocket());
	//const FVector Start = Transform.GetLocation();
	const FVector Start = Super::GetActorLocation();
	const FVector End = Start + (Controller->GetControlRotation().Vector() * 5000);

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

	GetWorld()->LineTraceSingleByChannel(HitData, Start, End, ECollisionChannel::ECC_Visibility, fCollisionQueryParams);

	if (HitData.Actor == nullptr) 
	{
		return;
	}

	ICombat* CombatInterface = Cast<ICombat>(HitData.Actor);
	if (CombatInterface)
	{
		this->Activate = true;
	}
	else
	{
		this->Activate = false;
	}
	GLog->Log("On Hit an Actor of " + HitData.Actor->GetName());
	FLinearColor fLineColor = FLinearColor(0.0, 255.0, 0.0, 0.0);
	float Duration  = 0.5;
	float Thickness = 1.0f;
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, fLineColor, Duration, Thickness);

}

