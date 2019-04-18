//
// Bloom.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

#include "Bloom.h"

#pragma region Init

Bloom::Bloom(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext)
{
	m_pDevice = pDevice;
	m_pImmediateContext = pImmediateContext;
	m_pVertexShader = nullptr;
	m_pExtractPixelShader = nullptr;
	m_pCombinePixelShader = nullptr;
	m_pVertexInputLayout = nullptr;
	m_pExtractBuffer = nullptr;
	m_pCombineBuffer = nullptr;
	m_pSamplerState = nullptr;
	m_pExtractRenderer = new OffScreenRenderer(pDevice, pImmediateContext);
	m_pBlur = new Blur(pDevice, pImmediateContext);
}

Bloom::~Bloom()
{
	SAFE_RELEASE(m_pVertexShader)
	SAFE_RELEASE(m_pExtractPixelShader)
	SAFE_RELEASE(m_pCombinePixelShader)
	SAFE_RELEASE(m_pVertexInputLayout)
	SAFE_RELEASE(m_pExtractBuffer)
	SAFE_RELEASE(m_pCombineBuffer)
	SAFE_RELEASE(m_pSamplerState)
	SAFE_DELETE(m_pExtractRenderer)
	SAFE_DELETE(m_pBlur)
}

HRESULT Bloom::Initialize(int iWindowWidth, int iWindowHeight)
{
	HRESULT result = S_OK;

	// Compile the vertex shader
	ID3DBlob *pCompiledVertexShader;
	result = Shader::CompileShaderFromFile(L"Shaders/BloomVertexShader.hlsl", "VS", "vs_5_0", &pCompiledVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile bloom vertex shader.", result);
		return result;
	}

	const void *ppCompiledVertexShader = pCompiledVertexShader->GetBufferPointer();
	SIZE_T compiledVertexShaderSize = pCompiledVertexShader->GetBufferSize();

	// Create the vertex shader
	result = m_pDevice->CreateVertexShader(ppCompiledVertexShader,
										   compiledVertexShaderSize,
										   nullptr,						// Class linkage interface for dynamic shader linking
										   &m_pVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create bloom vertex shader.", result);
		return result;
	}

	// Compile the extract pixel shader
	ID3DBlob *pCompiledExtractPixelShader;
	result = Shader::CompileShaderFromFile(L"Shaders/BloomExtractPixelShader.hlsl", "PS", "ps_5_0", &pCompiledExtractPixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile bloom extract pixel shader.", result);
		return result;
	}

	const void *ppCompiledExtractPixelShader = pCompiledExtractPixelShader->GetBufferPointer();
	SIZE_T compiledExtractPixelShaderSize = pCompiledExtractPixelShader->GetBufferSize();

	// Create the extract pixel shader
	result = m_pDevice->CreatePixelShader(ppCompiledExtractPixelShader,
										  compiledExtractPixelShaderSize,
										  nullptr,							// Class linkage interface for dynamic shader linking
										  &m_pExtractPixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create bloom extract pixel shader.", result);
		return result;
	}

	SAFE_RELEASE(pCompiledExtractPixelShader)

	// Compile the combine pixel shader
	ID3DBlob *pCompiledCombinePixelShader;
	result = Shader::CompileShaderFromFile(L"Shaders/BloomCombinePixelShader.hlsl", "PS", "ps_5_0", &pCompiledCombinePixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile bloom combine pixel shader.", result);
		return result;
	}

	const void *ppCompiledCombinePixelShader = pCompiledCombinePixelShader->GetBufferPointer();
	SIZE_T compiledCombinePixelShaderSize = pCompiledCombinePixelShader->GetBufferSize();

	// Create the combine pixel shader
	result = m_pDevice->CreatePixelShader(ppCompiledCombinePixelShader,
										  compiledCombinePixelShaderSize,
										  nullptr,							// Class linkage interface for dynamic shader linking
										  &m_pCombinePixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create bloom combine pixel shader.", result);
		return result;
	}

	SAFE_RELEASE(pCompiledCombinePixelShader)

	// Create the vertex input layout (should be the same as vertex struct)

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

	result = m_pDevice->CreateInputLayout(vertexInputDesc, uiElementCount, ppCompiledVertexShader, compiledVertexShaderSize, &m_pVertexInputLayout);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create bloom vertex input layout.", result);
		return result;
	}

	SAFE_RELEASE(pCompiledVertexShader)

	// Create the extract constant buffer
	// ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(BloomExtractBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// Resource is accessible by both the GPU (read only) and the CPU (write only); good choice for a resource that will be updated by the CPU at least once per frame
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// Bind the buffer as a constant buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pExtractBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create bloom extract buffer.", result);
		return result;
	}

	// Create the combine constant buffer
	bufferDesc.ByteWidth = sizeof(BloomCombineBuffer);
	result = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pCombineBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create bloom combine buffer.", result);
		return result;
	}

	// Create the texture sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// Use linear interpolation for minification, magnification, and mip-level sampling
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;		
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
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
		Utils::ShowError("Failed to create blooom sampler state.", result);
		return false;
	}

	// Initialize off-screen renderer
	result = m_pExtractRenderer->Initialize(iWindowWidth, iWindowHeight);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize blooom extract renderer.", result);
		return result;
	}

	// Initialize blur
	result = m_pBlur->Initialize(iWindowWidth, iWindowHeight);
	if (FAILED(result))
	{
		return result;
	}

	return result;
}

