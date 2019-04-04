//
// SkyDome.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Terrain Tutorial 7: Sky Domes (http://www.rastertek.com/dx11ter07.html)
//

#pragma once

#include "ObjModel.h"

struct SkyDomeVertex
{
	XMFLOAT3 position;
};

class SkyDome
{
public:
	SkyDome();
	~SkyDome();

	bool InitializeBuffers(ID3D11Device* device);
	void Render(ID3D11DeviceContext* immediateContext);

	void SetVertexCount(int iCount);
	void SetIndexCount(int iCount);
	int GetIndexCount();
	void SetVertexData(VertexData* vertexData);
	void SetWorldMatrix(XMMATRIX worldMatrix);
	XMMATRIX GetWorldMatrix();
	void SetTopColor(XMFLOAT4 topColor);
	XMFLOAT4 GetTopColor();
	void SetCenterColor(XMFLOAT4 centerColor);
	XMFLOAT4 GetCenterColor();
	void SetBottomColor(XMFLOAT4 bottomColor);
	XMFLOAT4 GetBottomColor();

private:
	ID3D11Buffer* m_pVertexBuffer;
	int m_iVertexCount;
	ID3D11Buffer* m_pIndexBuffer;
	int m_iIndexCount;
	VertexData* m_vertexData;
	XMMATRIX m_worldMatrix;
	XMFLOAT4 m_topColor;
	XMFLOAT4 m_centerColor;
	XMFLOAT4 m_bottomColor;
};
