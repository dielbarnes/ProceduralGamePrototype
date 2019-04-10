//
// Shader.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "Utils.h"

// Link library
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

struct MatrixBuffer // For vertex shader
{
	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
};

class Shader
{
public:
	Shader(ID3D11Device *device, ID3D11DeviceContext *immediateContext);
	virtual ~Shader();

	HRESULT Initialize(LPCWSTR vertexShaderFilename, LPCSTR vertexShaderEntryPoint, LPCWSTR pixelShaderFilename, LPCSTR pixelShaderEntryPoint, D3D11_INPUT_ELEMENT_DESC vertexInputDesc[], UINT uiElementCount);

protected:
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pImmediateContext;
	ID3D11VertexShader *m_pVertexShader;
	ID3D11PixelShader *m_pPixelShader;
	ID3D11InputLayout *m_pVertexInputLayout;
	ID3D11Buffer *m_pMatrixBuffer;

	HRESULT CompileShaderFromFile(LPCWSTR filename, LPCSTR entryPoint, LPCSTR target, ID3DBlob **ppCompiledCode);
	HRESULT SetMatrixBuffer(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
};
