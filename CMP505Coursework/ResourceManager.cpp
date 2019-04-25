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

	int iGroundCount = 5;
	Instance *groundInstances = new Instance[iGroundCount];
	groundInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.4f, 0.0f, 0.0f) * XMMatrixScaling(0.348f, 0.348f, 0.348f));
	groundInstances[0].textureTileCount = XMINT2(5, 5);
	groundInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-125.5f, 0.0f, 0.0f) * XMMatrixScaling(0.22f, 0.22f, 0.22f));
	groundInstances[1].textureTileCount = XMINT2(3, 3);
	groundInstances[2].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(126.7f, 0.0f, 0.0f) * XMMatrixScaling(0.22f, 0.22f, 0.22f));
	groundInstances[2].textureTileCount = XMINT2(3, 3);
	groundInstances[3].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-36.63f, 0.0f, 0.0f) * XMMatrixScaling(0.41f, 0.22f, 0.074f));
	groundInstances[3].textureTileCount = XMINT2(6, 1);
	groundInstances[4].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(37.3f, 0.0f, 0.0f) * XMMatrixScaling(0.41f, 0.22f, 0.074f));
	groundInstances[4].textureTileCount = XMINT2(6, 1);
	if (!m_txtModels[TxtModelResource::GroundModel]->InitializeBuffers(m_pDevice, iGroundCount, groundInstances))
	{
		MessageBox(0, "Failed to initialize ground vertex and index buffers.", "", 0);
		return false;
	}
	m_txtModels[TxtModelResource::GroundModel]->SetTexture(m_ddsTextures[DdsTextureResource::GroundTexture]);

	// Sky dome

	if (!LoadTxtModel(TxtModelResource::SkyDomeModel))
	{
		MessageBox(0, "Failed to load sky dome model.", "", 0);
		return false;
	}

	if (!m_pSkyDome->InitializeBuffers(m_pDevice))
	{
		MessageBox(0, "Failed to initialize sky dome vertex and index buffers.", "", 0);
		return false;
	}

	m_pSkyDome->SetTopColor(COLOR_XMF4(17.0f, 0.0f, 50.0f, 1.0f));
	m_pSkyDome->SetCenterColor(COLOR_XMF4(10.0f, 0.0f, 30.0f, 1.0f));
	m_pSkyDome->SetBottomColor(COLOR_XMF4(7.0f, 0.0f, 20.0f, 1.0f));

	// Crystal post

	// Distance between 2 posts: 6.55f (3 fences between)
	// 1.0f post movement = 1.1f fence z-movement

	int iCrystalPostCount = 12;
	Instance *crystalPostInstances = new Instance[iCrystalPostCount];
	XMMATRIX crystalPostScalingMatrix = XMMatrixScaling(1.1f, 1.1f, 1.1f);

	// Center room
	// Back
	crystalPostInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-5.473f, 0.0f, 5.573f) * crystalPostScalingMatrix);
	crystalPostInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(5.623f, 0.0f, 5.573f) * crystalPostScalingMatrix);
	// Front
	crystalPostInstances[2].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-5.473f, 0.0f, -5.523f) * crystalPostScalingMatrix);
	crystalPostInstances[3].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(5.623f, 0.0f, -5.523f) * crystalPostScalingMatrix);
	
	// Left room
	float fDisplacementX = 25.123f;
	// Back
	crystalPostInstances[4].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-3.2f - fDisplacementX, 0.0f, 3.3f) * crystalPostScalingMatrix);
	crystalPostInstances[5].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(3.35f - fDisplacementX, 0.0f, 3.3f) * crystalPostScalingMatrix);
	// Front
	crystalPostInstances[6].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-3.2f - fDisplacementX, 0.0f, -3.26f) * crystalPostScalingMatrix);
	crystalPostInstances[7].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(3.35f - fDisplacementX, 0.0f, -3.26f) * crystalPostScalingMatrix);

	// Right room
	// Back
	crystalPostInstances[8].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-3.2f + fDisplacementX, 0.0f, 3.3f) * crystalPostScalingMatrix);
	crystalPostInstances[9].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(3.35f + fDisplacementX, 0.0f, 3.3f) * crystalPostScalingMatrix);
	// Front
	crystalPostInstances[10].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-3.2f + fDisplacementX, 0.0f, -3.25f) * crystalPostScalingMatrix);
	crystalPostInstances[11].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(3.35f + fDisplacementX, 0.0f, -3.25f) * crystalPostScalingMatrix);
	
	for (int i = 0; i < iCrystalPostCount; i++)
	{
		crystalPostInstances[i].textureTileCount = XMINT2(1, 1);
		crystalPostInstances[i].lightDirection = DEFAULT_LIGHT_DIRECTION;
	}
	if (!LoadModel(ModelResource::CrystalPostModel, iCrystalPostCount, crystalPostInstances))
	{
		MessageBox(0, "Failed to load crystal post model.", "", 0);
		return false;
	}

	// Crystal fence

	float fDistanceBetweenFences = 2.5f;

	int iCrystalFenceCount = 68;
	Instance *crystalFenceInstances = new Instance[iCrystalFenceCount];
	XMMATRIX crystalFenceRotationMatrix = XMMatrixRotationRollPitchYaw(XM_PI * 0.0f, XM_PI * 0.5f, XM_PI * 0.0f);
	
	// Center room

	/*// Back
	crystalFenceInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f, 0.5f, 3.63f));
	crystalFenceInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f, 0.5f, 3.63f));
	crystalFenceInstances[2].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f, 0.5f, 3.63f));
	// Front
	crystalFenceInstances[3].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f, 0.5f, -3.575f));
	crystalFenceInstances[4].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f, 0.5f, -3.575f));
	crystalFenceInstances[5].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f, 0.5f, -3.575f));
	// Left
	crystalFenceInstances[6].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - 0.11f, 0.5f, -3.575f + 0.05f) * crystalFenceRotationMatrix);
	//crystalFenceInstances[7].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f - 0.11f, 0.5f, -3.575f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[7].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - 0.11f, 0.5f, -3.575f + 0.05f) * crystalFenceRotationMatrix);
	// Right
	crystalFenceInstances[8].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - 0.11f, 0.5f, 3.63f + 0.05f) * crystalFenceRotationMatrix);
	//crystalFenceInstances[10].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f - 0.11f, 0.5f, 3.63f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[9].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - 0.11f, 0.5f, 3.63f + 0.05f) * crystalFenceRotationMatrix);*/
	
	// Back
	crystalFenceInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-5.0f, 0.5f, 6.13f));
	crystalFenceInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f, 0.5f, 6.13f));
	crystalFenceInstances[2].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f, 0.5f, 6.13f));
	crystalFenceInstances[3].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f, 0.5f, 6.13f));
	crystalFenceInstances[4].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(5.0f, 0.5f, 6.13f));
	// Front
	crystalFenceInstances[5].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-5.0f, 0.5f, -6.075f));
	crystalFenceInstances[6].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f, 0.5f, -6.075f));
	crystalFenceInstances[7].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f, 0.5f, -6.075f));
	crystalFenceInstances[8].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f, 0.5f, -6.075f));
	crystalFenceInstances[9].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(5.0f, 0.5f, -6.075f));
	// Left
	crystalFenceInstances[10].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-5.0f - 0.11f, 0.5f, -6.075f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[11].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - 0.11f, 0.5f, -6.075f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[12].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - 0.11f, 0.5f, -6.075f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[13].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(5.0f - 0.11f, 0.5f, -6.075f + 0.05f) * crystalFenceRotationMatrix);
	// Right
	crystalFenceInstances[14].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-5.0f - 0.11f, 0.5f, 6.13f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[15].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - 0.11f, 0.5f, 6.13f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[16].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - 0.11f, 0.5f, 6.13f + 0.05f) * crystalFenceRotationMatrix);
	crystalFenceInstances[17].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(5.0f - 0.11f, 0.5f, 6.13f + 0.05f) * crystalFenceRotationMatrix);

	// Left room
	fDisplacementX = 27.63f;
	// Back
	crystalFenceInstances[18].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - fDisplacementX, 0.5f, 3.63f));
	crystalFenceInstances[19].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f - fDisplacementX, 0.5f, 3.63f));
	crystalFenceInstances[20].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - fDisplacementX, 0.5f, 3.63f));
	// Front
	crystalFenceInstances[21].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - fDisplacementX, 0.5f, -3.575f));
	crystalFenceInstances[22].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f - fDisplacementX, 0.5f, -3.575f));
	crystalFenceInstances[23].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - fDisplacementX, 0.5f, -3.575f));
	// Left
	crystalFenceInstances[24].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - 0.11f, 0.5f, -3.575f + 0.05f - fDisplacementX) * crystalFenceRotationMatrix);
	crystalFenceInstances[25].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f - 0.11f, 0.5f, -3.575f + 0.05f - fDisplacementX) * crystalFenceRotationMatrix);
	crystalFenceInstances[26].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - 0.11f, 0.5f, -3.575f + 0.05f - fDisplacementX) * crystalFenceRotationMatrix);
	// Right
	crystalFenceInstances[27].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - 0.11f, 0.5f, 3.63f + 0.05f - fDisplacementX) * crystalFenceRotationMatrix);
	crystalFenceInstances[28].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - 0.11f, 0.5f, 3.63f + 0.05f - fDisplacementX) * crystalFenceRotationMatrix);

	// Right room
	// Back
	crystalFenceInstances[29].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f + fDisplacementX, 0.5f, 3.63f));
	crystalFenceInstances[30].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f + fDisplacementX, 0.5f, 3.63f));
	crystalFenceInstances[31].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f + fDisplacementX, 0.5f, 3.63f));
	// Front
	crystalFenceInstances[32].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f + fDisplacementX, 0.5f, -3.575f));
	crystalFenceInstances[33].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f + fDisplacementX, 0.5f, -3.575f));
	crystalFenceInstances[34].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f + fDisplacementX, 0.5f, -3.575f));
	// Left
	crystalFenceInstances[35].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - 0.11f, 0.5f, -3.575f + 0.05f + fDisplacementX) * crystalFenceRotationMatrix);
	crystalFenceInstances[36].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - 0.11f, 0.5f, -3.575f + 0.05f + fDisplacementX) * crystalFenceRotationMatrix);
	// Right
	crystalFenceInstances[37].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(-2.5f - 0.11f, 0.5f, 3.63f + 0.05f + fDisplacementX) * crystalFenceRotationMatrix);
	crystalFenceInstances[38].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f - 0.11f, 0.5f, 3.63f + 0.05f + fDisplacementX) * crystalFenceRotationMatrix);
	crystalFenceInstances[39].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.5f - 0.11f, 0.5f, 3.63f + 0.05f + fDisplacementX) * crystalFenceRotationMatrix);

	// Left bridge
	float fStartX = -7.57f;
	int iStartIndex = 40;
	int iNumberOfFences = 7;
	float fFrontZ = -1.0f;
	float fBackZ = 1.05f;
	// Front
	for (int i = iStartIndex; i < iStartIndex + iNumberOfFences; i++)
	{
		crystalFenceInstances[i].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(fStartX - (fDistanceBetweenFences * (i - iStartIndex)), 0.5f, fFrontZ));
	}
	// Back
	iStartIndex += iNumberOfFences;
	for (int i = iStartIndex; i < iStartIndex + iNumberOfFences; i++)
	{
		crystalFenceInstances[i].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(fStartX - (fDistanceBetweenFences * (i - iStartIndex)), 0.5f, fBackZ));
	}

	// Right bridge
	fStartX = 22.56f;
	iStartIndex += iNumberOfFences;
	// Front
	for (int i = iStartIndex; i < iStartIndex + iNumberOfFences; i++)
	{
		crystalFenceInstances[i].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(fStartX - (fDistanceBetweenFences * (i - iStartIndex)), 0.5f, fFrontZ));
	}
	// Back
	iStartIndex += iNumberOfFences;
	for (int i = iStartIndex; i < iStartIndex + iNumberOfFences; i++)
	{
		crystalFenceInstances[i].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(fStartX - (fDistanceBetweenFences * (i - iStartIndex)), 0.5f, fBackZ));
	}

	for (int i = 0; i < iCrystalFenceCount; i++)
	{
		crystalFenceInstances[i].textureTileCount = XMINT2(1, 1);
		crystalFenceInstances[i].lightDirection = DEFAULT_LIGHT_DIRECTION;
	}
	if (!LoadModel(ModelResource::CrystalFenceModel, iCrystalFenceCount, crystalFenceInstances))
	{
		MessageBox(0, "Failed to load crystal fence model.", "", 0);
		return false;
	}

	m_models[ModelResource::CrystalFenceModel]->SetPointLightPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));

	// Clock

	int iClockCount = 2;
	Instance *clockInstances = new Instance[iClockCount];
	clockInstances[0].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f, 0.8f, -1.0f) * XMMatrixRotationRollPitchYaw(XM_PI * 0.0f, XM_PI * 1.0f, XM_PI * 0.0f) * XMMatrixScaling(9.0f, 9.0f, 9.0f));
	clockInstances[1].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f, 0.0f, -0.2f) * XMMatrixRotationRollPitchYaw(XM_PI * -0.5f, XM_PI * 1.0f, XM_PI * 0.0f) * XMMatrixScaling(18.0f, 18.0f, 18.0f));
	for (int i = 0; i < iClockCount; i++)
	{
		clockInstances[i].textureTileCount = XMINT2(1, 1);
		clockInstances[i].lightDirection = DEFAULT_LIGHT_DIRECTION;
	}
	if (!LoadModel(ModelResource::ClockModel, iClockCount, clockInstances))
	{
		MessageBox(0, "Failed to load clock model.", "", 0);
		return false;
	}

	// Test

	Model *pModel = new Model(m_pDevice, m_pImmediateContext, m_pDefaultTexture);
	pModel->GenerateCogwheel();
	m_models.push_back(pModel);
	pModel->SetWorldMatrix(XMMatrixTranslation(-28.0f, 5.0f, 0.0f));

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
	case ClockModel:
		strFilePath = "Resources/clock.obj";
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
