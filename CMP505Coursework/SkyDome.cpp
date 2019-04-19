//
// SkyDome.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Terrain Tutorial 7: Sky Domes (http://www.rastertek.com/dx11ter07.html)
//

#include "SkyDome.h"

#pragma region Init

SkyDome::SkyDome()
{
	m_pVertexBuffer = nullptr;
	m_iVertexCount = 0;
	m_pIndexBuffer = nullptr;
	m_iIndexCount = 0;
	m_worldMatrix = XMMatrixIdentity();
}

SkyDome::~SkyDome()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

bool SkyDome::InitializeBuffers(ID3D11Device *pDevice)
{
	SkyDomeVertex *vertices = new SkyDomeVertex[m_iVertexCount];
	unsigned long *indices = new unsigned long[m_iIndexCount];

	// Load the model data into the vertex and index arrays
	for (int i = 0; i < m_iVertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_vertexData[i].x, m_vertexData[i].y, m_vertexData[i].z);
		vertices[i].textureCoordinates = XMFLOAT2(m_vertexData[i].tu, m_vertexData[i].tv);

		indices[i] = i;
	}

	// Create the vertex buffer

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(SkyDomeVertex) * m_iVertexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;							// Require read and write access by the GPU
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// Bind the buffer as a vertex buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = 0;									// No CPU access is necessary

	D3D11_SUBRESOURCE_DATA subresourceData = {}; // Data that will be copied to the buffer during creation
	subresourceData.pSysMem = vertices;

	HRESULT result = pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create sky dome vertex buffer.", result);
		return false;
	}

	SAFE_DELETE_ARRAY(vertices);

	// Create the index buffer

	bufferDesc.ByteWidth = sizeof(unsigned long) * m_iIndexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;					// Bind the buffer as an index buffer to the input assembler stage

	subresourceData.pSysMem = indices;

	result = pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create sky dome index buffer.", result);
		return false;
	}

	SAFE_DELETE_ARRAY(indices);

	return true;
}

#pragma endregion

#pragma region Setters/Getters

void SkyDome::SetVertexCount(int iCount)
{
	m_iVertexCount = iCount;
}

void SkyDome::SetVertexData(VertexData *vertexData)
{
	m_vertexData = vertexData;
}

void SkyDome::SetIndexCount(int iCount)
{
	m_iIndexCount = iCount;
}

int SkyDome::GetIndexCount()
{
	return m_iIndexCount;
}

void SkyDome::SetWorldMatrix(XMMATRIX worldMatrix)
{
	m_worldMatrix = worldMatrix;
}

XMMATRIX SkyDome::GetWorldMatrix()
{
	return m_worldMatrix;
}

void SkyDome::SetTopColor(XMFLOAT4 topColor)
{
	m_topColor = topColor;
}

XMFLOAT4 SkyDome::GetTopColor()
{
	return m_topColor;
}

void SkyDome::SetCenterColor(XMFLOAT4 centerColor)
{
	m_centerColor = centerColor;
}

XMFLOAT4 SkyDome::GetCenterColor()
{
	return m_centerColor;
}

void SkyDome::SetBottomColor(XMFLOAT4 bottomColor)
{
	m_bottomColor = bottomColor;
}

XMFLOAT4 SkyDome::GetBottomColor()
{
	return m_bottomColor;
}

#pragma endregion

#pragma region Render

void SkyDome::Render(ID3D11DeviceContext *pImmediateContext)
{
	// Set the vertex and index buffers to active in the input assembler so they can be rendered (put them on the graphics pipeline)

	UINT uiStrides = sizeof(SkyDomeVertex);
	UINT uiOffsets = 0;

	pImmediateContext->IASetVertexBuffers(0,					// First input slot for binding
										  1,					// Number of vertex buffers in the array
										  &m_pVertexBuffer,
										  &uiStrides,			// Size in bytes of the elements to be used from a vertex buffer (one stride for each vertex buffer in the array)
										  &uiOffsets);			// Number of bytes between the first element of a vertex buffer and the first element that will be used (one offset for each vertex buffer in the array)

	pImmediateContext->IASetIndexBuffer(m_pIndexBuffer,
									    DXGI_FORMAT_R32_UINT,	// 32-bit format that supports 32 bits for the red channel
									    0);						// Offset in bytes from the start of the index buffer to the first index to use

	// Set the primitive topology (how the GPU obtains the three vertices it requires to render a triangle)
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

#pragma endregion
