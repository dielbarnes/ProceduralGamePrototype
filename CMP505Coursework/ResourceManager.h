//
// ResourceManager.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#pragma once

#include <vector>
#include <fstream>
#include <DirectXTK/DDSTextureLoader.h>
#include "SkyDome.h"
#include "Model.h"
#include "LightShader.h"
#include "Utils.h"

enum DdsTextureResource : int
{
	GroundTexture = 0
};

enum TxtModelResource : int
{
	GroundModel = 0,
	SkyDomeModel		// Not in models vector
};

enum ModelResource : int
{
	CrystalPostModel = 0,
	CrystalFenceModel
};

class ResourceManager
{
public:
	ResourceManager(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext);
	~ResourceManager();
	
	ID3D11ShaderResourceView* GetTexture(DdsTextureResource resource);
	TxtModel* GetModel(TxtModelResource resource);
	SkyDome* GetSkyDome();

	bool LoadResources();
	void RenderModel(TxtModelResource resource);
	bool RenderModel(ModelResource resource, Camera *pCamera, LightShader *pLightShader);

private:
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pImmediateContext;
	ID3D11ShaderResourceView *m_pDefaultTexture;
	std::vector<ID3D11ShaderResourceView*> m_ddsTextures;
	std::vector<TxtModel*> m_txtModels;
	SkyDome *m_pSkyDome;
	std::vector<Model*> m_models;

	HRESULT LoadDdsTexture(DdsTextureResource resource);
	bool LoadTxtModel(TxtModelResource resource);
	bool LoadModel(ModelResource resource);
};
