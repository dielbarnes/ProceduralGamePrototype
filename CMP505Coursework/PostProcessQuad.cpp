//
// PostProcessQuad.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

#include "PostProcessQuad.h"
#include "TxtModel.h"

#pragma region Init

PostProcessQuad::PostProcessQuad()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_iIndexCount = 0;
}

PostProcessQuad::~PostProcessQuad()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

HRESULT PostProcessQuad::InitializeBuffers(ID3D11Device *pDevice)
{
	HRESULT result = S_OK;

	// Create the vertex buffer

	/*PostProcessVertex vertices[] =
	{
		PostProcessVertex(XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)), // Bottom left
		PostProcessVertex(XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)), // Top left
		PostProcessVertex(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)), // Top right
		PostProcessVertex(XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)) // Bottom right
	};*/

	Vertex vertices[] =
	{
		Vertex(XMFLOAT3(-1.0f, -1.0f, 0.11f), XMFLOAT2(0.0f, 1.0f)), // Bottom left
		Vertex(XMFLOAT3(-1.0f, 1.0f, 0.11f), XMFLOAT2(0.0f, 0.0f)), // Top left
		Vertex(XMFLOAT3(1.0f, 1.0f, 0.11f), XMFLOAT2(1.0f, 0.0f)), // Top right
		Vertex(XMFLOAT3(1.0f, -1.0f, 0.11f), XMFLOAT2(1.0f, 1.0f)) // Bottom right
	};

	D3D11_BUFFER_DESC bufferDesc = {};
	//bufferDesc.ByteWidth = sizeof(PostProcessVertex) * ARRAYSIZE(vertices);
	bufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;							// Require read and write access by the GPU
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// Bind the buffer as a vertex buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = 0;									// No CPU access is necessary

	D3D11_SUBRESOURCE_DATA subresourceData = {}; // Data that will be copied to the buffer during creation
	subresourceData.pSysMem = vertices;

	result = pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create post-process quad vertex buffer.", result);
		return result;
	}

	// Create the index buffer

	UINT indices[] =
	{
		0, 1, 2,
		0, 2, 3
		//1,2,0,0,2,3
	};

	m_iIndexCount = ARRAYSIZE(indices);

	bufferDesc.ByteWidth = sizeof(UINT) * m_iIndexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;					// Bind the buffer as an index buffer to the input assembler stage

	subresourceData.pSysMem = indices;

	result = pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create post-process quad index buffer.", result);
		return result;
	}

	return result;
}

#pragma endregion

#pragma region Getters

int PostProcessQuad::GetIndexCount()
{
	return m_iIndexCount;
}

#pragma endregion

#pragma region Render

void PostProcessQuad::Render(ID3D11DeviceContext *pImmediateContext)
{
	// Set the vertex and index buffers to active in the input assembler so they can be rendered (put them on the graphics pipeline)

	//UINT uiStrides = sizeof(PostProcessVertex);
	UINT uiStrides = sizeof(Vertex);
	UINT uiOffsets = 0;

	pImmediateContext->IASetVertexBuffers(0,				// First input slot for binding
										  1,				// Number of vertex buffers in the array
										  &m_pVertexBuffer,
										  &uiStrides,		// Size in bytes of the elements to be used from that vertex buffer (one stride for each vertex buffer in the array)
										  &uiOffsets);		// Number of bytes between the first element of a vertex buffer and the first element that will be used (one offset for each vertex buffer in the array)

	pImmediateContext->IASetIndexBuffer(m_pIndexBuffer,
										DXGI_FORMAT_R32_UINT,	// 32-bit format that supports 32 bits for the red channel
										0);						// Offset in bytes from the start of the index buffer to the first index to use

	// Set the primitive topology (how the GPU obtains the three vertices it requires to render a triangle)
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

#pragma endregion
