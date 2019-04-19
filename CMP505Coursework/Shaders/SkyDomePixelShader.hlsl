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

float2 GetCellStar(float2 cellID)
{
    float2 temp = RandFloat2(cellID);
    return float2(temp.x, temp.y) * 0.8f;
}

float4 CreateStarField(PS_INPUT input, float2 domeUV, float f)
{
    float2 uv = domeUV / 2.0f;
	uv *= f;

    float2 gridCell = floor(uv);
    float2 gridCellPoint = GetCellStar(gridCell);
    float2 gridOffset = frac(uv) - 0.5f;
    float2 shrekt = gridOffset - gridCellPoint;

    float oldY = shrekt.y;   
    shrekt.y += sign(shrekt.y) * sqrt(abs(shrekt.x));
    shrekt.x += sign(shrekt.x) * sqrt(abs(oldY));

    float dToAdj = length(shrekt) * (1.0f + sin(t * length(gridCell)) * 0.2f);
	
    float s = smoothstep(0.15f, 0.1f, dToAdj);
    float3 col = float3(s, s, s);

    float3 inPosNorm = normalize(input.domePosition.xyz);
    col = col * step(0, input.domePosition.y);

    return float4(col, 1.0f);
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
    float3 starFieldColor1 = CreateStarField(input, input.domePosition.xy, 20.0f);
    float4 outputColor = max(float4(starFieldColor1, 1.0f), skyDomeColor);
    float3 starFieldColor2 = CreateStarField(input, input.domePosition.xz, 30.0f);
    return max(float4(starFieldColor2, 1.0f), outputColor);
}
