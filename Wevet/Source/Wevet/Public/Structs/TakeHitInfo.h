#pragma once
#include "Engine/EngineTypes.h"
#include "GameFramework/DamageType.h"
#include "TakeHitInfo.generated.h"

USTRUCT(BlueprintType)
struct WEVET_API FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	float ActualDamage;

	UPROPERTY()
	UClass* DamageTypeClass;

	UPROPERTY()
	TWeakObjectPtr<class Apawn> PawnInstigator;

	UPROPERTY()
	TWeakObjectPtr<class AActor> DamageCauser;

	UPROPERTY()
	uint8 DamageEventClassID;

	UPROPERTY()
	bool bKilled;

private:

	UPROPERTY()
	uint8 EnsureReplicationByte;

	UPROPERTY()
	FDamageEvent GeneralDamageEvent;

	UPROPERTY()
	FPointDamageEvent PointDamageEvent;

	UPROPERTY()
	FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo() 
		: ActualDamage(0),
		DamageTypeClass(nullptr),
		PawnInstigator(nullptr),
		DamageCauser(nullptr),
		DamageEventClassID(0),
		bKilled(false),
		EnsureReplicationByte(0)
	{}

	FDamageEvent& GetDamageEvent()
	{
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			if (PointDamageEvent.DamageTypeClass == nullptr)
			{
				PointDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return PointDamageEvent;

		case FRadialDamageEvent::ClassID:
			if (RadialDamageEvent.DamageTypeClass == nullptr)
			{
				RadialDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return RadialDamageEvent;

		default:
			if (GeneralDamageEvent.DamageTypeClass == nullptr)
			{
				GeneralDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return GeneralDamageEvent;
		}
	}

	void SetDamageEvent(const FDamageEvent& DamageEvent)
	{
		DamageEventClassID = DamageEvent.GetTypeID();
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			PointDamageEvent = *((FPointDamageEvent const*)(&DamageEvent));
			break;
		case FRadialDamageEvent::ClassID:
			RadialDamageEvent = *((FRadialDamageEvent const*)(&DamageEvent));
			break;
		default:
			GeneralDamageEvent = DamageEvent;
		}
	}

	void EnsureReplication()
	{
		++EnsureReplicationByte;
	}
};