//
// ObjModel.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 7: 3D Model Rendering (http://www.rastertek.com/dx11tut07.html)
// RasterTek Tutorial 10: Specular Lighting (http://www.rastertek.com/dx11tut10.html)
// Texturing and Lighting in DirectX 11 (https://www.3dgep.com/texturing-lighting-directx-11)
// RasterTek Tutorial 37: Instancing (http://www.rastertek.com/dx11tut37.html)
//

#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include "Utils.h"

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 textureCoordinate;
	XMFLOAT3 normal;
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
};

class ObjModel
{
public:
	ObjModel();
	~ObjModel();

	bool InitializeBuffers(ID3D11Device* device, int iInstanceCount, Instance* instances = nullptr);
	void Render(ID3D11DeviceContext* immediateContext);

	void SetTexture(ID3D11ShaderResourceView &texture);
	ID3D11ShaderResourceView** GetTexture();
	void SetVertexCount(int iCount);
	void SetIndexCount(int iCount);
	int GetIndexCount();
	int GetInstanceCount();
	void SetVertexData(VertexData* vertexData);
	XMMATRIX GetWorldMatrix();
	void TransformWorldMatrix(XMMATRIX translationMatrix, XMMATRIX rotationMatrix, XMMATRIX scalingMatrix);
	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT4 GetSpecularColor();
	float GetSpecularPower();
	void SetLightDirection(float x, float y, float z);
	XMFLOAT3 GetLightDirection();

protected:
	ID3D11ShaderResourceView* m_pTexture;
	ID3D11Buffer* m_pVertexBuffer;
	int m_iVertexCount;
	ID3D11Buffer* m_pIndexBuffer;
	int m_iIndexCount;
	ID3D11Buffer* m_pInstanceBuffer;
	int m_iInstanceCount;
	VertexData* m_vertexData;
	XMMATRIX m_worldMatrix;
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT4 m_specularColor;
	float m_fSpecularPower;
	XMFLOAT3 m_lightDirection;
};
