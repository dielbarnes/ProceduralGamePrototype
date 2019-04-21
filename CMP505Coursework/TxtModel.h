//
// TxtModel.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 7: 3D Model Rendering (http://www.rastertek.com/dx11tut07.html)
// RasterTek Tutorial 10: Specular Lighting (http://www.rastertek.com/dx11tut10.html)
// Texturing and Lighting in DirectX 11 (https://www.3dgep.com/texturing-lighting-directx-11)
// RasterTek Tutorial 37: Instancing (http://www.rastertek.com/dx11tut37.html)
//

#pragma once

#include <vector>
#include <d3d11.h>
#include <directxmath.h>
#include "Utils.h"

#define DEFAULT_LIGHT_DIRECTION XMFLOAT3(0.0f, -0.8f, 0.5f)

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 textureCoordinates;
	XMFLOAT3 normal;

	Vertex() {}

	Vertex(XMFLOAT3 pos, XMFLOAT2 texCoord, XMFLOAT3 norm)
	{
		position = pos;
		textureCoordinates = texCoord;
		normal = norm;
	}
};

struct VertexData
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
};

struct Instance
{
	XMMATRIX worldMatrix;
	XMINT2 textureTileCount;
	XMFLOAT3 lightDirection;
};

class TxtModel
{
public:
	TxtModel();
	~TxtModel();

	void SetTexture(ID3D11ShaderResourceView *texture);
	ID3D11ShaderResourceView* GetTexture();
	void SetTextureTileCount(int x, int y);
	XMINT2 GetTextureTileCount();
	void SetVertexCount(int iCount);
	void SetVertexData(VertexData *vertexData);
	void SetIndexCount(int iCount);
	int GetIndexCount();
	int GetInstanceCount();
	XMMATRIX GetWorldMatrix();
	void TransformWorldMatrix(XMMATRIX translationMatrix, XMMATRIX rotationMatrix, XMMATRIX scalingMatrix);
	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT4 GetSpecularColor();
	float GetSpecularPower();
	void SetLightDirection(float x, float y, float z);
	XMFLOAT3 GetLightDirection();
	XMFLOAT3 GetPointLightColor();
	float GetPointLightStrength();
	XMFLOAT3 GetPointLightPosition();

	bool InitializeBuffers(ID3D11Device *pDevice, int iInstanceCount, Instance *instances = nullptr);
	void Render(ID3D11DeviceContext *pImmediateContext);

protected:
	ID3D11ShaderResourceView *m_pTexture;
	XMINT2 m_textureTileCount;
	ID3D11Buffer *m_pVertexBuffer;
	int m_iVertexCount;
	VertexData *m_vertexData;
	ID3D11Buffer *m_pIndexBuffer;
	int m_iIndexCount;
	ID3D11Buffer *m_pInstanceBuffer;
	int m_iInstanceCount;
	XMMATRIX m_worldMatrix;
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT4 m_specularColor;
	float m_fSpecularPower;
	XMFLOAT3 m_lightDirection;
	XMFLOAT3 m_pointLightColor;
	float m_fPointLightStrength;
	XMFLOAT3 m_pointLightPosition;

	void InitializeVerticesAndIndices(std::vector<Vertex> &vertices, std::vector<unsigned long> &indices);
};