#pragma endregion

#pragma region Getters

ID3D11ShaderResourceView* Bloom::GetExtractTexture()
{
	return m_pExtractRenderer->GetOutputTexture();
}

ID3D11ShaderResourceView* Bloom::GetBlurTexture()
{
	return m_pBlur->GetOutputTexture();
}

#pragma endregion

#pragma region Render

bool Bloom::RenderBloomExtractToTexture(PostProcessQuad *pQuad, ID3D11ShaderResourceView *pSceneTexture)
{
	// Set and clear the render target
	m_pExtractRenderer->SetRenderTarget();

	// Set the vertex input layout
	m_pImmediateContext->IASetInputLayout(m_pVertexInputLayout);

	// Set the vertex shader to the device
	m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);

	// Update the extract constant buffer

	// Lock the extract buffer so it can be written to
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = m_pImmediateContext->Map(m_pExtractBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the bloom extract buffer.", result);
		return false;
	}

	// Get a pointer to the extract buffer data
	BloomExtractBuffer *pExtractBufferData = (BloomExtractBuffer*)mappedResource.pData;

	// Copy the bloom threshold into the extract buffer
	pExtractBufferData->threshold = 0.35f;
	pExtractBufferData->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// Unlock the extract buffer
	m_pImmediateContext->Unmap(m_pExtractBuffer, 0);

	// Set the constant buffers to be used by the pixel shader
	ID3D11Buffer *psConstantBuffers[1] = { m_pExtractBuffer };
	m_pImmediateContext->PSSetConstantBuffers(0, 1, psConstantBuffers);

	// Set the textures to be used by the pixel shader
	m_pImmediateContext->PSSetShaderResources(0, 1, &pSceneTexture);

	// Set the sampler state in the pixel shader
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	// Set the pixel shader to the device
	m_pImmediateContext->PSSetShader(m_pExtractPixelShader, nullptr, 0);

	// Render triangles
	m_pImmediateContext->DrawIndexed(pQuad->GetIndexCount(), 0, 0);

	return true;
}

bool Bloom::RenderHorizontalBlurToTexture(PostProcessQuad *pQuad, ID3D11ShaderResourceView *pInputTexture)
{
	// For debugging only: save bloom extract texture as jpg
	//m_pExtractRenderer->SaveTextureToFile();

	return m_pBlur->RenderHorizontalBlurToTexture(pQuad, pInputTexture);
}

bool Bloom::RenderVerticalBlurToTexture(PostProcessQuad *pQuad)
{
	return m_pBlur->RenderVerticalBlurToTexture(pQuad);
}

bool Bloom::RenderBloomCombine(PostProcessQuad *pQuad, ID3D11ShaderResourceView *pSceneTexture)
{
	// Set the vertex input layout
	m_pImmediateContext->IASetInputLayout(m_pVertexInputLayout);

	// Set the vertex shader to the device
	m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);

	// Update the combine constant buffer

	// Lock the combine buffer so it can be written to
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = m_pImmediateContext->Map(m_pCombineBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the bloom combine buffer.", result);
		return false;
	}

	// Get a pointer to the combine buffer data
	BloomCombineBuffer *pCombineBufferData = (BloomCombineBuffer*)mappedResource.pData;

	// Copy the bloom and scene data into the combine buffer
	pCombineBufferData->bloomIntensity = 2.0f;
	pCombineBufferData->bloomSaturation = 1.0f;
	pCombineBufferData->sceneIntensity = 1.0f;
	pCombineBufferData->sceneSaturation = 1.0f;

	// Unlock the combine buffer
	m_pImmediateContext->Unmap(m_pCombineBuffer, 0);

	// Set the constant buffers to be used by the pixel shader
	ID3D11Buffer *psConstantBuffers[1] = { m_pCombineBuffer };
	m_pImmediateContext->PSSetConstantBuffers(0, 1, psConstantBuffers);

	// Set the textures to be used by the pixel shader
	m_pImmediateContext->PSSetShaderResources(0, 1, &pSceneTexture);
	ID3D11ShaderResourceView *pBloomTexture = m_pBlur->GetOutputTexture();
	m_pImmediateContext->PSSetShaderResources(1, 1, &pBloomTexture);

	// Set the sampler state in the pixel shader
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	// Set the pixel shader to the device
	m_pImmediateContext->PSSetShader(m_pCombinePixelShader, nullptr, 0);

	// Render triangles
	m_pImmediateContext->DrawIndexed(pQuad->GetIndexCount(), 0, 0);

	return true;
}

#pragma endregion
