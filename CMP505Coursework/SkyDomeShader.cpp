//
// SkyDomeShader.cpp
// Copyright � 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Terrain Tutorial 7: Sky Domes (http://www.rastertek.com/dx11ter07.html)
//

#include "SkyDomeShader.h"

#pragma region Init

SkyDomeShader::SkyDomeShader(ID3D11Device &device, ID3D11DeviceContext &immediateContext) : Shader(device, immediateContext)
{
	m_pColorBuffer = nullptr;
}

SkyDomeShader::~SkyDomeShader()
{
	SAFE_RELEASE(m_pColorBuffer)
}

HRESULT SkyDomeShader::Initialize()
{
	HRESULT result = S_OK;

	// Compile and create the vertex shader
	// Compile and create the pixel shader
	// Create the vertex input layout
	// Create the matrix constant buffer

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT uiElementCount = ARRAYSIZE(vertexInputDesc);

	result = Shader::Initialize(L"Shaders/SkyDomeVertexShader.hlsl", "VS", L"Shaders/SkyDomePixelShader.hlsl", "PS", vertexInputDesc, uiElementCount);
	if (FAILED(result))
	{
		return result;
	}

	// Create the color constant buffer
	// ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(ColorBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// Resource is accessible by both the GPU (read only) and the CPU (write only); good choice for a resource that will be updated by the CPU at least once per frame
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// Bind the buffer as a constant buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pColorBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create color buffer.", result);
		return result;
	}

	return result;
}

#pragma endregion

#pragma region Render

bool SkyDomeShader::Render(SkyDome* pSkyDome, Camera* pCamera)
{
	HRESULT result = S_OK;

	// Set the vertex input layout
	m_pImmediateContext->IASetInputLayout(m_pVertexInputLayout);

	// Update and set the matrix constant buffer to be used by the vertex shader
	Shader::SetMatrixBuffer(pSkyDome->GetWorldMatrix(), pCamera->GetViewMatrix(), pCamera->GetProjectionMatrix());

	// Set the vertex shader to the device
	m_pImmediateContext->VSSetShader(m_pVertexShader,
									 nullptr,			// Array of class instance interfaces used by the vertex shader
									 0);				// Number of class instance interfaces

	// Update the color constant buffer

	// Lock the color buffer so it can be written to
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = m_pImmediateContext->Map(m_pColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the color buffer.", result);
		return false;
	}

	// Get a pointer to the color buffer data
	ColorBuffer* colorBufferData = (ColorBuffer*)mappedResource.pData;

	// Copy the colors into the color buffer
	colorBufferData->topColor = pSkyDome->GetTopColor();
	colorBufferData->centerColor = pSkyDome->GetCenterColor();
	colorBufferData->bottomColor = pSkyDome->GetBottomColor();

	// Unlock the color buffer
	m_pImmediateContext->Unmap(m_pColorBuffer, 0);

	// Set the constant buffers to be used by the pixel shader
	ID3D11Buffer* psConstantBuffers[1] = { m_pColorBuffer };
	m_pImmediateContext->PSSetConstantBuffers(0, 1, psConstantBuffers);

	// Set the pixel shader to the device
	m_pImmediateContext->PSSetShader(m_pPixelShader,
									 nullptr,			// Array of class instance interfaces used by the pixel shader 
									 0);				// Number of class instance interfaces

	// Render triangles
	m_pImmediateContext->DrawIndexed(pSkyDome->GetIndexCount(),
									 0,							// Location of the first index read by the GPU from the index buffer
									 0);						// Value added to each index before reading a vertex from the vertex buffer

	return true;
}

#pragma endregion