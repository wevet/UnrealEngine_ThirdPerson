#pragma once
#include "CoreMinimal.h"

class RLPhysicsCollisionShapeData
{
public:
    RLPhysicsCollisionShapeData() {}
    ~RLPhysicsCollisionShapeData() {}

    FString m_strName = "";
    bool m_bBoneActivate = false;
    bool m_bIsCCStdBoneAxis = false;
    FString m_strBoundType = "";
    FString m_strBoundAxis = "";
    float m_fMargin = 0.f;
    float m_fFriction = 0.f;
    float m_fElasticity = 0.f;

    TArray< float > m_kWorldTranslate = { 0.0, 0.0, 0.0 };
    TArray< float > m_kWorldRotation = { 0.0, 0.0, 0.0, 0.0 };
    TArray< float > m_kWorldScale = { 0.0, 0.0, 0.0 };

    TArray< float > m_kShapeLocalPosition = { 0.0, 0.0, 0.0 };
    TArray< float > m_kExtent = { 0.0, 0.0, 0.0 };
    float m_fRadius = 0.f;
    float m_fCapsuleLength = 0.f;
};