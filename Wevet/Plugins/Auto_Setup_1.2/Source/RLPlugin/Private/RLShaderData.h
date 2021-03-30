#pragma once
#include "CoreMinimal.h"

TMap< FString, FString > g_kTeethMap =
{
    { "Gums Mask", "Teeth Mask Map" },
    { "Gradient AO", "Gradient AO Map" },
    { "MicroNormal", "MicroNormal Map" },

    // MicroNormal
    { "Flip MicroNormal Y", "Flip Micro Normal Y" },
    { "Teeth MicroNormal Tiling", "Teeth MicroNormal Tiling" },
    { "Teeth MicroNormal Strength", "Teeth MicroNormal Strength" },
    { "Gums MicroNormal Tiling", "Flesh MicroNormal Tiling" },
    { "Gums MicroNormal Strength", "Flesh MicroNormal Strength" },

    // TeethGum
    { "Teeth Edge Color", "Teeth Edge Color" },
    { "Is Upper Teeth", "Is Upper Teeth" },
    { "Teeth Brightness", "Teeth Diffuse Brightness" },
    { "Teeth Desaturation", "Teeth Desaturation" },
    { "Gums Brightness", "Flesh Brightness" },
    { "Gums Desaturation", "Flesh Desaturation" },
    { "Front Roughness", "TeethGum Front Roughness" },
    { "Front Specular", "TeethGum Front Specular" },
    // TeethGum-Adv
    { "Front AO", "TeethGum Front AO" },
    { "Back AO", "TeethGum Back AO" },
    { "Back Roughness", "TeethGum Back Roughness" },
    { "Back Specular", "TeethGum Back Specular" },
    // SSS
    { "Gums Scatter", "Flesh Scatter" },
    { "Teeth Scatter", "Teeth Scatter" }
};

TMap< FString, FString > g_kTongueMap =
{
    { "MicroNormal", "MicroNormal Map" },
    { "Gradient AO", "Gradient AO Map" },

    // MicroNormal
    { "Flip MicroNormal Y", "Flip Micro Normal Y" },
    { "MicroNormal Tiling", "Flesh MicroNormal Tiling" },
    { "MicroNormal Strength", "Flesh MicroNormal Strength" },
    // Tongue
    { "_Brightness", "Flesh Brightness" },
    { "_Desaturation", "Flesh Desaturation" },
    { "Front Roughness", "Tongue Front Roughness" },
    { "Front Specular", "Tongue Front Specular" },
    // Tongue-Adv
    { "Back Roughness", "Tongue Back Roughness" },
    { "Back Specular", "Tongue Back Specular" },
    { "Front AO", "Tongue Front AO" },
    { "Back AO", "Tongue Back AO" },
    // SSS
    { "_Scatter", "Flesh Scatter" }
};

TMap< FString, FString > g_kSkinMap =
{
    { "SSS Map", "SSS Map" },
    { "MicroNormalMask", "MicroNormal Mask Map" },
    { "MicroNormal", "MicroNormal Map" },
    { "RGBA Area Mask", "RGBA Area Mask Map" },

    { "_BaseColorMap Brightness", "BaseColorMap_Brightness" },
    { "_BaseColorMap Saturation", "BaseColorMap_Saturation" },
    // MicroNormal
    { "Flip MicroNormal Y", "Flip Micro Normal Y" },
    { "MicroNormal Tiling", "MicroNormal Tiling Value" },
    { "MicroNormal Strength", "MicroNormal Strength" },
    // Roughness
    { "Micro Roughness Scale", "Micro Roughness Scale" },
    { "R Channel Roughness Scale", "R Channel Roughness Scale" },
    { "G Channel Roughness Scale", "G Channel Roughness Scale" },
    { "B Channel Roughness Scale", "B Channel Roughness Scale" },
    { "A Channel Roughness Scale", "A Channel Roughness Scale" },
    { "Unmasked Roughness Scale", "Unmask Roughness Scale" },
    { "Edge Roughness Multiplier", "Edge Roughness Multiplier" },

    // Specular Metallic
    { "_Specular", "Specular" },
    { "SSS Metallic Threshold", "Metallic SSS Threshold" },

    // SSS
    { "G Channel Scatter Scale", "G Channel Scatter" },
    { "R Channel Scatter Scale", "R Channel Scatter" },
    { "B Channel Scatter Scale", "B Channel Scatter" },
    { "A Channel Scatter Scale", "A Channel Scatter" },
    { "Unmasked Scatter Scale", "SSS Unmasked Scale" }
};

