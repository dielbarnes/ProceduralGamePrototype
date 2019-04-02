//
// ShaderManager.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#include "ShaderManager.h"

#pragma region Init

ShaderManager::ShaderManager(ID3D11Device &device, ID3D11DeviceContext &immediateContext)
{
	m_pLightShader = new LightShader(device, immediateContext);
	m_pSkyDomeShader = new SkyDomeShader(device, immediateContext);
}

ShaderManager::~ShaderManager()
{
	SAFE_DELETE(m_pLightShader)
	SAFE_DELETE(m_pSkyDomeShader)
}

HRESULT ShaderManager::InitializeShaders()
{
	HRESULT result = S_OK;
	
	result = m_pLightShader->Initialize();
	if (FAILED(result))
	{
		return result;
	}

	result = m_pSkyDomeShader->Initialize();
	if (FAILED(result))
	{
		return result;
	}

	return result;
}

#pragma endregion

#pragma region Render

bool ShaderManager::RenderModel(Model* pModel, Camera* pCamera)
{
	return m_pLightShader->Render(pModel, pCamera);
}

bool ShaderManager::RenderSkyDome(SkyDome *pSkyDome, Camera* pCamera)
{
	return m_pSkyDomeShader->Render(pSkyDome, pCamera);
}

#pragma endregion
