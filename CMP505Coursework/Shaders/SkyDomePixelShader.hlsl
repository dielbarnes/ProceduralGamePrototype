//
// SkyDomePixelShader.hlsl
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Terrain Tutorial 7: Sky Domes (http://www.rastertek.com/dx11ter07.html)
// Stars_jb94 (https://www.shadertoy.com/view/3tsGRH)
//

// Constant buffer

cbuffer SkyColorBuffer
{
	float4 topColor;
	float4 centerColor;
	float4 bottomColor;
    float time;
    float3 padding;
};

// Input

struct PS_INPUT
{
	float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float4 domePosition : DOME_POSITION;
};

// Utility functions

#define t time * 0.0003f

float N2(float2 p)
{
	// Reference:
	// Hash without Sine (https://www.shadertoy.com/view/4djSRW)

    float3 p3 = frac(float3(p.xyx) * float3(443.897f, 441.423f, 437.195f));
    p3 += dot(p3, p3.yzx + 19.19f);
    return frac((p3.x + p3.y) * p3.z);
}

float2 RandFloat2(float2 p)
{
    return float2(N2(p), N2(p.yx)) - 0.5f;
}

float2 GetCellPoint(float2 cellId)
{
    float2 rand = RandFloat2(cellId);
    return rand * 0.8f;
}

float4 CreateStarField(PS_INPUT input, float2 domeUv, float uvFactor)
{
    float2 uv = domeUv / 2.0f;
    uv *= uvFactor;

    float2 gridCell = floor(uv); // Give each cell an id
    float2 gridCellPoint = GetCellPoint(gridCell); // Generate a random point within the cell
    float2 gridOffset = frac(uv) - 0.5f; // Get the fractional part of uv so that it is between 0 and 1, then subtract 0.5 so that (0, 0) is at the center of the cell and it becomes from -0.5 to 0.5
    float2 rect = gridOffset - gridCellPoint;

	// Give the star a diamond shape
    float oldY = rect.y;
    rect.y += sign(rect.y) * sqrt(abs(rect.x));
    rect.x += sign(rect.x) * sqrt(abs(oldY));

	// Use sin function to make the star twinkle
    float dToAdj = length(rect) * (1.0f + sin(t * length(gridCell)) * 0.2f);
	
    float s = smoothstep(0.15f, 0.1f, dToAdj); // Decrease opacity as distance from the center increases
    float3 color = float3(s, s, s);
    color = color * step(0, input.domePosition.y); // Display stars only at the top half of the dome
    return float4(color, 1.0f);
}

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
	float4 skyDomeColor = lerp(bottomColor, centerColor, height);
    skyDomeColor = lerp(skyDomeColor, topColor, height / 2.0f);

	// Create star fields
    float4 starFieldColor1 = CreateStarField(input, input.domePosition.xy, 20.0f);
    float4 outputColor = max(starFieldColor1, skyDomeColor);
    float4 starFieldColor2 = CreateStarField(input, input.domePosition.xz, 30.0f);
    return max(starFieldColor2, outputColor);
}
