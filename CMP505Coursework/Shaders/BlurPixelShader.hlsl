//
// BlurPixelShader.hlsl
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

#define BLUR_SAMPLE_COUNT 9

Texture2D shaderTexture;
SamplerState samplerState;

// Constant buffer

cbuffer BlurSampleBuffer
{
    float4 sampleOffsets[BLUR_SAMPLE_COUNT];
    float4 sampleWeights[BLUR_SAMPLE_COUNT];
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
	float4 outputColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < BLUR_SAMPLE_COUNT; i++)
    {
        outputColor += shaderTexture.Sample(samplerState, input.texCoord + float2(sampleOffsets[i].x, sampleOffsets[i].y)) * sampleWeights[i].x;
    }
	
    return outputColor;
}
