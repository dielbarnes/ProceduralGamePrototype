//
// ShaderManager.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#pragma once

#include "LightShader.h"
#include "SkyDomeShader.h"
#include "ColorShader.h"

class ShaderManager
{
public:
	ShaderManager(ID3D11Device *device, ID3D11DeviceContext *immediateContext);
	~ShaderManager();

	LightShader* GetLightShader();

	HRESULT InitializeShaders();
	bool RenderModel(TxtModel *pModel, Camera *pCamera);
	bool RenderSkyDome(SkyDome *pSkyDome, Camera *pCamera, float fTime);
	bool RenderModel(ColorModel *pModel, Camera *pCamera);

private:
	LightShader *m_pLightShader;
	SkyDomeShader *m_pSkyDomeShader;
	ColorShader *m_pColorShader;
};
