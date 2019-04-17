//
// Blur.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Real-Time 3D Rendering With DirectX And HLSL by Paul Varcholik (Chapter 18: Post-Processing)
//

#pragma once

#include <vector>
#include "Shader.h"
#include "OffScreenRenderer.h"
#include "PostProcessQuad.h"

#define BLUR_SAMPLE_COUNT 9

struct BlurSampleBuffer // For pixel shader
{
	XMFLOAT4 sampleOffsets[BLUR_SAMPLE_COUNT];
	XMFLOAT4 sampleWeights[BLUR_SAMPLE_COUNT];
};

class Blur
{
public:
	Blur(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext, ID3D11ShaderResourceView *pSceneTexture);
	~Blur();

	ID3D11ShaderResourceView* GetOutputTexture();

	HRESULT Initialize(int iWindowWidth, int iWindowHeight);
	bool RenderHorizontalBlurToTexture(ID3D11ShaderResourceView *pTexture);
	bool RenderVerticalBlurToTexture();

private:
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pImmediateContext;
	ID3D11VertexShader *m_pVertexShader;
	ID3D11PixelShader *m_pPixelShader;
	ID3D11InputLayout *m_pVertexInputLayout;
	ID3D11Buffer *m_pSampleBuffer;
	ID3D11SamplerState *m_pSamplerState;
	ID3D11ShaderResourceView *m_pSceneTexture;
	ID3D11ShaderResourceView *m_pOutputTexture;
	OffScreenRenderer *m_pHorizontalBlurRenderer;
	OffScreenRenderer *m_pVerticalBlurRenderer;
	PostProcessQuad *m_pPostProcessQuad;
	std::vector<XMFLOAT2> m_horizontalSampleOffsets;
	std::vector<XMFLOAT2> m_verticalSampleOffsets;
	std::vector<float> m_sampleWeights;
	float m_fBlurAmount;
	XMINT2 m_windowSize;

	void InitializeSampleOffsets();
	void InitializeSampleWeights();
	float GetWeight(float f);
	bool Render(std::vector<XMFLOAT2> sampleOffsets, ID3D11ShaderResourceView *pTexture);
};
