#pragma once
#include "CoreMinimal.h"

class RLTextureData
{
public:
    RLTextureData() {}
    ~RLTextureData() {}

    FString m_strTexturePath = "";
    bool m_bShareImage = false;
    float m_fStrength = 100.f;
    TArray< float > m_kOffset = { 0.f, 0.f };
    TArray< float > m_kTiling = { 1.f, 1.f };
    float m_fTessellationLevel = 0.f;
    float m_fTessellationMulitiplier = 0.f;
    float m_fGrayScaleBaseValue = 0.f;
    FString m_strDisplacementMapType = "";
    FString m_strBlendMode = "Multiply";
};