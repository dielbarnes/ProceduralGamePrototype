//
// BlurShader.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Tutorial 46: Glow (http://www.rastertek.com/dx11tut46.html)
//

#include "BlurShader.h"

#pragma region Init

BlurShader::BlurShader(ID3D11Device *device, ID3D11DeviceContext *immediateContext) : Shader(device, immediateContext)
{
	m_pTextureBuffer = nullptr;
	m_pSamplerState = nullptr;
}

BlurShader::~BlurShader()
{
	SAFE_RELEASE(m_pTextureBuffer)
	SAFE_RELEASE(m_pSamplerState)
}

HRESULT BlurShader::Initialize()
{
	HRESULT result = S_OK;
	
	return result;
}

#pragma endregion
