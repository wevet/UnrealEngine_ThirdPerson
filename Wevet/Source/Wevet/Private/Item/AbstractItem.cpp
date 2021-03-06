// Copyright 2018 wevet works All Rights Reserved.


#include "Item/AbstractItem.h"
#include "Character/CharacterBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"


AAbstractItem::AAbstractItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollisionComponent"));
	CollisionComponent->SetSphereRadius(90.0f);
	RootComponent = CollisionComponent;

	StaticMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(CollisionComponent);
	StaticMeshComponent->bRenderCustomDepth = false;
	StaticMeshComponent->CustomDepthStencilValue = 0;

	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetDrawSize(FVector2D(250.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 60.f));
	WidgetComponent->SetupAttachment(CollisionComponent);

	static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(Wevet::ProjectFile::GetPickupSoundPath());
	PickupSound = FindAsset.Object;
}


void AAbstractItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}


void AAbstractItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


void AAbstractItem::BeginDestroy()
{
	Super::BeginDestroy();
}


void AAbstractItem::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);

	if (Wevet::ComponentExtension::HasValid(WidgetComponent))
	{
		WidgetComponent->SetVisibility(false);
	}
	AddDelegate();
}


void AAbstractItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Target = nullptr;
	Super::EndPlay(EndPlayReason);
}


void AAbstractItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


#pragma region Interface
void AAbstractItem::Take_Implementation(ACharacterBase* NewCharacter)
{
}


void AAbstractItem::Release_Implementation(ACharacterBase* NewCharacter)
{
	// ItemをPickupする(AbstractWeaponと併せた)
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
	}

	// PlayerのPostprocessが有効だったら、無効にする
	if (NewCharacter)
	{
		NewCharacter->OverlapActor(nullptr);
	}

	Target = nullptr;
	RemoveDelegate();
	UpdateCustomDepth(false);
	Super::SetActorTickEnabled(false);
	WidgetComponent->SetVisibility(false);

	if (IsValidLowLevel())
	{
		Super::Destroy();
		Super::ConditionalBeginDestroy();
	}

}


void AAbstractItem::SpawnToWorld_Implementation()
{
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		StaticMeshComponent->SetEnableGravity(true);
		StaticMeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));
		StaticMeshComponent->SetSimulatePhysics(true);
		StaticMeshComponent->WakeAllRigidBodies();
	}
}


EItemType AAbstractItem::GetItemType_Implementation() const
{
	return WeaponAmmoInfo.BaseItemType;
}
#pragma endregion


#pragma region HitEvent
void AAbstractItem::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor);
	if (Interface == nullptr)
	{
		return;
	}

	if (IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
	{
		UpdateCustomDepth(true);
		IInteractionPawn::Execute_OverlapActor(Interface->_getUObject(), this);
	}
}


void AAbstractItem::EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor);
	if (Interface == nullptr)
	{
		return;
	}

	if (IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
	{
		UpdateCustomDepth(false);
		IInteractionPawn::Execute_OverlapActor(Interface->_getUObject(), nullptr);
	}
}


void AAbstractItem::HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}
#pragma endregion


void AAbstractItem::UpdateCustomDepth(const bool bEnable)
{
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(bEnable);
		StaticMeshComponent->SetCustomDepthStencilValue(bEnable ? (int32)ECustomDepthType::Item : (int32)ECustomDepthType::None);
	}
}


void AAbstractItem::AddDelegate()
{
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAbstractItem::BeginOverlapRecieve);
		CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AAbstractItem::EndOverlapRecieve);
	}
}


void AAbstractItem::RemoveDelegate()
{
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AAbstractItem::BeginOverlapRecieve);
		CollisionComponent->OnComponentEndOverlap.RemoveDynamic(this, &AAbstractItem::EndOverlapRecieve);
	}
}

