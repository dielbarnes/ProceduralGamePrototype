//
// ColorPixelShader.hlsl
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 4: Buffers, Shaders, and HLSL (http://www.rastertek.com/dx11tut04.html)
//

// Input

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// Entry point

float4 PS(PS_INPUT input) : SV_TARGET
{
    return input.color;
}
