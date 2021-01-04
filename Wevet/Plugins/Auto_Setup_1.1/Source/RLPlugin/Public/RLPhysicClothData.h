#pragma once
#include "CoreMinimal.h"

class RLPhysicClothData
{
public:
    RLPhysicClothData() {}
    ~RLPhysicClothData() {}

    bool m_bActivate = false;
    bool m_bUseGlobalGravity = false;
    FString m_strWeightMapPath = "";
    float m_fMass = 0.f;
    float m_fFriction = 0.f;
    float m_fDamping = 0.f;
    float m_fDrag = 0.f;
    float m_fSolverFrequency = 0.f;
    float m_fTetherLimit = 0.f;
    float m_fElasticity = 0.f;
    float m_fStretch = 0.f;
    float m_fBending = 0.f;
    TArray< float > m_kInertia = { 6.0, 6.0, 6.0 };
    bool m_bSoftVsRigidCollision = false;
    float m_fSoftVsRigidCollisionMargin = 0.f;
    bool m_bSelfCollision = false;
    float m_fSelfCollisionMargin = 0.f;
    float m_fStiffnessFrequency = 0.f;
};