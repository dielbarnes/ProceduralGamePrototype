//
// LightPixelShader.hlsl
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 7: 3D Model Rendering (http://www.rastertek.com/dx11tut07.html)
// RasterTek Tutorial 10: Specular Lighting (http://www.rastertek.com/dx11tut10.html)
//

Texture2D shaderTexture;
SamplerState samplerState;

// Constant buffer

cbuffer LightPSBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float4 specularColor;
	float specularPower;
	float3 lightDirection;
};

// Input

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

// Entry point

float4 PS(PS_INPUT input) : SV_TARGET
{
	// Set the default output color to the ambient light value for all pixels
	float4 outputColor = ambientColor;

	// Invert the light direction and calculate the amount of light on this pixel
	float lightIntensity = saturate(dot(input.normal, -lightDirection));

	// Initialize the specular light
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	if (lightIntensity > 0.0f)
	{
		// Determine the final amount of diffuse color
		outputColor += (diffuseColor * lightIntensity);

		// Calculate the reflection vector
		float3 reflection = normalize(2 * lightIntensity * input.normal - lightDirection);

		// Determine the amount of specular light
		specular = specularColor * pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	}

	// Saturate the ambient color and the diffuse color
	outputColor = saturate(outputColor);

	// Sample the pixel color from the texture using the sampler at this texture coordinate
	float4 textureColor = shaderTexture.Sample(samplerState, input.texCoord);

	// Multiply the texture color
	outputColor *= textureColor;

	// Add the specular component last
	outputColor = saturate(outputColor + specular);

	return outputColor;
}
