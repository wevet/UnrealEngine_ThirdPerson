// Copyright © 2018 wevet works All Rights Reserved.


#include "AnimNotify/ReloadAnimNotify.h"
#include "Character/CharacterBase.h"

UReloadAnimNotify::UReloadAnimNotify(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UReloadAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Character->ReloadBulletAction();
	}
	Super::Notify(MeshComp, Animation);
}
