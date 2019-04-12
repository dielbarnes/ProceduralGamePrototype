//
// TxtModel.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 7: 3D Model Rendering (http://www.rastertek.com/dx11tut07.html)
// RasterTek Tutorial 10: Specular Lighting (http://www.rastertek.com/dx11tut10.html)
// Texturing and Lighting in DirectX 11 (https://www.3dgep.com/texturing-lighting-directx-11)
// RasterTek Tutorial 37: Instancing (http://www.rastertek.com/dx11tut37.html)
//

#include "TxtModel.h"

#pragma region Init

TxtModel::TxtModel()
{
	m_textureTileCount = XMINT2(1, 1);
	m_pVertexBuffer = nullptr;
	m_iVertexCount = 0;
	m_pIndexBuffer = nullptr;
	m_iIndexCount = 0;
	m_pInstanceBuffer = nullptr;
	m_iInstanceCount = 0;
	m_worldMatrix = XMMatrixIdentity();
	m_ambientColor = COLOR_XMF4(51.0f, 51.0f, 51.0f, 1.0f); // Ambient should not be too bright otherwise the scene will appear overexposed and washed-out
	m_diffuseColor = COLOR_XMF4(255.0f, 255.0f, 255.0f, 1.0f);

	// To disable specular light, set the specular color to black (don't use 0.0 specular power)
	// Models with 1.0 specular power will appear overly bright and washed-out
	// 10.0 specular power is appropriate for dull surfaces; the specular light will still appear very large and bright so reduce the intensity of the specular color
	// 128.0 specular power is appropriate for smooth plastics, glass, or metal; the specular light will appear very small and focused
	m_specularColor = COLOR_XMF4(10.0f, 10.0f, 10.0f, 1.0f);
	m_fSpecularPower = 24.0f;

	m_lightDirection = XMFLOAT3(0.0f, -0.8f, 0.5f);
}

TxtModel::~TxtModel()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pInstanceBuffer);
}

bool TxtModel::InitializeBuffers(ID3D11Device *pDevice, int iInstanceCount, Instance *instances)
{
	Vertex *vertices = new Vertex[m_iVertexCount];
	unsigned long *indices = new unsigned long[m_iIndexCount];

	// Load the model data into the vertex and index arrays
	for (int i = 0; i < m_iVertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_vertexData[i].x, m_vertexData[i].y, m_vertexData[i].z);
		vertices[i].textureCoordinates = XMFLOAT2(m_vertexData[i].tu, m_vertexData[i].tv);
		vertices[i].normal = XMFLOAT3(m_vertexData[i].nx, m_vertexData[i].ny, m_vertexData[i].nz);

		indices[i] = i;
	}

	// Create the vertex buffer

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(Vertex) * m_iVertexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;							// Require read and write access by the GPU
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// Bind the buffer as a vertex buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = 0;									// No CPU access is necessary

	D3D11_SUBRESOURCE_DATA subresourceData = {}; // Data that will be copied to the buffer during creation
	subresourceData.pSysMem = vertices;

	HRESULT result = pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create vertex buffer.", result);
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
		Utils::ShowError("Failed to create index buffer.", result);
		return false;
	}

	SAFE_DELETE_ARRAY(indices);

	m_iInstanceCount = iInstanceCount;

	if (iInstanceCount > 1)
	{
		// Create the instance buffer

		bufferDesc.ByteWidth = sizeof(Instance) * iInstanceCount;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		subresourceData.pSysMem = instances;

		result = pDevice->CreateBuffer(&bufferDesc, &subresourceData, &m_pInstanceBuffer);
		if (FAILED(result))
		{
			Utils::ShowError("Failed to create instance buffer.", result);
			return false;
		}
	}

	return true;
}

#pragma endregion

#pragma region Setters/Getters

void TxtModel::SetTexture(ID3D11ShaderResourceView *texture)
{
	m_pTexture = texture;
}

ID3D11ShaderResourceView* TxtModel::GetTexture()
{
	return m_pTexture;
}

void TxtModel::SetTextureTileCount(int x, int y)
{
	m_textureTileCount = XMINT2(x, y);
}

XMINT2 TxtModel::GetTextureTileCount()
{
	return m_textureTileCount;
}

void TxtModel::SetVertexCount(int iCount)
{
	m_iVertexCount = iCount;
}

void TxtModel::SetVertexData(VertexData *vertexData)
{
	m_vertexData = vertexData;
}

void TxtModel::SetIndexCount(int iCount)
{
	m_iIndexCount = iCount;
}

int TxtModel::GetIndexCount()
{
	return m_iIndexCount;
}

int TxtModel::GetInstanceCount()
{
	return m_iInstanceCount;
}

XMMATRIX TxtModel::GetWorldMatrix()
{
	return m_worldMatrix;
}

void TxtModel::TransformWorldMatrix(XMMATRIX translationMatrix, XMMATRIX rotationMatrix, XMMATRIX scalingMatrix)
{
	m_worldMatrix = m_worldMatrix * translationMatrix * rotationMatrix * scalingMatrix;
}

XMFLOAT4 TxtModel::GetAmbientColor()
{
	return m_ambientColor;
}

XMFLOAT4 TxtModel::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT4 TxtModel::GetSpecularColor()
{
	return m_specularColor;
}

float TxtModel::GetSpecularPower()
{
	return m_fSpecularPower;
}

void TxtModel::SetLightDirection(float x, float y, float z)
{
	m_lightDirection = XMFLOAT3(x, y, z);
}

XMFLOAT3 TxtModel::GetLightDirection()
{
	return m_lightDirection;
}

#pragma endregion

#pragma region Render

void TxtModel::Render(ID3D11DeviceContext *pImmediateContext)
{
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

		ID3D11Buffer* bufferPointers[2];
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
