//
// BloomCombinePixelShader.hlsl
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
// Microsoft DirectXTK Tutorial: PostprocessSpriteTest (https://github.com/walbourn/directxtk-tutorials/blob/master/DX11/PostprocessSpriteTest)
//

// The constants 0.3, 0.59, and 0.11 are chosen because the human eye is more sensitive to green light, and less to blue
static const float3 grayScaleIntensity = { 0.3f, 0.59f, 0.11f };

Texture2D sceneTexture;
Texture2D bloomTexture;
Texture2D textTexture;
SamplerState samplerState;

// Constant buffer

cbuffer BloomCombineBuffer
{
    float bloomIntensity;
    float bloomSaturation;
    float sceneIntensity;
    float sceneSaturation;
    int shouldShowText;
    int3 padding;
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
    float4 textColor = textTexture.Sample(samplerState, input.texCoord);
    
    sceneColor = AdjustSaturation(sceneColor, sceneSaturation) * sceneIntensity;
    bloomColor = AdjustSaturation(bloomColor, bloomSaturation) * bloomIntensity;
    
	// Darken down the base image in areas where there is a lot of bloom to prevent things looking excessively burned-out
    sceneColor *= (1 - saturate(bloomColor));

    return sceneColor + bloomColor + (textColor * shouldShowText);
}
