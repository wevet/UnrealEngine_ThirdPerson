// Copyright 2018 wevet works All Rights Reserved.

#include "AI/AICharacterBase.h"
#include "AI/AIControllerBase.h"
#include "AnimInstance/AICharacterAnimInstanceBase.h"
#include "Perception/AiPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WevetExtension.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"
#include "Widget/AIHealthController.h"


AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer),
	bAttackInitialized(false)
{
	PrimaryActorTick.bCanEverTick = true;

	GetMovementComponent()->NavAgentProps.AgentRadius = 42;
	GetMovementComponent()->NavAgentProps.AgentRadius = 192;

	BulletDelay = 0.3f;
	WalkingSpeed = 300.f;
	WidgetViewPortOffset = FVector2D(0.5f, 0.0f);
	Tags.Add(DAMAGE_TAG);
	Tags.Add(WATER_TAG);

	GetMesh()->ComponentTags.Add(WATER_LOCAL_TAG);

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(TEXT("/Game/Game/Blueprints/Tool/BP_SearchNodeGenerator.BP_SearchNodeGenerator_C"));
		NodeHolderTemplate = FindAsset.Object;
	}

	// TeamID = 1
}

void AAICharacterBase::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
}

void AAICharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAICharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyHealthController();
	Super::EndPlay(EndPlayReason);
}

void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	TFunction<void(AAbstractWeapon*)> Func = [this](AAbstractWeapon* Weapon)
	{
		CurrentWeapon = MakeWeakObjectPtr<AAbstractWeapon>(Weapon);
	};

	Super::CreateWeaponInstance(PrimaryWeapon, Func);
	Super::CreateWeaponInstance(SecondaryWeapon, [&](AAbstractWeapon* Weapon)
	{
		//
	});

	//CreateHealthController();
	
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAICharacterBase::InitializePosses()
{
	AIController = Cast<AAIControllerBase>(GetController());
}

#pragma region Ragdoll
void AAICharacterBase::StartRagdollAction()
{
	Super::StartRagdollAction();
	if (AIController)
	{
		AIController->SetBlackboardTarget(nullptr);
		AIController->StopTree();
	}

	SetStanning(true);
	if (HasEquipWeapon())
	{
		CurrentWeapon.Get()->SetEquip(false);
	}
}

void AAICharacterBase::RagdollToWakeUpAction()
{
	if (IDamageInstigator::Execute_IsDeath(this))
	{
		return;
	}

	Super::RagdollToWakeUpAction();
	if (AIController)
	{
		AIController->ResumeTree();
	}
	SetStanning(false);
}
#pragma endregion

#pragma region Interface
float AAICharacterBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (!IDamageInstigator::Execute_IsDeath(this))
	{
	}
	else
	{
		//
	}
	return ActualDamage;
}

void AAICharacterBase::Die_Implementation()
{
	if (Super::bWasDied)
	{
		return;
	}

	Tags.Reset(0);
	Tags.Add(FName(TEXT("DeadBody")));

	RemoveSearchNodeGenerator();

	UWevetBlueprintFunctionLibrary::DrawDebugString(this, FString(TEXT("DeadBody")), FLinearColor::Red);

	Super::SetActorTickEnabled(false);
	DestroyHealthController();
	TargetCharacter = nullptr;

	if (DeathDelegate.IsBound())
	{
		DeathDelegate.Broadcast();
	}

	Super::GetMesh()->SetRenderCustomDepth(false);
	Super::Die_Implementation();

	// Remove AIController
	DetachFromControllerPendingDestroy();
}

void AAICharacterBase::Equipment_Implementation()
{
	if (!Super::HasEquipWeapon())
	{
		Super::Equipment_Implementation();
	}
}

void AAICharacterBase::UnEquipment_Implementation()
{
	if (Super::HasEquipWeapon())
	{
		Super::UnEquipment_Implementation();
	}
}

float AAICharacterBase::GetMeleeDistance_Implementation() const
{
	if (!CurrentWeapon.IsValid())
	{
		return ZERO_VALUE;
	}
	return CurrentWeapon.Get()->GetWeaponItemInfo().MeleeDistance;
}

void AAICharacterBase::CombatStateChange_Implementation(const EAICombatState NewAICombatState)
{
	if (AIController)
	{
		AIController->SetBlackboardCombatState(NewAICombatState);
	}
}

void AAICharacterBase::ActionStateChange_Implementation(const EAIActionState NewAIActionState)
{
	if (AIController)
	{
		AIController->SetBlackboardActionState(NewAIActionState);
	}
}

bool AAICharacterBase::CanMeleeStrike_Implementation() const
{
	if (!TargetCharacter || !CurrentWeapon.IsValid() || IDamageInstigator::Execute_IsDeath(TargetCharacter))
	{
		return false;
	}

	if (!CurrentWeapon.Get()->WasEquip())
	{
		return false;
	}

	const FVector TargetLocation = TargetCharacter->GetActorLocation();
	const float Distance = (GetActorLocation() - TargetLocation).Size();
	const bool bWasResult = (GetMeleeDistance_Implementation() > Distance);
	return bWasResult;
}
#pragma endregion

