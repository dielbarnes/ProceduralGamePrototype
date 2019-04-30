//
// ColorShader.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 4: Buffers, Shaders, and HLSL (http://www.rastertek.com/dx11tut04.html)
//

#include "ColorShader.h"

ColorShader::ColorShader(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext) : Shader(pDevice, pImmediateContext)
{
}

ColorShader::~ColorShader()
{
}

HRESULT ColorShader::Initialize()
{
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT uiElementCount = ARRAYSIZE(vertexInputDesc);

	HRESULT result = Shader::Initialize(L"Shaders/ColorVertexShader.hlsl", "VS", L"Shaders/ColorPixelShader.hlsl", "PS", vertexInputDesc, uiElementCount);
	return result;
}

bool ColorShader::Render(ColorModel *pModel, Camera *pCamera)
{
	// Set the vertex input layout
	m_pImmediateContext->IASetInputLayout(m_pVertexInputLayout);

	// Update and set the matrix constant buffer to be used by the vertex shader
	Shader::SetMatrixBuffer(pModel->GetWorldMatrix(), pCamera->GetViewMatrix(), pCamera->GetProjectionMatrix());

	// Set the vertex shader to the device
	m_pImmediateContext->VSSetShader(m_pVertexShader,
									 nullptr,			// Array of class instance interfaces used by the vertex shader
									 0);				// Number of class instance interfaces

	// Set the pixel shader to the device
	m_pImmediateContext->PSSetShader(m_pPixelShader,
									 nullptr,			// Array of class instance interfaces used by the pixel shader 
									 0);				// Number of class instance interfaces

	// Render lines
	m_pImmediateContext->DrawIndexed(pModel->GetIndexCount(),
									 0,							// Location of the first index read by the GPU from the index buffer
									 0);						// Value added to each index before reading a vertex from the vertex buffer

	return true;
}
