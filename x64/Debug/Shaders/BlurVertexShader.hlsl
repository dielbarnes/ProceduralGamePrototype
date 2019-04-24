//
// BlurVertexShader.hlsl
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

// Input/output

struct VS_INPUT
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

// Entry point

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = input.position;
    output.texCoord = input.texCoord;

    return output;
}
