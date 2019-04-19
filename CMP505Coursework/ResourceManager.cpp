//
// ResourceManager.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#include "ResourceManager.h"

#pragma region Init

ResourceManager::ResourceManager(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext)
{
	m_pDevice = pDevice;
	m_pImmediateContext = pImmediateContext;

	unsigned char color[] = { 200, 200, 220, 255 };
	Model::Create1x1ColorTexture(m_pDevice, color, &m_pDefaultTexture);
}

ResourceManager::~ResourceManager()
{
	SAFE_RELEASE(m_pDefaultTexture)
	for (auto &texture : m_ddsTextures)
	{
		SAFE_RELEASE(texture);
	}
	for (auto &model : m_txtModels)
	{
		SAFE_DELETE(model);
	}
	SAFE_DELETE(m_pSkyDome);
	for (auto &model : m_models)
	{
		SAFE_DELETE(model);
	}
}

bool ResourceManager::LoadResources()
{
	// Loading of resources should be in the same order as the enums

	// Ground

	HRESULT result = LoadDdsTexture(DdsTextureResource::GroundTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to load ground texture.", result);
		return false;
	}

	if (!LoadTxtModel(TxtModelResource::GroundModel))
	{
		MessageBox(0, "Failed to load ground model.", "", 0);
		return false;
	}

	m_txtModels[TxtModelResource::GroundModel]->SetTexture(m_ddsTextures[DdsTextureResource::GroundTexture]);
	m_txtModels[TxtModelResource::GroundModel]->SetTextureTileCount(3, 3);

	if (!m_txtModels[TxtModelResource::GroundModel]->InitializeBuffers(m_pDevice, 1))
	{
		MessageBox(0, "Failed to initialize ground vertex and index buffers.", "", 0);
		return false;
	}

	XMMATRIX groundTranslationMatrix = XMMatrixTranslation(0.4f, 0.0f, 0.0f);
	XMMATRIX groundScalingMatrix = XMMatrixScaling(0.22f, 0.22f, 0.22f);
	m_txtModels[TxtModelResource::GroundModel]->TransformWorldMatrix(groundTranslationMatrix, XMMatrixIdentity(), groundScalingMatrix);

	// Sky dome

	if (!LoadTxtModel(TxtModelResource::SkyDomeModel))
	{
		MessageBox(0, "Failed to load sky dome model.", "", 0);
		return false;
	}

	m_pSkyDome->SetTopColor(COLOR_XMF4(17.0f, 0.0f, 50.0f, 1.0f));
	m_pSkyDome->SetCenterColor(COLOR_XMF4(10.0f, 0.0f, 30.0f, 1.0f));
	m_pSkyDome->SetBottomColor(COLOR_XMF4(7.0f, 0.0f, 20.0f, 1.0f));

	if (!m_pSkyDome->InitializeBuffers(m_pDevice))
	{
		MessageBox(0, "Failed to initialize sky dome vertex and index buffers.", "", 0);
		return false;
	}

	// Crystal post

	// Distance between 2 posts: 6.55f (3 fences between)
	// 1.0f post z-movement = 1.1f fence z-movement

	int iCrystalPostCount = 4;
	Instance *crystalPostInstances = new Instance[iCrystalPostCount];
	XMMATRIX crystalPostScalingMatrix = XMMatrixScaling(1.1f, 1.1f, 1.1f);
	// Back
	crystalPostInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-3.2f, 0.0f, 3.3f) * crystalPostScalingMatrix);
	crystalPostInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(3.35f, 0.0f, 3.3f) * crystalPostScalingMatrix);
	// Front
	crystalPostInstances[2].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-3.2f, 0.0f, -3.25f) * crystalPostScalingMatrix);
	crystalPostInstances[3].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(3.35f, 0.0f, -3.25f) * crystalPostScalingMatrix);
	if (!LoadModel(ModelResource::CrystalPostModel, iCrystalPostCount, crystalPostInstances))
	{
		MessageBox(0, "Failed to load crystal post model.", "", 0);
		return false;
	}

	// Crystal fence

	// Distance between 2 fences: 2.5f

	int iCrystalFenceCount = 12;
	Instance *crystalFenceInstances = new Instance[iCrystalFenceCount];
	// Back
	crystalFenceInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f, 0.5f, 3.63f));
	crystalFenceInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f, 0.5f, 3.63f));
	crystalFenceInstances[2].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f, 0.5f, 3.63f));
	// Front
	crystalFenceInstances[3].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f, 0.5f, -3.575f));
	crystalFenceInstances[4].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f, 0.5f, -3.575f));
	crystalFenceInstances[5].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f, 0.5f, -3.575f));
	// Left
	XMMATRIX crystalFenceRotationMatrix = XMMatrixRotationRollPitchYaw(XM_PI * 0.0f, XM_PI * 0.5f, XM_PI * 0.0f);
	crystalFenceInstances[6].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - 0.11f, 0.5f, -3.575f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[7].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f - 0.11f, 0.5f, -3.575f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[8].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - 0.11f, 0.5f, -3.575f + 0.05f) * crystalFenceRotationMatrix);
	// Right
	crystalFenceInstances[9].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - 0.11f, 0.5f, 3.63f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[10].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f - 0.11f, 0.5f, 3.63f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[11].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - 0.11f, 0.5f, 3.63f + 0.05f) * crystalFenceRotationMatrix);
	if (!LoadModel(ModelResource::CrystalFenceModel, iCrystalFenceCount, crystalFenceInstances))
	{
		MessageBox(0, "Failed to load crystal fence model.", "", 0);
		return false;
	}

	m_models[ModelResource::CrystalFenceModel]->SetPointLightPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));

	return true;
}

