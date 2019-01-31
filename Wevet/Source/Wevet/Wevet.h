// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWevetClient, Verbose, All);

#define SPRITER_IMPORT_ERROR(FormatString, ...) \
	if (!bSilent) { UE_LOG(LogWevetClient, Warning, FormatString, __VA_ARGS__); }
#define SPRITER_IMPORT_WARNING(FormatString, ...) \
	if (!bSilent) { UE_LOG(LogWevetClient, Warning, FormatString, __VA_ARGS__); }

#define DEFAULT_VOLUME 1.f
#define DEFAULT_FORWARD 200.f
#define MONTAGE_DELAY 1.6f


namespace Wevet
{
	class WEVET_API ArrayExtension
	{
	public:
		template<typename T>
		static FORCEINLINE T* GetAssetFromAssetLibrary(const TArray<T*> AssetLibrary, const FString& AssetID)
		{
			for (const UObject* Asset : AssetLibrary)
			{
				T* AssetPtr = Cast<T>(Asset);
				if (!AssetPtr)
				{
					continue;
				}

				if (AssetPtr == AssetID)
				{
					return AssetPtr;
				}
			}
			return nullptr;
		}

		template<typename T>
		static FORCEINLINE bool NullOrEmpty(const TArray<T*> Array)
		{
			return (Array.Num() <= 0);
		}
	};

	class WEVET_API ComponentExtension
	{
	public :
		// usage
		// auto Components = ComponentExtension::GetComponentsArray<USkeletalMeshComponent>(this);
		template<typename T>
		static FORCEINLINE TArray<T*> GetComponentsArray(const AActor* Owner)
		{
			TArray<T*> Array;
			TArray<UActorComponent*> Components = Owner->GetComponentsByClass(T::StaticClass());
			for (UActorComponent* Component : Components)
			{
				if (T* CustomComp = Cast<T>(Component))
				{
					Array.Add(CustomComp);
				}
			}
			return Array;
		}

		static FORCEINLINE bool HasValid(const UActorComponent* Component)
		{
			return Component && Component->IsValidLowLevel();
		}

	};

	class WEVET_API EnumExtension
	{
	public:
		// ETeam Team = ETeam::Alpha;
		// FString Message = TEXT("Our enum value: ") + EnumToString(TEXT("ETeam"), static_cast<uint8>(Team));
		// This prints: "Our enum value: ETeam::Alpha"
		static FORCEINLINE const FString EnumToString(const TCHAR* Enum, const int32 EnumValue)
		{
			const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);
			if (!EnumPtr)
			{
				return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();
			}
#if WITH_EDITOR
			return EnumPtr->GetDisplayNameTextByIndex(EnumValue).ToString();
#else
			return EnumPtr->GetEnumName(EnumValue);
#endif
		}
	};
}

