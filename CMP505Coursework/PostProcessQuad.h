//
// PostProcessQuad.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include "Utils.h"

using namespace DirectX;

struct PostProcessVertex
{
	XMFLOAT4 position;
	XMFLOAT2 textureCoordinates;

	PostProcessVertex(XMFLOAT4 pos, XMFLOAT2 texCoord)
	{
		position = pos;
		textureCoordinates = texCoord;
	}
};

class PostProcessQuad
{
public:
	PostProcessQuad();
	~PostProcessQuad();

	int GetIndexCount();

	HRESULT InitializeBuffers(ID3D11Device *pDevice);
	void Render(ID3D11DeviceContext *pImmediateContext);

private:
	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Buffer *m_pIndexBuffer;
	int m_iIndexCount;
};
