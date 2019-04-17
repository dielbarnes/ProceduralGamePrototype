//
// Bloom.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

#pragma once

#include "Blur.h"

struct BloomExtractBuffer // For pixel shader
{
	float threshold;
	XMFLOAT3 padding;
};

struct BloomCombineBuffer // For pixel shader
{
	float bloomIntensity;
	float bloomSaturation;
	float sceneIntensity;
	float sceneSaturation;
};

class Bloom
{
public:
	Bloom(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext, ID3D11ShaderResourceView *pSceneTexture);
	~Bloom();

	HRESULT Initialize(int iWindowWidth, int iWindowHeight);
	bool RenderBloomExtractToTexture(ID3D11ShaderResourceView *pTexture);
	bool RenderHorizontalBlurToTexture(ID3D11ShaderResourceView *pTexture);
	bool RenderVerticalBlurToTexture();
	bool RenderBloomCombine(PostProcessQuad *pQuad, ID3D11ShaderResourceView *pTexture1, ID3D11ShaderResourceView *pTexture2);

	OffScreenRenderer *m_pExtractRenderer;
	Blur *m_pBlur;

private:
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pImmediateContext;
	ID3D11VertexShader *m_pVertexShader;
	ID3D11PixelShader *m_pExtractPixelShader;
	ID3D11PixelShader *m_pCombinePixelShader;
	ID3D11InputLayout *m_pVertexInputLayout;
	ID3D11Buffer *m_pExtractBuffer;
	ID3D11Buffer *m_pCombineBuffer;
	ID3D11SamplerState *m_pSamplerState;
	ID3D11ShaderResourceView *m_pSceneTexture;
	PostProcessQuad *m_pPostProcessQuad;
	
};
