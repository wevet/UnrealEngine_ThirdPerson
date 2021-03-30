// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EngineMinimal.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWevetClient, Verbose, All);

#define SPRITER_IMPORT_ERROR(FormatString, ...) \
	if (!bSilent) { UE_LOG(LogWevetClient, Warning, FormatString, __VA_ARGS__); }
#define SPRITER_IMPORT_WARNING(FormatString, ...) \
	if (!bSilent) { UE_LOG(LogWevetClient, Warning, FormatString, __VA_ARGS__); }

#define GETENUMSTRING(etype, evalue)\
	 ((FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetNameStringByIndex((int32)evalue) : FString("Invalid - UENUM() macro?"))

#define DEFAULT_PLAY_RATE 1.f
#define DEFAULT_ANIM_TRANSITION 0.2f

#define MIN_VOLUME 0.001f
#define DEFAULT_VOLUME 1.f

#define MAX_PERCENT 100.f

// Health
#define HALF_WEIGHT 0.5f
#define QUART_WEIGHT 0.25f

#define DEFFENCE_CONST 2

#define MONTAGE_DELAY 1.6f

#define ZERO_VALUE 0.f
#define DEFAULT_VALUE 1.f	 
#define INT_ZERO 0
#define INT_ONE 1

// WeaponDrop OffsetPosition
#define DEFAULT_FORWARD_VECTOR 200.f

// def speeds
#define WALK_SPEED 200.f
#define RUN_SPEED 400.f
#define SPRINT_SPEED 800.f

 
//stencil range
#define STENCIL_MAX 255

// Sockets
#define FPS_SOCKET FName(TEXT("FPS_Socket"))
#define HEAD_SOCKET FName(TEXT("Head_Socket"))
#define CHEST_SOCKET FName(TEXT("Chest_Socket"))
#define PELVIS_SOCKET FName(TEXT("Pelvis_Socket"))
#define BACKPACK_SOCKET FName(TEXT("Backpack_Socket"))

#define HEAD_BONE FName(TEXT("Head"))
#define CHEST_BONE FName(TEXT("spine_03"))
#define PELVIS_BONE FName(TEXT("pelvis"))

#define WATER_TAG FName(TEXT("Water"))
#define WATER_BODY_TAG FName(TEXT("WaterBody"))
#define WATER_LOCAL_TAG FName(TEXT("WaterLocal"))

#define DAMAGE_TAG FName(TEXT("DamageInstigator"))
#define CHARACTER_TAG FName(TEXT("Character"))
#define PLAYER_TAG FName(TEXT("Player"))

#define FULL_BODY FName(TEXT("FullBody"))
#define UPPER_BODY FName(TEXT("UpperBody"))

