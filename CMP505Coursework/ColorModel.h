//
// ColorModel.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 4: Buffers, Shaders, and HLSL (http://www.rastertek.com/dx11tut04.html)
//

#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include "Utils.h"

using namespace DirectX;

struct ColorVertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;

	ColorVertex() {}

	ColorVertex(XMFLOAT3 pos, XMFLOAT4 col)
	{
		position = pos;
		color = col;
	}
};

class ColorModel
{
public:
	ColorModel();
	~ColorModel();

	int GetIndexCount();
	void SetWorldMatrix(XMMATRIX worldMatrix);
	XMMATRIX GetWorldMatrix();

	bool InitializeBuffers(ID3D11Device *pDevice);
	void Render(ID3D11DeviceContext *pImmediateContext);

private:
	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Buffer *m_pIndexBuffer;
	int m_iIndexCount;
	XMMATRIX m_worldMatrix;

	void InitializeVerticesAndIndices(std::vector<ColorVertex> &vertices, std::vector<WORD> &indices);
};
