// Copyright © 2018 wevet works All Rights Reserved.


#include "AnimNotifyState/MeleeAttackAnimNotifyState.h"
#include "Character/CharacterBase.h"

UMeleeAttackAnimNotifyState::UMeleeAttackAnimNotifyState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMeleeAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (!Character || Character->IsDeath_Implementation())
	{
		Super::NotifyBegin(MeshComp, Animation, TotalDuration);
		return;
	}

	Character->MeleeAttack(true);
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UMeleeAttackAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UMeleeAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (!Character || Character->IsDeath_Implementation())
	{
		Super::NotifyEnd(MeshComp, Animation);
		return;
	}

	Character->MeleeAttack(false);
	Super::NotifyEnd(MeshComp, Animation);
}
