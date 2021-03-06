//
// Mesh.cpp
// Copyright � 2019 Diel Barnes. All rights reserved.
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
	m_iIndexCount = 0;
	m_pInstanceBuffer = nullptr;
	m_pInstances = nullptr;
	m_iInstanceCount = 0;
	m_worldMatrix = XMMatrixIdentity();
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
	SAFE_RELEASE(m_pInstanceBuffer);
	SAFE_DELETE(m_pInstances);
}

bool Mesh::InitializeBuffers(ID3D11Device *pDevice, std::vector<Vertex> &vertices, std::vector<DWORD> &indices, 
							 int iInstanceCount, Instance *instances)
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

	m_iInstanceCount = iInstanceCount;

	if (iInstanceCount > 1)
	{
		m_pInstances = instances;

		// Create the instance buffer

		bufferDesc.ByteWidth = sizeof(Instance) * iInstanceCount;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		for (int i = 0; i < iInstanceCount; i++)
		{
			instances[i].worldMatrix = m_transformMatrix * instances[i].worldMatrix;
		}
		subresourceData.pSysMem = instances;

		result = pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_pInstanceBuffer);
		if (FAILED(result))
		{
			Utils::ShowError("Failed to create mesh instance buffer.", result);
			return false;
		}
	}

	return true;
}

#pragma endregion

#pragma region Setters/Getters

void Mesh::SetTextures(std::vector<ID3D11ShaderResourceView*> textures)
{
	m_textures = textures;
}

std::vector<ID3D11ShaderResourceView*> Mesh::GetTextures()
{
	return m_textures;
}

int Mesh::GetIndexCount()
{
	return m_iIndexCount;
}

int Mesh::GetInstanceCount()
{
	return m_iInstanceCount;
}

void Mesh::SetWorldMatrix(XMMATRIX worldMatrix)
{
	m_worldMatrix = worldMatrix;
	if (m_pInstances != nullptr)
	{
		for (int i = 0; i < m_iInstanceCount; i++)
		{
			m_pInstances[i].worldMatrix = m_transformMatrix * worldMatrix;
		}
	}
}

XMMATRIX Mesh::GetWorldMatrix()
{
	return m_transformMatrix * m_worldMatrix;
}

#pragma endregion

#pragma region Render

void Mesh::Render(ID3D11DeviceContext *pImmediateContext)
{
	if (m_pInstances != nullptr)
	{
		// Update the instance buffer

		// Lock the instance buffer so it can be written to
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT result = pImmediateContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			Utils::ShowError("Failed to map the mesh instance buffer.", result);
		}

		// Copy the instances into the instance buffer
		memcpy(mappedResource.pData, m_pInstances, sizeof(Instance) * m_iInstanceCount);

		// Unlock the instance buffer
		pImmediateContext->Unmap(m_pInstanceBuffer, 0);
	}

	// Set the vertex and index buffers to active in the input assembler so they can be rendered (put them on the graphics pipeline)

	if (m_iInstanceCount == 1)
	{
		UINT uiStrides = sizeof(Vertex);
		UINT uiOffsets = 0;

		pImmediateContext->IASetVertexBuffers(0,				// First input slot for binding
											  1,				// Number of vertex buffers in the array
											  &m_pVertexBuffer,
											  &uiStrides,		// Size in bytes of the elements to be used from that vertex buffer (one stride for each vertex buffer in the array)
											  &uiOffsets);		// Number of bytes between the first element of a vertex buffer and the first element that will be used (one offset for each vertex buffer in the array)
	}
	else
	{
		UINT strides[2];
		strides[0] = sizeof(Vertex);
		strides[1] = sizeof(Instance);

		UINT offsets[2];
		offsets[0] = 0;
		offsets[1] = 0;

		ID3D11Buffer *bufferPointers[2];
		bufferPointers[0] = m_pVertexBuffer;
		bufferPointers[1] = m_pInstanceBuffer;

		pImmediateContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
	}

	pImmediateContext->IASetIndexBuffer(m_pIndexBuffer,
										DXGI_FORMAT_R32_UINT,	// 32-bit format that supports 32 bits for the red channel
										0);						// Offset in bytes from the start of the index buffer to the first index to use

	// Set the primitive topology (how the GPU obtains the three vertices it requires to render a triangle)
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

#pragma endregion
