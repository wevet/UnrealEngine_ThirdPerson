#pragma once
#include "RLTextureData.h"
#include "RLPhysicClothData.h"
#include "RLShaderData.h"

enum class ENodeType
{
    None,
    Hair,
    Accessory
};

class RLMaterialData
{
public:
    RLMaterialData() {}
    ~RLMaterialData() {}

    void SetShaderData( TSharedPtr< RLShaderData > spShaderData ) { m_spShaderData = spShaderData; }
    const RLShaderData* GetShaderData() const { return m_spShaderData ? m_spShaderData.Get() : nullptr; }
    RLShaderData* GetShaderData() { return m_spShaderData ? m_spShaderData.Get() : nullptr; }

    void SetScatter( TSharedPtr< RLScatter > spScatter ) { m_spScatter = spScatter; }
    RLScatter* GetScatter() { return m_spScatter ? m_spScatter.Get() : nullptr; }

    bool m_bIsPbr = false;
    ENodeType m_eNodeType = ENodeType::None;
    bool m_bTwoSide = false;
    int m_iUvChannelIndex = 0;
    TArray< float > m_kDiffuseColor = { 255.f, 255.f, 255.f };
    TArray< float > m_kAmbientColor = { 50.f, 50.f, 50.f };
    TArray< float > m_kSpecularColor = { 229.f, 229.f, 229.f };
    float m_fOpacity = 1.f;
    float m_fSelfIllumination = 0.f;
    float m_fSpecular = 0.f;
    float m_fGlossiness = 0.f;
    TMap< FString, RLTextureData > m_kTextureDatas;
    TSharedPtr< RLPhysicClothData > m_spPhysicClothData = nullptr;

private:
    TSharedPtr< RLShaderData > m_spShaderData;
    TSharedPtr< RLScatter > m_spScatter;
};