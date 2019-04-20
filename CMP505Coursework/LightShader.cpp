//
// LightShader.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 7: 3D Model Rendering (http://www.rastertek.com/dx11tut07.html)
// RasterTek Tutorial 10: Specular Lighting (http://www.rastertek.com/dx11tut10.html)
//

#include "LightShader.h"

#pragma region Init

LightShader::LightShader(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext) : Shader(pDevice, pImmediateContext)
{
	m_pInstanceVertexShader = nullptr;
	m_pInstanceVertexInputLayout = nullptr;
	m_pLightVSBuffer = nullptr;
	m_pLightPSBuffer = nullptr;
	m_pSamplerState = nullptr;
}

LightShader::~LightShader()
{
	SAFE_RELEASE(m_pInstanceVertexShader)
	SAFE_RELEASE(m_pInstanceVertexInputLayout)
	SAFE_RELEASE(m_pLightVSBuffer)
	SAFE_RELEASE(m_pLightPSBuffer)
	SAFE_RELEASE(m_pSamplerState)
}

HRESULT LightShader::Initialize()
{
	HRESULT result = S_OK;

	// Compile and create the vertex shader
	// Compile and create the pixel shader
	// Create the vertex input layout
	// Create the matrix constant buffer

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] =
	{
		{ "POSITION",					// Semantic name
		  0,							// Semantic index (only needed when there is more than one element with the same semantic)
		  DXGI_FORMAT_R32G32B32_FLOAT,	// 96-bit format that supports 32 bits per color channel
		  0,							// Input slot (index of vertex buffer the GPU should fetch ranging form 0 to 15)
		  0,							// Offset in bytes between each element (tells the GPU the memory location to start fetching the data for this element); D3D11_APPEND_ALIGNED_ELEMENT defines the current element directly after the previous one
		  D3D11_INPUT_PER_VERTEX_DATA,  // Input classification
		  0 },							// Number of instances to draw using the same per-instance data before advancing in the buffer by one element (must be 0 for an element that contains per-vertex data)
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT uiElementCount = ARRAYSIZE(vertexInputDesc);

	result = Shader::Initialize(L"Shaders/LightVertexShader.hlsl", "VS", L"Shaders/LightPixelShader.hlsl", "PS", vertexInputDesc, uiElementCount);
	if (FAILED(result))
	{
		return result;
	}

	// Compile the instance vertex shader
	ID3DBlob *pCompiledInstanceVertexShader;
	result = CompileShaderFromFile(L"Shaders/LightInstanceVertexShader.hlsl", "VS", "vs_5_0", &pCompiledInstanceVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile instance vertex shader.", result);
		return result;
	}

	const void *ppCompiledInstanceVertexShader = pCompiledInstanceVertexShader->GetBufferPointer();
	SIZE_T compiledInstanceVertexShaderSize = pCompiledInstanceVertexShader->GetBufferSize();

	// Create the instance vertex shader
	result = m_pDevice->CreateVertexShader(ppCompiledInstanceVertexShader, compiledInstanceVertexShaderSize, nullptr, &m_pInstanceVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create instance vertex shader.", result);
		return result;
	}

	// Create the instance vertex input layout

	D3D11_INPUT_ELEMENT_DESC instanceVertexInputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEX_TILE", 0, DXGI_FORMAT_R32G32_UINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "LIGHT_DIR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	uiElementCount = ARRAYSIZE(instanceVertexInputDesc);

	result = m_pDevice->CreateInputLayout(instanceVertexInputDesc, uiElementCount, ppCompiledInstanceVertexShader, compiledInstanceVertexShaderSize, &m_pInstanceVertexInputLayout);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create instance vertex input layout.", result);
		return result;
	}

	SAFE_RELEASE(pCompiledInstanceVertexShader)

	// Create the light vertex shader constant buffer
	// ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(LightVSBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// Resource is accessible by both the GPU (read only) and the CPU (write only); good choice for a resource that will be updated by the CPU at least once per frame
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// Bind the buffer as a constant buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pLightVSBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create light vertex shader buffer.", result);
		return result;
	}

	// Create the light pixel shader constant buffer
	bufferDesc.ByteWidth = sizeof(LightPSBuffer);
	result = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pLightPSBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create light pixel shader buffer.", result);
		return result;
	}

	// Create the texture sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// Use linear interpolation for minification, magnification, and mip-level sampling
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// Tile the texture when resolving a u texture coordinate that is outside the 0 to 1 range
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;							// Offset from the calculated mipmap level
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;									// Lower end of the mipmap range to clamp access to, where 0 is the largest and most detailed mipmap level and any level higher than that is less detailed
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;					// No upper limit on LOD
	result = m_pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create light sampler state.", result);
		return false;
	}

	return result;
}

#pragma endregion

#pragma region Render

