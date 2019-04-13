//
// BlurShader.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// Tutorial 46: Glow (http://www.rastertek.com/dx11tut46.html)
//

#pragma once

#include "Shader.h"

struct TextureBuffer // For vertex shader
{
	XMFLOAT2 size;
	XMFLOAT2 padding;
};

class BlurShader : public Shader
{
public:
	BlurShader(ID3D11Device *device, ID3D11DeviceContext *immediateContext);
	~BlurShader();

	HRESULT Initialize();

private:
	ID3D11Buffer *m_pTextureBuffer;
	ID3D11SamplerState *m_pSamplerState;
};
