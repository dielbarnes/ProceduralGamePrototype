//
// Blur.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

#include "Blur.h"

#pragma region Init

Blur::Blur(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext)
{
	m_pDevice = pDevice;
	m_pImmediateContext = pImmediateContext;
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pVertexInputLayout = nullptr;
	m_pSampleBuffer = nullptr;
	m_pSamplerState = nullptr;
	m_pOutputTexture = nullptr;
	m_pHorizontalBlurRenderer = new OffScreenRenderer(pDevice, pImmediateContext);
	m_pVerticalBlurRenderer = new OffScreenRenderer(pDevice, pImmediateContext);
	m_fBlurAmount = 8.0f;
}

Blur::~Blur()
{
	SAFE_RELEASE(m_pVertexShader)
	SAFE_RELEASE(m_pPixelShader)
	SAFE_RELEASE(m_pVertexInputLayout)
	SAFE_RELEASE(m_pSampleBuffer)
	SAFE_RELEASE(m_pSamplerState)
	SAFE_RELEASE(m_pOutputTexture)
	SAFE_DELETE(m_pHorizontalBlurRenderer)
	SAFE_DELETE(m_pVerticalBlurRenderer)
}

HRESULT Blur::Initialize(int iWindowWidth, int iWindowHeight)
{
	HRESULT result = S_OK;

	m_windowSize = XMINT2(iWindowWidth, iWindowHeight);

	// Compile the vertex shader
	ID3DBlob *pCompiledVertexShader;
	result = Shader::CompileShaderFromFile(L"Shaders/BlurVertexShader.hlsl", "VS", "vs_5_0", &pCompiledVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile blur vertex shader.", result);
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
		Utils::ShowError("Failed to create blur vertex shader.", result);
		return result;
	}

	// Compile the pixel shader
	ID3DBlob *pCompiledPixelShader;
	result = Shader::CompileShaderFromFile(L"Shaders/BlurPixelShader.hlsl", "PS", "ps_5_0", &pCompiledPixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile blur pixel shader.", result);
		return result;
	}

	const void *ppCompiledPixelShader = pCompiledPixelShader->GetBufferPointer();
	SIZE_T compiledPixelShaderSize = pCompiledPixelShader->GetBufferSize();

	// Create the pixel shader
	result = m_pDevice->CreatePixelShader(ppCompiledPixelShader,
										  compiledPixelShaderSize,
										  nullptr,						// Class linkage interface for dynamic shader linking
										  &m_pPixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create blur pixel shader.", result);
		return result;
	}

	SAFE_RELEASE(pCompiledPixelShader)

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
		Utils::ShowError("Failed to create blur vertex input layout.", result);
		return result;
	}

	SAFE_RELEASE(pCompiledVertexShader)

	// Create the sample constant buffer
	// ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(BlurSampleBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// Resource is accessible by both the GPU (read only) and the CPU (write only); good choice for a resource that will be updated by the CPU at least once per frame
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// Bind the buffer as a constant buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pSampleBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create blur sample buffer.", result);
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
		Utils::ShowError("Failed to create blur sampler state.", result);
		return false;
	}

	// Initialize off-screen renderers

	result = m_pHorizontalBlurRenderer->Initialize(iWindowWidth, iWindowHeight);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize horizontal blur renderer.", result);
		return result;
	}

	result = m_pVerticalBlurRenderer->Initialize(iWindowWidth, iWindowHeight);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize vertical blur renderer.", result);
		return result;
	}

	// Initialize sample offsets and weights
	InitializeSampleOffsets();
	InitializeSampleWeights();

	return result;
}

void Blur::InitializeSampleOffsets()
{
	float fHorizontalPixelSize = 1.0f / m_windowSize.x;
	float fVerticalPixelSize = 1.0f / m_windowSize.y;

	m_horizontalSampleOffsets.resize(BLUR_SAMPLE_COUNT);
	m_verticalSampleOffsets.resize(BLUR_SAMPLE_COUNT);
	m_horizontalSampleOffsets[0] = XMFLOAT2(0.0f, 0.0f);
	m_verticalSampleOffsets[0] = XMFLOAT2(0.0f, 0.0f);

	for (UINT i = 0; i < BLUR_SAMPLE_COUNT / 2; i++)
	{
		float fSampleOffset = i * 2 + 1.5f;
		float fHorizontalOffset = fHorizontalPixelSize * fSampleOffset;
		float fVerticalOffset = fVerticalPixelSize * fSampleOffset;

		m_horizontalSampleOffsets[i * 2 + 1] = XMFLOAT2(fHorizontalOffset, 0.0f);
		m_horizontalSampleOffsets[i * 2 + 2] = XMFLOAT2(-fHorizontalOffset, 0.0f);

		m_verticalSampleOffsets[i * 2 + 1] = XMFLOAT2(0.0f, fVerticalOffset);
		m_verticalSampleOffsets[i * 2 + 2] = XMFLOAT2(0.0f, -fVerticalOffset);
	}
}