bool LightShader::PreRender(int iInstanceCount, XMINT2 textureTileCount, XMFLOAT4 ambientColor,
							XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, float specularPower,
							XMFLOAT3 lightDirection, XMFLOAT3 pointLightColor, float pointLightStrength,
						    XMFLOAT3 pointLightPosition, Camera *pCamera)
{
	// Set the vertex input layout
	if (iInstanceCount == 1)
	{
		m_pImmediateContext->IASetInputLayout(m_pVertexInputLayout);
	}
	else
	{
		m_pImmediateContext->IASetInputLayout(m_pInstanceVertexInputLayout);
	}

	// Update the light vertex shader constant buffer

	// Lock the light vertex shader buffer so it can be written to
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = m_pImmediateContext->Map(m_pLightVSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the  light vertex shader buffer.", result);
		return false;
	}

	// Get a pointer to the light vertex shader buffer data
	LightVSBuffer *pLightVSBufferData = (LightVSBuffer*)mappedResource.pData;

	// Copy the camera position and the texture tile count into the light vertex shader buffer
	pLightVSBufferData->cameraPosition = pCamera->GetPosition();
	pLightVSBufferData->textureTileCountX = textureTileCount.x;
	pLightVSBufferData->textureTileCountY = textureTileCount.y;
	pLightVSBufferData->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// Unlock the light vertex shader buffer
	m_pImmediateContext->Unmap(m_pLightVSBuffer, 0);

	// Set the constant buffers to be used by the vertex shader
	m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_pLightVSBuffer);

	// Set the vertex shader to the device
	if (iInstanceCount == 1)
	{
		m_pImmediateContext->VSSetShader(m_pVertexShader,
										 nullptr,			// Array of class instance interfaces used by the vertex shader
										 0);				// Number of class instance interfaces
	}
	else
	{
		m_pImmediateContext->VSSetShader(m_pInstanceVertexShader, nullptr, 0);
	}

	// Update the light pixel shader constant buffer

	// Lock the light pixel shader buffer so it can be written to
	result = m_pImmediateContext->Map(m_pLightPSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the light pixel shader buffer.", result);
		return false;
	}

	// Get a pointer to the light pixel shader buffer data
	LightPSBuffer *pLightPSBufferData = (LightPSBuffer*)mappedResource.pData;

	// Copy the light data into the light pixel shader buffer
	pLightPSBufferData->ambientColor = ambientColor;
	pLightPSBufferData->diffuseColor = diffuseColor;
	pLightPSBufferData->specularColor = specularColor;
	pLightPSBufferData->specularPower = specularPower;
	pLightPSBufferData->lightDirection = lightDirection;
	pLightPSBufferData->pointLightColor = pointLightColor;
	pLightPSBufferData->pointLightStrength = pointLightStrength;
	pLightPSBufferData->pointLightPosition = pointLightPosition;
	pLightPSBufferData->instanceCount = iInstanceCount;

	// Unlock the light pixel shader buffer
	m_pImmediateContext->Unmap(m_pLightPSBuffer, 0);

	// Set the constant buffers to be used by the pixel shader
	ID3D11Buffer *psConstantBuffers[1] = { m_pLightPSBuffer };
	m_pImmediateContext->PSSetConstantBuffers(0, 1, psConstantBuffers);

	// Set the sampler state in the pixel shader
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	// Set the pixel shader to the device
	m_pImmediateContext->PSSetShader(m_pPixelShader,
									 nullptr,			// Array of class instance interfaces used by the pixel shader 
									 0);				// Number of class instance interfaces

	return true;
}

bool LightShader::Render(TxtModel *pModel, Camera *pCamera)
{
	std::vector<ID3D11ShaderResourceView*> textures;
	textures.push_back(pModel->GetTexture());

	if (!PreRender(pModel->GetInstanceCount(), pModel->GetTextureTileCount(), pModel->GetAmbientColor(),
				   pModel->GetDiffuseColor(), pModel->GetSpecularColor(), pModel->GetSpecularPower(),
				   pModel->GetLightDirection(), pModel->GetPointLightColor(), pModel->GetPointLightStrength(), 
				   pModel->GetPointLightPosition(), pCamera))
	{
		return false;
	}

	Render(pModel->GetInstanceCount(), pModel->GetWorldMatrix(), textures, pModel->GetIndexCount(), pCamera);

	return true;
}

bool LightShader::PreRender(Model *pModel, Camera *pCamera)
{
	return PreRender(pModel->GetInstanceCount(), XMINT2(1, 1), pModel->GetAmbientColor(), pModel->GetDiffuseColor(), pModel->GetSpecularColor(),
					 pModel->GetSpecularPower(), pModel->GetLightDirection(), pModel->GetPointLightColor(), pModel->GetPointLightStrength(),
					 pModel->GetPointLightPosition(), pCamera);
}

void LightShader::Render(Mesh *pMesh, Camera *pCamera)
{
	Render(pMesh->GetInstanceCount(), pMesh->GetWorldMatrix(), pMesh->GetTextures(), pMesh->GetIndexCount(), pCamera);
}

void LightShader::Render(int iInstanceCount, XMMATRIX worldMatrix, std::vector<ID3D11ShaderResourceView*> textures, 
						 int iIndexCount, Camera *pCamera)
{
	// Update and set the matrix constant buffer to be used by the vertex shader
	Shader::SetMatrixBuffer(worldMatrix, pCamera->GetViewMatrix(), pCamera->GetProjectionMatrix());

	// Set the texture to be used by the pixel shader
	m_pImmediateContext->PSSetShaderResources(0, 1, &textures[0]);

	// Render triangles
	if (iInstanceCount == 1)
	{
		m_pImmediateContext->DrawIndexed(iIndexCount,
										 0,				// Location of the first index read by the GPU from the index buffer
										 0);			// Value added to each index before reading a vertex from the vertex buffer
	}
	else
	{
		m_pImmediateContext->DrawIndexedInstanced(iIndexCount, iInstanceCount, 0, 0, 0);
	}
}

#pragma endregion