HRESULT ResourceManager::LoadDdsTexture(DdsTextureResource resource)
{
	HRESULT result = S_OK;

	// Create texture

	const wchar_t *filePath = L"";
	switch (resource)
	{
	case GroundTexture:
		filePath = L"Resources/cobblestone.dds";
		break;
	}

	ID3D11ShaderResourceView *pTexture;
	result = CreateDDSTextureFromFile(m_pDevice, m_pImmediateContext, filePath, nullptr, &pTexture, 0, nullptr);
	if (FAILED(result))
	{
		return result;
	}

	// Store texture in vector
	m_ddsTextures.push_back(pTexture);

	return result;
}

bool ResourceManager::LoadTxtModel(TxtModelResource resource)
{
	// Reference:
	// RasterTek Tutorial 8: Loading Maya 2011 Models (http://www.rastertek.com/dx11tut08.html)

	// Create model
	TxtModel *pModel = nullptr;
	if (resource == SkyDomeModel)
	{
		m_pSkyDome = new SkyDome();
	}
	else
	{
		pModel = new TxtModel();
	}

	// Open file

	const char *filePath = "";
	switch (resource)
	{
	case GroundModel:
		filePath = "Resources/plane.txt";
		break;
	case SkyDomeModel:
		filePath = "Resources/skydome.txt";
		break;
	}

	std::ifstream file;
	file.open(filePath);
	if (file.fail())
	{
		return false;
	}

	// Read up to the value of the vertex count
	char input;
	file.get(input);
	while (input != ':')
	{
		file.get(input);
	}

	// Read the vertex count
	int iVertexCount;
	file >> iVertexCount;
	if (resource == SkyDomeModel)
	{
		m_pSkyDome->SetVertexCount(iVertexCount);
		m_pSkyDome->SetIndexCount(iVertexCount);
	}
	else
	{
		pModel->SetVertexCount(iVertexCount);
		pModel->SetIndexCount(iVertexCount);
	}

	// Read up to the beginning of the vertex data
	file.get(input);
	while (input != ':')
	{
		file.get(input);
	}
	file.get(input);
	file.get(input);

	// Read the vertex data
	VertexData *vertexData = new VertexData[iVertexCount];
	for (int i = 0; i < iVertexCount; i++)
	{
		file >> vertexData[i].x >> vertexData[i].y >> vertexData[i].z;
		file >> vertexData[i].tu >> vertexData[i].tv;
		file >> vertexData[i].nx >> vertexData[i].ny >> vertexData[i].nz;
	}
	if (resource == SkyDomeModel)
	{
		m_pSkyDome->SetVertexData(vertexData);
	}
	else
	{
		pModel->SetVertexData(vertexData);
	}

	// Close file
	file.close();

	if (resource != SkyDomeModel)
	{
		// Store model in vector
		m_txtModels.push_back(pModel);
	}

	return true;
}

bool ResourceManager::LoadModel(ModelResource resource, int iInstanceCount, Instance *instances)
{
	std::string strFilePath = "";
	switch (resource)
	{
	case CrystalPostModel:
		strFilePath = "Resources/crystal_post.obj";
		break;
	case CrystalFenceModel:
		strFilePath = "Resources/crystal_fence.obj";
		break;
	}

	Model *pModel = new Model(m_pDevice, m_pImmediateContext, m_pDefaultTexture);
	if (!pModel->Initialize(strFilePath, iInstanceCount, instances))
	{
		return false;
	}

	m_models.push_back(pModel);

	return true;
}

#pragma endregion

#pragma region Getters

ID3D11ShaderResourceView* ResourceManager::GetTexture(DdsTextureResource resource)
{
	return m_ddsTextures[resource];
}

TxtModel* ResourceManager::GetModel(TxtModelResource resource)
{
	return m_txtModels[resource];
}

SkyDome* ResourceManager::GetSkyDome()
{
	return m_pSkyDome;
}

#pragma endregion

#pragma region Render

void ResourceManager::RenderModel(TxtModelResource resource)
{
	if (resource == SkyDomeModel)
	{
		m_pSkyDome->Render(m_pImmediateContext);
	}
	else
	{
		m_txtModels[resource]->Render(m_pImmediateContext);
	}
}

bool ResourceManager::RenderModel(ModelResource resource, Camera *pCamera, LightShader *pLightShader)
{
	if (!pLightShader->PreRender(m_models[resource], pCamera))
	{
		return false;
	}

	std::vector<Mesh*> meshes = m_models[resource]->GetMeshes();

	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->Render(m_pImmediateContext);
		pLightShader->Render(meshes[i], pCamera);
	}

	return true;
}

#pragma endregion
