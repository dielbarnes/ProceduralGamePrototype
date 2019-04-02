//
// ResourceManager.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#pragma once

#include <fstream>
#include <vector>
#include "DDSTextureLoader.h"
#include "SkyDome.h"
#include "Utils.h"

enum TextureResource : int
{
	GroundTexture = 0
};

enum ModelResource : int
{
	GroundModel = 0,
	SkyDomeModel		// Not in models vector
};

class ResourceManager
{
public:
	ResourceManager(ID3D11Device &device, ID3D11DeviceContext &immediateContext);
	~ResourceManager();

	bool LoadResources();
	ID3D11ShaderResourceView* GetTexture(TextureResource resource);
	Model* GetModel(ModelResource resource);
	SkyDome* GetSkyDome();
	void RenderModel(ModelResource resource);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	std::vector<ID3D11ShaderResourceView*> m_textures;
	std::vector<Model*> m_models;
	SkyDome *m_pSkyDome;

	HRESULT LoadTexture(TextureResource resource);
	bool LoadModel(ModelResource resource);
};