TMap< FString, FString > g_kHeadMap =
{
    { "BaseColor Blend2", "Blend Map_2" },
    { "NormalMap Blend", "NormalMap_Blend" },
    { "SSS Map", "SSS Map" },
    { "MicroNormalMask", "MicroNormal Mask Map" },
    { "MicroNormal", "MicroNormal Map" },
    { "Specular Mask", "Specular Cavity Map" },
    { "Mouth Cavity Mask and AO", "Mouth Cavity And AO Map" },
    { "Nose Mouth UpperInnerLid Mask", "NLML Mask Map" },
    { "Cheek Fore UpperLip Chin Mask", "CFUC Mask Map" },
    { "Ear Neck Mask", "EN Mask Map" },

    // Blend Map
    { "BaseColor Blend2 Strength", "BaseColor_Blend2_Strength" },
    { "NormalMap Blend Strength", "NormalMap_Blend_Strength" },
    { "_BaseColorMap Brightness", "BaseColorMap_Brightness" },
    { "_BaseColorMap Saturation", "BaseColorMap_Saturation" },
    // MicroNormal
    { "Flip MicroNormal Y", "Flip Micro Normal Y" },
    { "MicroNormal Tiling", "MicroNormal Tiling Value" },
    { "MicroNormal Strength", "MicroNormal Strength" },
    // Roughness
    { "Micro Roughness Scale", "Micro Roughness Scale" },
    { "Nose Roughness Scale", "Nose Roughness Scale" },
    { "Mouth Roughness Scale", "Mouth Roughness Scale" },
    { "UpperLid Roughness Scale", "UpperLid Roughness Scale" },
    { "InnerLid Roughness Scale", "InnerLid Roughness Scale" },
    { "Ear Roughness Scale", "Ear Roughness Scale" },
    { "Neck Roughness Scale", "Neck Roughness Scale" },
    { "Cheek Roughness Scale", "Cheek Roughness Scale" },
    { "Forehead Roughness Scale", "Forehead Roughness Scale" },
    { "UpperLip Roughness Scale", "UpperLips Roughness Scale" },
    { "Chin Roughness Scale", "Chin Roughness Scale" },
    { "Unmasked Roughness Scale", "Unmask Roughness Scale" },
    { "Edge Roughness Multiplier", "Edge Roughness Multiplier" },
    // Specular Metallic
    { "_Specular", "Head Specular" },
    { "SSS Metallic Threshold", "Metallic SSS Threshold" },
    { "Inner Mouth AO", "Inner Mouth AO" },
    { "Nostril AO", "Nostril AO" },
    { "Lips Gap AO", "Lips_Gap_AO" },
    // SSS
    { "Nose Scatter Scale", "Nose Scatter Scale" },
    { "Mouth Scatter Scale", "Mouth Scatter Scale" },
    { "UpperLid Scatter Scale", "UpperLid Scatter Scale" },
    { "InnerLid Scatter Scale", "InnerLid Scatter Scale" },
    { "Ear Scatter Scale", "Ear Scatter Scale" },
    { "Neck Scatter Scale", "Neck Scatter Scale" },
    { "Cheek Scatter Scale", "Cheek Scatter Scale" },
    { "Forehead Scatter Scale", "Forehead Scatter Scale" },
    { "UpperLip Scatter Scale", "UpperLips Scatter Scale" },
    { "Chin Scatter Scale", "Chin Scatter Scale" },
    { "Unmasked Scatter Scale", "SSS Unmasked Scale" }
};

