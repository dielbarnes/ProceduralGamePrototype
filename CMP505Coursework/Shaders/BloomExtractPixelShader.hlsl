//
// BloomExtractPixelShader.hlsl
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

Texture2D shaderTexture;
SamplerState samplerState;

// Constant buffer

cbuffer BloomExtractBuffer
{
    float threshold;
    float3 padding;
};

// Input

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

// Entry point

float4 PS(PS_INPUT input) : SV_TARGET
{
    float4 outputColor = shaderTexture.Sample(samplerState, input.texCoord);
    return saturate((outputColor - threshold) / (1 - threshold));
}
