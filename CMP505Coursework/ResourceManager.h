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

enum DdsTextureResource : int
{
	GroundTexture = 0,
	KnightTexture
};

enum ObjModelResource : int
{
	GroundModel = 0,
	SkyDomeModel		// Not in obj models vector
};

class ResourceManager
{
public:
	ResourceManager(ID3D11Device &device, ID3D11DeviceContext &immediateContext);
	~ResourceManager();

	bool LoadResources();
	ID3D11ShaderResourceView* GetDdsTexture(DdsTextureResource resource);
	ObjModel* GetObjModel(ObjModelResource resource);
	SkyDome* GetSkyDome();
	void RenderObjModel(ObjModelResource resource);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	std::vector<ID3D11ShaderResourceView*> m_ddsTextures;
	std::vector<ObjModel*> m_objModels;
	SkyDome *m_pSkyDome;

	HRESULT LoadDdsTexture(DdsTextureResource resource);
	bool LoadObjModel(ObjModelResource resource);
};
