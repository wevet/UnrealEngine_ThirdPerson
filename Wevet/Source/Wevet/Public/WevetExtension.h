// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"


namespace Wevet
{
	class WEVET_API ProjectFile
	{
	public:
		static const wchar_t* GetBulletImpactPath()
		{
			return TEXT("/Game/VFX/Cascade/Gameplay/Blood/P_body_bullet_impact");
		};

		static const wchar_t* GetSplashBulletImpactPath()
		{
			return TEXT("/Game/VFX/Cascade/Gameplay/Water/P_splash_bullet_impact");
		};

		static const wchar_t* GetFootStepPath()
		{
			return TEXT("/Game/Sound/FootSteps/Footstep_Dry_Cue");
		};

		static const wchar_t* GetMantleLowPath()
		{
			return TEXT("/Game/Player_Assets/Blueprints/ALS/Curves/MantleCurves/Mantle_1m");
		};

		static const wchar_t* GetMantleHighPath()
		{
			return TEXT("/Game/Player_Assets/Blueprints/ALS/Curves/MantleCurves/Mantle_2m");
		};

#pragma region BlueprintPath

		static const wchar_t* GetPistolBulletPath()
		{
			return TEXT("/Game/Game/Blueprints/Bullet/BP_Pistol_Bullet.BP_Pistol_Bullet_C");
		};

		static const wchar_t* GetRifleBulletPath()
		{
			return TEXT("/Game/Game/Blueprints/Bullet/BP_Rifle_Bullet.BP_Rifle_Bullet_C");
		};

		static const wchar_t* GetSniperBulletPath()
		{
			return TEXT("/Game/Game/Blueprints/Bullet/BP_SniperRifle_Bullet.BP_SniperRifle_Bullet_C");
		};

		static const wchar_t* GetNodeGeneratorPath()
		{
			return TEXT("/Game/Game/Blueprints/Tool/BP_SearchNodeGenerator.BP_SearchNodeGenerator_C");
		};

		static const wchar_t* GetSearchNodePath()
		{
			return TEXT("/Game/Game/Blueprints/Tool/BP_SearchNode.BP_SearchNode_C");
		};

		static const wchar_t* GetBackPackPath()
		{
			return TEXT("/Game/Game/Blueprints/Tool/BP_Backpack.BP_Backpack_C");
		};

		static const wchar_t* GetUMGPath()
		{
			return TEXT("/Game/Game/Blueprints/Widgets/BP_UMGManager.BP_UMGManager_C");
		};
#pragma endregion

		static const wchar_t* GetPickupSoundPath()
		{
			return TEXT("/Engine/VREditor/Sounds/UI/Object_PickUp");
		};
	};


	class WEVET_API ControllerExtension
	{
	public:
		// usage
		// Wevet::ControllerExtension::GetPlayer(this)
		static FORCEINLINE APlayerController* GetPlayer(const UObject* WorldContextObject, int32 PlayerIndex = 0)
		{
			return UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex);
		}

		// usage
		// Wevet::ControllerExtension::GetCameraManager(this);
		static FORCEINLINE APlayerCameraManager* GetCameraManager(const UObject* WorldContextObject, int32 PlayerIndex = 0)
		{
			return UGameplayStatics::GetPlayerCameraManager(WorldContextObject, PlayerIndex);
		}

		// usage
		// Wevet::ControllerExtension::GetPlayerPawn(this)
		static FORCEINLINE APawn* GetPlayerPawn(const UObject* WorldContextObject, int32 PlayerIndex = 0)
		{
			return UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex)->GetPawn();
		}
	};


	class WEVET_API AssetExtension
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
	};


	class WEVET_API ArrayExtension
	{
	public:
		template<typename T>
		static FORCEINLINE bool NullOrEmpty(const TArray<T*> Array)
		{
			return (Array.Num() <= 0);
		}

		template<typename T>
		static FORCEINLINE bool NullOrEmpty(const TArray<T> Array)
		{
			return (Array.Num() <= 0);
		}

		template<typename T>
		static FORCEINLINE bool NullOrEmpty(const TArray<TWeakObjectPtr<T>> Array)
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
			if (!Owner)
			{
				return Array;
			}

			TArray<UActorComponent*> Components;
			Owner->GetComponents(T::StaticClass(), Components, true);
			for (UActorComponent* Component : Components)
			{
				if (T* CustomComp = Cast<T>(Component))
				{
					Array.Add(CustomComp);
					Array.Shrink();
				}
			}
			return Array;
		}

		// usage
		// auto TargetComponent = ComponentExtension::GetComponentFirstOrDefault<USkeletalMeshComponent>(this);
		template<typename T>
		static FORCEINLINE T* GetComponentFirstOrDefault(const AActor* Owner)
		{
			if (!Owner)
			{
				return nullptr;
			}
			TArray<UActorComponent*> Components;
			Owner->GetComponents(T::StaticClass(), Components, true);
			for (UActorComponent* Component : Components)
			{
				if (T * CustomComp = Cast<T>(Component))
				{
					return CustomComp;
				}
			}
			return nullptr;
		}

		static FORCEINLINE bool HasValid(const UActorComponent* Component)
		{
			if (ensure(Component && Component->IsValidLowLevel()))
			{
				return true;
			}
			return false;
		}

	};
}

