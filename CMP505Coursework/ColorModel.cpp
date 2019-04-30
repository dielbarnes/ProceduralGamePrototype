//
// ColorModel.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 4: Buffers, Shaders, and HLSL (http://www.rastertek.com/dx11tut04.html)
//

#include "ColorModel.h"

#pragma region Init

ColorModel::ColorModel()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_iIndexCount = 0;
	m_worldMatrix = XMMatrixIdentity();
}

ColorModel::~ColorModel()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

void ColorModel::InitializeVerticesAndIndices(std::vector<ColorVertex> &vertices, std::vector<WORD> &indices)
{
	// Reference:
	// DirectXMath Win32 Sample: Collision (https://code.msdn.microsoft.com/DirectXMath-Win32-Sample-f365b9e5)

	XMFLOAT4 color = COLOR_XMF4(0.0f, 255.0f, 0.0f, 1.0f);

	// Cube

	vertices =
	{
		ColorVertex(XMFLOAT3(-1.0f, -1.0f, -1.0f), color),
		ColorVertex(XMFLOAT3(1.0f, -1.0f, -1.0f), color),
		ColorVertex(XMFLOAT3(1.0f, -1.0f, 1.0f), color),
		ColorVertex(XMFLOAT3(-1.0f, -1.0f, 1.0f), color),
		ColorVertex(XMFLOAT3(-1.0f, 1.0f, -1.0f), color),
		ColorVertex(XMFLOAT3(1.0f, 1.0f, -1.0f), color),
		ColorVertex(XMFLOAT3(1.0f, 1.0f, 1.0f), color),
		ColorVertex(XMFLOAT3(-1.0f, 1.0f, 1.0f), color)
	};

	indices =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	m_iIndexCount = static_cast<int>(indices.size());
}

bool ColorModel::InitializeBuffers(ID3D11Device *pDevice)
{
	std::vector<ColorVertex> vertices;
	std::vector<WORD> indices;

	InitializeVerticesAndIndices(vertices, indices);

	// Create the vertex buffer

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(ColorVertex) * vertices.size();
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;							// Require read and write access by the GPU
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// Bind the buffer as a vertex buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = 0;									// No CPU access is necessary

	D3D11_SUBRESOURCE_DATA subresourceData = {}; // Data that will be copied to the buffer during creation
	subresourceData.pSysMem = vertices.data();

	HRESULT result = pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create color model vertex buffer.", result);
		return false;
	}

	// Create the index buffer

	bufferDesc.ByteWidth = sizeof(WORD) * indices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;					// Bind the buffer as an index buffer to the input assembler stage

	subresourceData.pSysMem = indices.data();

	result = pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create color model index buffer.", result);
		return false;
	}

	return true;
}

#pragma endregion

#pragma region Setters/Getters

int ColorModel::GetIndexCount()
{
	return m_iIndexCount;
}

XMMATRIX ColorModel::GetWorldMatrix()
{
	return m_worldMatrix;
}

void ColorModel::SetWorldMatrix(XMMATRIX worldMatrix)
{
	m_worldMatrix = worldMatrix;
}

#pragma endregion

#pragma region Render

void ColorModel::Render(ID3D11DeviceContext *pImmediateContext)
{
	// Set the vertex and index buffers to active in the input assembler so they can be rendered (put them on the graphics pipeline)

	UINT uiStrides = sizeof(ColorVertex);
	UINT uiOffsets = 0;

	pImmediateContext->IASetVertexBuffers(0,					// First input slot for binding
										  1,					// Number of vertex buffers in the array
										  &m_pVertexBuffer,
										  &uiStrides,			// Size in bytes of the elements to be used from a vertex buffer (one stride for each vertex buffer in the array)
										  &uiOffsets);			// Number of bytes between the first element of a vertex buffer and the first element that will be used (one offset for each vertex buffer in the array)

	pImmediateContext->IASetIndexBuffer(m_pIndexBuffer,
										DXGI_FORMAT_R16_UINT,	// 16-bit format that supports 16 bits for the red channel
										0);						// Offset in bytes from the start of the index buffer to the first index to use

	// Set the primitive topology
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

#pragma endregion
