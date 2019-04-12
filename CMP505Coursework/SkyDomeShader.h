//
// SkyDomeShader.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Terrain Tutorial 7: Sky Domes (http://www.rastertek.com/dx11ter07.html)
//

#pragma once

#include "Shader.h"
#include "SkyDome.h"
#include "Camera.h"

struct SkyColorBuffer // For pixel shader
{
	XMFLOAT4 topColor;
	XMFLOAT4 centerColor;
	XMFLOAT4 bottomColor;
};

class SkyDomeShader : public Shader
{
public:
	SkyDomeShader(ID3D11Device *device, ID3D11DeviceContext *immediateContext);
	~SkyDomeShader();

	HRESULT Initialize();
	bool Render(SkyDome *pSkyDome, Camera *pCamera);

private:
	ID3D11Buffer *m_pColorBuffer;
};
