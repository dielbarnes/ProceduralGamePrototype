//
// ColorShader.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 4: Buffers, Shaders, and HLSL (http://www.rastertek.com/dx11tut04.html)
//

#pragma once

#include "Shader.h"
#include "ColorModel.h"
#include "Camera.h"

class ColorShader : public Shader
{
public:
	ColorShader(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext);
	~ColorShader();

	HRESULT Initialize();
	bool Render(ColorModel *pModel, Camera *pCamera);
};