TMap< FString, FString > g_kEyeMap =
{
    { "Iris Normal", "Iris Normal Map" },
    { "Sclera Normal", "Normal Map" },
    { "EyeBlendMap2", "EyeBlendMap2" },
    { "Inner Iris Mask", "Inner Iris Mask" },
    { "Sclera", "Sclera Map" },

    // Eye
    { "BlendMap2 Strength", "BlendMap2_Strength" },
    { "Shadow Radius", "Shadow Radius" },
    { "Shadow Hardness", "Shadow Hardness" },
    { "Specular Scale", "Specular Multiplier" },
    // Eye-Adv
    { "Is Left Eye", "Is Left Eye" },
    { "Eye Corner Darkness Color", "Eye Corner Darkness Color" },
    // Iris
    { "Iris Depth Scale", "Iris Depth Scale" },
    { "_Iris Roughness", "Iris Roughness" },
    { "Iris Color Brightness", "Iris Color Brightness" },
    { "Pupil Scale", "Pupil Scale" },
    // Iris-Adv
    { "_IoR", "Ior" },
    { "Iris Cloudy Color", "Iris Cloudy Color" },
    { "Iris Inner Color", "Iris Inner Color" },
    { "Iris Inner Scale", "Iris Inner Scale" },
    { "Iris UV Radius", "Iris UV Radius" },
    { "Iris Color", "Iris Color" },

    // Limbus
    { "Limbus UV Width Color", "Limbus UV Width Color" },
    { "Limbus Dark Scale", "Limbus Dark Scale" },
    // Sclera
    { "ScleraBrightness", "Sclera Brightness" },
    { "Sclera Roughness", "Sclera Roughness" },
    // Sclera-Adv
    { "Sclera Flatten Normal", "Sclera Normal" },
    { "Sclera Normal UV Scale", "Sclera Normal UV Scale" },
    { "Sclera UV Radius", "Sclera UV Radius" },
};

#define FLIP_TANGENT_Y  "Flip Tangent Y"
#define ACTIVATE_HAIR_COLOR "Activate Hair Color"

TMap< FString, FString > g_kHairMap =
{
    { "Hair Tangent Map"      , "Tangent Map" },
    { "Hair Flow Map"         , "Flow Map" },
    { "Hair Specular Mask Map", "Specular Map" },
    { "Hair Root Map"         , "Root Map" },
    { "Hair ID Map"           , "ID Map" },

    { "TangentMapFlipGreen", FLIP_TANGENT_Y },
    { "AO Map Occlude All Lighting", "AO Map Occlude All Lighting" },

    { "Diffuse Strength", "Scatter" },
    { "Hair Roughness Map Strength", "Roughness Multiplier" },
    { "Hair Specular Map Strength", "Specular Multiplier" },

    { "VertexGrayToColor"     , "Vertex Color" },
    { "VertexColorStrength"   , "Vertex Color Strength" },
    { "ActiveChangeHairColor" , ACTIVATE_HAIR_COLOR },
    { "BaseColorMapStrength"  , "Base Color Map Strength" },
    // Strand Color
    { "RootColor"             , "Root Color" },
    { "TipColor"              , "End Color" },
    { "RootTipBlendMode"      , "Root End Blend Mode" },
    { "UseRootTipColor"       , "Global Strength" },
    { "RootColorStrength"     , "Root Color Strength" },
    { "TipColorStrength"      , "End Color Strength" },
    { "InvertRootTip"         , "Invert Root and End Color" },
    // highlight A
    { "_1st Dye Color"                      , "Highlight A Color" },
    { "_1st Dye Strength"                   , "Highlight A Strength" },
    { "_1st Dye Distribution from Grayscale", "Highlight A Affected Range" },
    { "_1st BlendMode"                      , "Highlight A Blend Mode" },
    { "Mask 1st Dye by RootMap"             , "Highlight A Overlap End Color" },
    { "Invert 1st Dye RootMap Mask"         , "Highlight A Invert End to Root Color" },
    { "_1st Dye add Specular"               , "Highlight A Specular Strength" },
    // hightlight B
    { "_2nd Dye Color"                      , "Highlight B Color" },
    { "_2nd Dye Strength"                   , "Highlight B Strength" },
    { "_2nd Dye Distribution from Grayscale", "Highlight B Affected Range" },
    { "_2nd BlendMode"                      , "Highlight B Blend Mode" },
    { "Mask 2nd Dye by RootMap"             , "Highlight B Overlap End Color" },
    { "Invert 2nd Dye RootMap Mask"         , "Highlight B Invert End to Root Color" },
    { "_2nd Dye add Specular"               , "Highlight B Specular Strength" },
    // 
    { "BlackColor Reflection Offset X", "Rotate Vertical by Black ID" },
    { "BlackColor Reflection Offset Y", "Rotate Horizontal by Black ID" },
    { "BlackColor Reflection Offset Z", "Shift by Black ID" },
    { "WhiteColor Reflection Offset X", "Rotate Vertical by White ID" },
    { "WhiteColor Reflection Offset Y", "Rotate Horizontal by White ID" },
    { "WhiteColor Reflection Offset Z", "Shift by White ID" },
};

