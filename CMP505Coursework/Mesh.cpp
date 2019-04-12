//
// Mesh.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// C++ DirectX 11 Engine Tutorial 58 - Light Attenuation (https://youtu.be/RzsPqrmDzQg)
//

#include "Mesh.h"

#pragma region Init

Mesh::Mesh(std::vector<ID3D11ShaderResourceView*> &textures, XMMATRIX transformMatrix)
{
	m_textures = textures;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_transformMatrix = transformMatrix;
}

Mesh::~Mesh()
{
	for (auto &texture : m_textures)
	{
		SAFE_RELEASE(texture);
	}
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

bool Mesh::InitializeBuffers(ID3D11Device *pDevice, std::vector<Vertex> &vertices, std::vector<DWORD> &indices)
{
	int iVertexCount = vertices.size();
	m_iIndexCount = indices.size();

	// Create the vertex buffer

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(Vertex) * iVertexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;							// Require read and write access by the GPU
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// Bind the buffer as a vertex buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = 0;									// No CPU access is necessary

	D3D11_SUBRESOURCE_DATA subresourceData = {}; // Data that will be copied to the buffer during creation
	subresourceData.pSysMem = vertices.data();

	HRESULT result = pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create mesh vertex buffer.", result);
		return false;
	}

	// Create the index buffer

	bufferDesc.ByteWidth = sizeof(DWORD) * m_iIndexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;					// Bind the buffer as an index buffer to the input assembler stage

	subresourceData.pSysMem = indices.data();

	result = pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create mesh index buffer.", result);
		return false;
	}

	return true;
}

#pragma endregion

#pragma region Setters/Getters

std::vector<ID3D11ShaderResourceView*> Mesh::GetTextures()
{
	return m_textures;
}

int Mesh::GetIndexCount()
{
	return m_iIndexCount;
}

XMMATRIX Mesh::GetTransformMatrix()
{
	return m_transformMatrix;
}

#pragma endregion

#pragma region Render

void Mesh::Render(ID3D11DeviceContext *pImmediateContext)
{
	// Set the vertex and index buffers to active in the input assembler so they can be rendered (put them on the graphics pipeline)

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
