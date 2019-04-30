//
// ColorVertexShader.hlsl
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 4: Buffers, Shaders, and HLSL (http://www.rastertek.com/dx11tut04.html)
//

// Constant buffer

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// Input/output

struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 color : COLOR;
};

// Entry point

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

	// Change the position vector to be 4 units for proper matrix calculations
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the color for the pixel shader
    output.color = input.color;

    return output;
}
