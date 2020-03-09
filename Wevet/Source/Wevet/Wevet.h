// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ObjectMacros.h"
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

#define MIN_VOLUME 0.001f
#define DEFAULT_VOLUME 1.f

#define MAX_PERCENT 100.f
#define HALF_WEIGHT 0.5f
#define QUART_WEIGHT 0.25f
#define DEFFENCE_CONST 2

#define MONTAGE_DELAY 1.6f

#define ZERO_VALUE 0.f
#define DEFAULT_VALUE 1.f	 
#define INT_ZERO 0
#define INT_ONE 1

#define DEFAULT_FORWARD_VECTOR 200
#define MAX_WALK_SPEED 800.f
#define MAX_WALK_SPEED_CROUCHED 400.f

#define PTG_TEAM_ID_PLAYER 0
#define PTG_TEAM_ID_ENEMY 1
#define PTG_TEAM_ID_NPC 2
//stencil range
#define STENCIL_MAX 255