void AAICharacterBase::OnSightStimulus(AActor* const Actor, const FAIStimulus InStimulus, const bool InWasDeadCrew)
{
	if (AIController == nullptr)
	{
		return;
	}
	
	AIController->SetBlackboardTarget(Actor);
	EquipmentActionMontage();
}

void AAICharacterBase::OnHearStimulus(AActor* const Actor, const FAIStimulus InStimulus, const bool InWasDeadCrew)
{
	if (AIController == nullptr)
	{
		return;
	}
	UpdateSearchNodeHolder(InStimulus.StimulusLocation);
	EquipmentActionMontage();
}

void AAICharacterBase::OnPredictionStimulus(AActor* const Actor, const FAIStimulus InStimulus)
{
	if (AIController == nullptr)
	{
		return;
	}
	UE_LOG(LogWevetClient, Log, TEXT("Prediction Sense : %f"), InStimulus.GetAge());
}

void AAICharacterBase::EquipmentActionMontage()
{
	if (ALSMovementMode != ELSMovementMode::Grounded)
	{
		return;
	}

	if (!HasEquipWeapon())
	{
		Super::EquipmentActionMontage();
	}
}

void AAICharacterBase::UnEquipmentActionMontage()
{
	if (ALSMovementMode != ELSMovementMode::Grounded)
	{
		return;
	}

	if (HasEquipWeapon())
	{
		Super::UnEquipmentActionMontage();
	}
}

#pragma region Attack
void AAICharacterBase::AttackInitialize(const float InInterval, const float InTimeOut)
{
	if (!bAttackInitialized)
	{
		BulletInterval = InInterval;
		MeleeAttackTimeOut = InTimeOut;
		bAttackInitialized = true;
	}
}

void AAICharacterBase::AttackUnInitialize()
{
	MeleeAttackTimeOut = ZERO_VALUE;
	bAttackInitialized = false;
}
#pragma endregion

FVector AAICharacterBase::BulletTraceForwardLocation() const
{
	if (CurrentWeapon.IsValid())
	{
		const FTransform MuzzleTransform = CurrentWeapon.Get()->GetMuzzleTransform();
		const float Distance = CurrentWeapon.Get()->GetTraceDistance();
		const FRotator MuzzleRotation = FRotator(MuzzleTransform.GetRotation());
		const FVector MuzzleLocation = MuzzleTransform.GetLocation();

		if (TargetCharacter)
		{
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, TargetCharacter->GetActorLocation());
			//MuzzleRotation.Yaw = LookAtRotation.Yaw;
		}
		//UKismetMathLibrary::GetForwardVector(MuzzleRotation)
		return MuzzleLocation + (MuzzleRotation.Vector() * Distance);
	}
	return GetActorForwardVector();
}

void AAICharacterBase::Sprint()
{
	Super::Sprint();
	if (bSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
}

void AAICharacterBase::StopSprint()
{
	Super::StopSprint();
	if (!bSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	}
}

void AAICharacterBase::CreateHealthController()
{
	if (UIHealthController || !UIHealthControllerTemplate)
	{
		return;
	}

	UIHealthController = CreateWidget<UAIHealthController>(GetWorld(), UIHealthControllerTemplate);
	if (UIHealthController)
	{
		UIHealthController->AddToViewport((int32)EUMGLayerType::WorldScreen);
		UIHealthController->SetViewPortOffset(WidgetViewPortOffset);
		UIHealthController->Initializer(this);
	}

}

void AAICharacterBase::DestroyHealthController()
{
	if (UIHealthController)
	{
		UIHealthController->ResetCharacterOwner();
		UIHealthController->RemoveFromParent();
		UIHealthController->ConditionalBeginDestroy();
		UIHealthController = nullptr;
	}
}

// Run to EQS
void AAICharacterBase::UpdateSearchNodeHolder(const FVector SearchOriginLocation)
{
	if (NodeHolderTemplate == nullptr || AIController == nullptr)
	{
		UE_LOG(LogWevetClient, Error, TEXT("NodeHolderTemplate or AIController nullptr"));
		return;
	}


	const FTransform Transform = UKismetMathLibrary::MakeTransform(SearchOriginLocation, FRotator::ZeroRotator, FVector::OneVector);
	ASearchNodeGenerator* const SpawningObject = UWevetBlueprintFunctionLibrary::SpawnActorDeferred<ASearchNodeGenerator>(
		this,
		NodeHolderTemplate,
		Transform,
		this);

	if (!SpawningObject)
	{
		return;
	}

	RemoveSearchNodeGenerator();

	SpawningObject->FinishSpawning(Transform);
	AIController->SetBlackboardSearchNodeHolder(SpawningObject);
	AIController->SetBlackboardDestinationLocation(SearchOriginLocation);
}

// Refresh to SearchNodeGenerator
void AAICharacterBase::RemoveSearchNodeGenerator()
{
	ASearchNodeGenerator* NodeHolder = Cast<ASearchNodeGenerator>(AIController->GetBlackboardSearchNodeHolder());
	if (NodeHolder)
	{
		NodeHolder->PrepareDestroy();
		NodeHolder->Destroy();
	}
}

