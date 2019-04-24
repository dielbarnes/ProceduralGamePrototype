//
// SkyDomeVertexShader.hlsl
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Terrain Tutorial 7: Sky Domes (http://www.rastertek.com/dx11ter07.html)
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
    float2 texCoord : TEXCOORD0;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float4 domePosition : DOME_POSITION;
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
	output.position = mul(output.position, projectionMatrix).xyww;

	// Store the texture coordinates for the pixel shader
    output.texCoord = input.texCoord;

	// Store the dome position for the pixel shader
	output.domePosition = input.position;

	return output;
}