TMap< FString, FString > g_kEyeOccusionMap =
{
    { "Display Blur Range", "Blur Color" },
    { "Blur Strength", "Blur Strength" },
    { "_Expand", "Expand" },

    { "Top Blur Range",             "Top Blur Range" },
    { "Top Blur Contrast",          "Top Blur Contrast" },
    { "Bottom Blur Range",          "Bottom Blur Range" },
    { "Bottom Blur Contrast",       "Bottom Blur Contrast" },
    { "Outer Corner Blur Range",    "Outer Corner Blur Range" },
    { "Outer Corner Blur Contrast", "Outer Corner Blur Contrast" },

    { "Tear Duct Position",      "Tear Duct Position" },
    { "Tear Duct Contrast",      "Tear Duct Contrast" },
    { "Tear Duct Shadow Offset", "Tear Duct Shadow Offset" },
    // 1st layer shadow
    { "Shadow Color",              "Shadow 1 Color" },
    { "Shadow Strength",           "Shadow 1 Strength" },
    { "Shadow Top",                "Shadow 1 Top" },
    { "Shadow Top Range",          "Shadow 1 Top Range" },
    { "Shadow Top Arc",            "Shadow 1 Top Edge" },
    { "Shadow Bottom",             "Shadow 1 Bottom" },
    { "Shadow Bottom Range",       "Shadow 1 Bottom Range" },
    { "Shadow Bottom Arc",         "Shadow 1 Bottom Edge" },
    { "Shadow Inner Corner",       "Shadow 1 Inner Corner" },
    { "Shadow Inner Corner Range", "Shadow 1 Inner Corner Range" },
    { "Shadow Outer Corner",       "Shadow 1 Outer Corner" },
    { "Shadow Outer Corner Range", "Shadow 1 Outer Corner Range" },
    // 2nd_Layer_Shadow
    { "Shadow2 Color",     "Shadow 2 Color" },
    { "Shadow2 Strength",  "Shadow 2 Strength" },
    { "Shadow2 Top",       "Shadow 2 Top" },
    { "Shadow2 Top Range", "Shadow 2 Top Range" },
    // Vertex_Offset
    { "Depth Offset",        "Depth Offset" },
    { "Fade Distance",       "Fade Distance" }, // depth fade distance
    { "Top Offset",          "Top Offset" },
    { "Bottom Offset",       "Bottom Offset" },
    { "Inner Corner Offset", "Inner Corner Offset" },
    { "Outer Corner Offset", "Outer Corner Offset" },
};

TMap< FString, FString > g_kTearLineMap =
{
    { "Depth Offset", "DepthOffset" },
    { "Detail Amount", "DetailAmount" },
    { "Detail Scale U", "DetailScale_U" },
    { "Detail Scale V", "DetailScale_V" },
    { "Edge Fadeout", "Edge_fadeout" },
    { "_Roughness", "Roughness" }
};

#define AO_MAP_OCCLUDE_ALL_LIGHTS "Occlude All Lighting"
#define FLIP_MICRO_NORMAL_Y "Flip Micro Normal Y"

