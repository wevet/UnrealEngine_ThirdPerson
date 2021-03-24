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


AAICharacterBase::AAICharacterBase(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	WidgetViewPortOffset = FVector2D(0.5f, 0.0f);

	Tags.Add(WATER_TAG);
	GetMesh()->ComponentTags.Add(WATER_LOCAL_TAG);

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(Wevet::ProjectFile::GetNodeGeneratorPath());
		NodeHolderTemplate = FindAsset.Object;
	}

	// TeamID = 1
}

void AAICharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyHealthController();
	Super::EndPlay(EndPlayReason);
}

void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	WeaponFunc Func = [this](AAbstractWeapon* Weapon)
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

float AAICharacterBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (!ICombatInstigator::Execute_IsDeath(this))
	{
		//
	}
	return ActualDamage;
}

#pragma region Ragdoll
void AAICharacterBase::StartRagdollAction()
{
	if (ICombatInstigator::Execute_IsDeath(this))
	{
		return;
	}

	TargetCharacter = nullptr;
	Super::StartRagdollAction();

	if (AIController)
	{
		AIController->SetBlackboardTarget(nullptr);
		AIController->StopTree();
	}

	RemoveSearchNodeGenerator();
}

void AAICharacterBase::RagdollToWakeUpAction()
{
	if (ICombatInstigator::Execute_IsDeath(this))
	{
		return;
	}

	Super::RagdollToWakeUpAction();

	if (AIController)
	{
		AIController->ResumeTree();
	}
}
#pragma endregion

#pragma region Interface
void AAICharacterBase::Initializer_Implementation()
{
	AIController = Cast<AAIControllerBase>(GetController());
}

void AAICharacterBase::Die_Implementation()
{
	if (Super::bWasDied)
	{
		return;
	}

	Tags.Reset(0);
	Tags.Add(FName(TEXT("DeadBody")));

	//UWevetBlueprintFunctionLibrary::DrawDebugString(this, FString(TEXT("DeadBody")), FLinearColor::Red);

	if (AIController)
	{
		AIController->SetBlackboardTarget(nullptr);
	}

	Super::SetActorTickEnabled(false);
	RemoveSearchNodeGenerator();
	DestroyHealthController();

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

void AAICharacterBase::OnActionStateChange_Implementation()
{
	check(AIController);
	AIController->SetBlackboardActionState(ActionState);
}

void AAICharacterBase::DoSightReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew)
{
	check(AIController);

	if (TargetCharacter)
	{
		return;
	}

	if (ACharacterBase* Character = Cast<ACharacterBase>(Actor))
	{
		TargetCharacter = Character;
		AIController->SetBlackboardTarget(TargetCharacter);
		EquipmentActionMontage();
	}
}

void AAICharacterBase::DoHearReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew)
{
	check(AIController);

	// Is it a class that inherits ICombatInstigator instead of the same class?
	if (Actor->GetClass()->ImplementsInterface(UCombatInstigator::StaticClass()))
	{
		CreateSearchNodeGenerator(InStimulus.StimulusLocation);
		EquipmentActionMontage();
	}

}

void AAICharacterBase::DoPredictionReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus)
{
	check(AIController);

	// check same classes
	if (Actor->GetClass() == GetClass())
	{
		return;
	}

	if (Actor->GetClass()->ImplementsInterface(UCombatInstigator::StaticClass()))
	{
		UE_LOG(LogWevetClient, Log, TEXT("Prediction Sense : %f"), InStimulus.GetAge());
	}
	else
	{
		// Another class
	}
}

void AAICharacterBase::DoDamageReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus)
{
	UE_LOG(LogWevetClient, Log, TEXT("Damage Sense : %f"), InStimulus.GetAge());
}
#pragma endregion

#pragma region Widget
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
#pragma endregion

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

// Run to EQS
void AAICharacterBase::CreateSearchNodeGenerator(const FVector SearchOriginLocation)
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

