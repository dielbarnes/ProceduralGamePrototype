//
// BloomCombinePixelShader.hlsl
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

static const float3 grayScaleIntensity = { 0.299f, 0.587f, 0.114f };

Texture2D sceneTexture;
Texture2D bloomTexture;
SamplerState samplerState;

// Constant buffer

cbuffer BloomCombineBuffer
{
    float bloomIntensity;
    float bloomSaturation;
    float sceneIntensity;
    float sceneSaturation;
};

// Input

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

// Utility function

float4 AdjustSaturation(float4 color, float saturation)
{
    float intensity = dot(color.rgb, grayScaleIntensity);
    return float4(lerp(intensity.rrr, color.rgb, saturation), color.a);
}

// Entry point

float4 PS(PS_INPUT input) : SV_TARGET
{
    float4 sceneColor = sceneTexture.Sample(samplerState, input.texCoord);
    float4 bloomColor = bloomTexture.Sample(samplerState, input.texCoord);
    
    sceneColor = AdjustSaturation(sceneColor, sceneSaturation) * sceneIntensity;
    bloomColor = AdjustSaturation(bloomColor, bloomSaturation) * bloomIntensity;
    
    sceneColor *= (1 - saturate(bloomColor));

    return sceneColor + bloomColor;
}
