//
// SkyDomePixelShader.hlsl
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Terrain Tutorial 7: Sky Domes (http://www.rastertek.com/dx11ter07.html)
//

// Constant buffer

cbuffer ColorBuffer
{
	float4 topColor;
	float4 centerColor;
	float4 bottomColor;
};

// Input

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 domePosition : TEXCOORD0;
};

// Entry point

float4 PS(PS_INPUT input) : SV_TARGET
{
	// Determine the position of the pixel on the sky dome
	float height = input.domePosition.y;

	// The value ranges from -1.0f to 1.0f so change it to only positive values
	if (height < 0.0f)
	{
		height = 0.0f;
	}

	// Determine the gradient color by interpolating between colors based on the height of the pixel
	float4 outputColor = lerp(bottomColor, centerColor, height);
	outputColor = lerp(outputColor, topColor, height / 2.0f);

	return outputColor;
}
