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
#include "TxtModel.h"
#include "Utils.h"

using namespace DirectX;

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