void Blur::InitializeSampleWeights()
{
	m_sampleWeights.resize(BLUR_SAMPLE_COUNT);
	m_sampleWeights[0] = GetWeight(0);

	float totalWeight = m_sampleWeights[0];
	for (UINT i = 0; i < BLUR_SAMPLE_COUNT / 2; i++)
	{
		float weight = GetWeight((float)i + 1);
		m_sampleWeights[i * 2 + 1] = weight;
		m_sampleWeights[i * 2 + 2] = weight;
		totalWeight += weight * 2;
	}

	// Normalize the weights so that they sum to one
	for (UINT i = 0; i < m_sampleWeights.size(); i++)
	{
		m_sampleWeights[i] /= totalWeight;
	}
}

float Blur::GetWeight(float f)
{
	return (float)(exp(-(f * f) / (2 * m_fBlurAmount * m_fBlurAmount)));
}

#pragma endregion

#pragma region Getters

ID3D11ShaderResourceView* Blur::GetOutputTexture()
{
	return m_pOutputTexture;
}

#pragma endregion

#pragma region Render

bool Blur::RenderHorizontalBlurToTexture(PostProcessQuad *pQuad, ID3D11ShaderResourceView *pInputTexture)
{
	m_pHorizontalBlurRenderer->SetRenderTarget();
	if (!Render(pQuad, m_horizontalSampleOffsets, pInputTexture))
	{
		return false;
	}

	return true;
}

bool Blur::RenderVerticalBlurToTexture(PostProcessQuad *pQuad)
{
	m_pVerticalBlurRenderer->SetRenderTarget();
	if (!Render(pQuad, m_verticalSampleOffsets, m_pHorizontalBlurRenderer->GetOutputTexture()))
	{
		return false;
	}

	m_pOutputTexture = m_pVerticalBlurRenderer->GetOutputTexture();

	return true;
}

bool Blur::Render(PostProcessQuad *pQuad, std::vector<XMFLOAT2> sampleOffsets, ID3D11ShaderResourceView *pTexture)
{
	// Set the vertex input layout
	m_pImmediateContext->IASetInputLayout(m_pVertexInputLayout);

	// Set the vertex shader to the device
	m_pImmediateContext->VSSetShader(m_pVertexShader,
									 nullptr,			// Array of class instance interfaces used by the vertex shader
									 0);				// Number of class instance interfaces

	// Update the sample constant buffer

	// Lock the sample buffer so it can be written to
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = m_pImmediateContext->Map(m_pSampleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the blur sample buffer.", result);
		return false;
	}

	// Get a pointer to the sample buffer data
	BlurSampleBuffer *pSampleBufferData = (BlurSampleBuffer*)mappedResource.pData;

	// Copy the sample offsets and weights into the sample buffer
	for (int i = 0; i < BLUR_SAMPLE_COUNT; i++)
	{
		pSampleBufferData->sampleOffsets[i] = XMFLOAT4(sampleOffsets[i].x, sampleOffsets[i].y, 0.0f, 0.0f);
	}
	for (int i = 0; i < BLUR_SAMPLE_COUNT; i++)
	{
		pSampleBufferData->sampleWeights[i] = XMFLOAT4(m_sampleWeights[i], 0.0f, 0.0f, 0.0f);
	}

	// Unlock the sample buffer
	m_pImmediateContext->Unmap(m_pSampleBuffer, 0);

	// Set the constant buffers to be used by the pixel shader
	ID3D11Buffer *psConstantBuffers[1] = { m_pSampleBuffer };
	m_pImmediateContext->PSSetConstantBuffers(0, 1, psConstantBuffers);

	// Set the sampler state in the pixel shader
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	// Set the pixel shader to the device
	m_pImmediateContext->PSSetShader(m_pPixelShader,
									 nullptr,			// Array of class instance interfaces used by the pixel shader 
									 0);				// Number of class instance interfaces

	// Set the texture to be used by the pixel shader
	m_pImmediateContext->PSSetShaderResources(0, 1, &pTexture);

	// Render triangles
	m_pImmediateContext->DrawIndexed(pQuad->GetIndexCount(),
									 0,							// Location of the first index read by the GPU from the index buffer
									 0);						// Value added to each index before reading a vertex from the vertex buffer

	return true;
}

#pragma endregion
