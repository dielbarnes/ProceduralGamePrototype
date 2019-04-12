//
// Mesh.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// C++ DirectX 11 Engine Tutorial 58 - Light Attenuation (https://youtu.be/RzsPqrmDzQg)
//

#pragma once

#include <vector>
#include <d3d11.h>
#include <directxmath.h>
#include "TxtModel.h"
#include "Utils.h"

using namespace DirectX;

class Mesh
{
public:
	Mesh(std::vector<ID3D11ShaderResourceView*> &textures, XMMATRIX transformMatrix);
	~Mesh();

	std::vector<ID3D11ShaderResourceView*> GetTextures();
	int GetIndexCount();
	XMMATRIX GetTransformMatrix();

	bool InitializeBuffers(ID3D11Device *pDevice, std::vector<Vertex> &vertices, std::vector<DWORD> &indices);
	void Render(ID3D11DeviceContext *pImmediateContext);

private:
	std::vector<ID3D11ShaderResourceView*> m_textures;
	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Buffer *m_pIndexBuffer;
	int m_iIndexCount;
	XMMATRIX m_transformMatrix;
};