TMap< FString, FString > g_kGeneralMap =
{
    { "SSS Map", "SSS Map" },
    { "MicroNormalMask", "MicroNormal Mask Map" },
    { "MicroNormal", "MicroNormal Map" },
    { "RGBA Area Mask", "RGBA Area Mask Map" },

    { "_BaseColorMap Brightness", "BaseColorMap_Brightness" },
    { "_BaseColorMap Saturation", "BaseColorMap_Saturation" },
    { "AO Map Affect All Lights", AO_MAP_OCCLUDE_ALL_LIGHTS },
    // Micro_Normal
    { "Flip MicroNormal Y", FLIP_MICRO_NORMAL_Y },
    { "MicroNormal Tiling", "MicroNormal Tiling Value" },
    { "MicroNormal Strength", "MicroNormal Strength" },
    // _Specular_Roughness
    { "_Specular", "Specular" },
    { "Micro Roughness Scale", "Micro Roughness Scale" },
    // _Specular_Roughness-Adv
    { "R Channel Roughness Scale", "R Channel Roughness Scale" },
    { "G Channel Roughness Scale", "G Channel Roughness Scale" },
    { "B Channel Roughness Scale", "B Channel Roughness Scale" },
    { "A Channel Roughness Scale", "A Channel Roughness Scale" },
    { "Unmasked Roughness Scale", "Unmask Roughness Scale" },
    { "Edge Roughness Multiplier", "Edge Roughness Multiplier" },

    // SSS
    { "R Channel Scatter Scale", "R Channel Scatter" },
    { "G Channel Scatter Scale", "G Channel Scatter" },
    { "B Channel Scatter Scale", "B Channel Scatter" },
    { "A Channel Scatter Scale", "A Channel Scatter" },
    { "Unmasked Scatter Scale", "SSS Unmasked Scale" },
};

class RLShaderData
{
public:
    RLShaderData() {}
    ~RLShaderData() {}

    FString m_strShaderName;
    TMap< FString, FString > m_kTexture;
    TMap< FString, float > m_kParameter;
    TMap< FString, TArray< float > > m_kColorParameter;
};

class RLScatter
{
public:
    RLScatter() {}
    ~RLScatter() {}

    RLScatter( FLinearColor sss,
               FLinearColor falloff,
               float radius,
               float distribution,
               float IOR,
               float extinction,
               float normalScale,
               float roughness1,
               float roughness2,
               float lobeMix )
        :m_kFalloff( falloff ),
         m_fRadius(radius),
         m_fDistribution(distribution),
         m_fIOR(IOR),
         m_kSubsurfaceColor( sss ),
         m_fExtinctionScale(extinction),
         m_fNormalScale(normalScale),
         m_fRoughness0(roughness1),
         m_fRoughness1(roughness2),
         m_fLobeMix(lobeMix)
    {
    }

    void SetDefaultParameter( FLinearColor kSubsurfaceColor,
                              float fDistribution,
                              float fIOR,
                              float fExtinctionScale,
                              float fNormalScale,
                              float fRoughness0,
                              float fRoughness1,
                              float fLobeMix )
    {
        m_kSubsurfaceColor = kSubsurfaceColor;
        m_fDistribution = fDistribution;
        m_fIOR = fIOR;
        m_fExtinctionScale = fExtinctionScale;
        m_fNormalScale = fNormalScale;
        m_fRoughness0 = fRoughness0;
        m_fRoughness1 = fRoughness1;
        m_fLobeMix = fLobeMix;
    }

    FLinearColor m_kFalloff;
    float m_fRadius = 0.0f;
    float m_fDistribution = 0.0f;
    float m_fIOR = 0.0f;

    FLinearColor m_kSubsurfaceColor;
    float m_fExtinctionScale = 0.0f;
    float m_fNormalScale = 0.0f;
    float m_fRoughness0 = 0.0f;
    float m_fRoughness1 = 0.0f;
    float m_fLobeMix = 0.0f;
};