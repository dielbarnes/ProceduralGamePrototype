//
// ResourceManager.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#include "ResourceManager.h"

#pragma region Init

ResourceManager::ResourceManager(ID3D11Device &device, ID3D11DeviceContext &immediateContext)
{
	m_pDevice = &device;
	m_pImmediateContext = &immediateContext;
}

ResourceManager::~ResourceManager()
{
	for (auto& texture : m_ddsTextures)
	{
		SAFE_RELEASE(texture);
	}
	for (auto& model : m_objModels)
	{
		SAFE_DELETE(model);
	}
	SAFE_DELETE(m_pSkyDome);
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

	if (!LoadObjModel(ObjModelResource::GroundModel))
	{
		MessageBox(0, "Failed to load ground model.", "", 0);
		return false;
	}

	m_objModels[ObjModelResource::GroundModel]->SetTexture(*m_ddsTextures[DdsTextureResource::GroundTexture]);

	// Sky Dome

	if (!LoadObjModel(ObjModelResource::SkyDomeModel))
	{
		MessageBox(0, "Failed to load sky dome model.", "", 0);
		return false;
	}

	m_pSkyDome->SetTopColor(COLOR_XMF4(255.0f, 204.0f, 248.0f, 1.0f)); // Light pink
	m_pSkyDome->SetCenterColor(COLOR_XMF4(200.0f, 180.0f, 180.0f, 1.0f)); // Light gray
	m_pSkyDome->SetBottomColor(COLOR_XMF4(255.0f, 193.0f, 127.0f, 1.0f)); // Light orange

	// Initialize the vertex, index, and instance buffers

	if (!m_objModels[ObjModelResource::GroundModel]->InitializeBuffers(m_pDevice, 1))
	{
		MessageBox(0, "Failed to initialize ground vertex and index buffers.", "", 0);
		return false;
	}

	if (!m_pSkyDome->InitializeBuffers(m_pDevice))
	{
		MessageBox(0, "Failed to initialize sky dome vertex and index buffers.", "", 0);
		return false;
	}

	// Transform models

	XMMATRIX groundTranslationMatrix = XMMatrixTranslation(0.0f, 0.0f, -5.0f);
	XMMATRIX groundScalingMatrix = XMMatrixScaling(0.7f, 0.7f, 0.7f);
	m_objModels[ObjModelResource::GroundModel]->TransformWorldMatrix(groundTranslationMatrix, XMMatrixIdentity(), groundScalingMatrix);

	return true;
}

HRESULT ResourceManager::LoadDdsTexture(DdsTextureResource resource)
{
	HRESULT result = S_OK;

	// Create texture

	const wchar_t* filename = L"";
	switch (resource)
	{
	case GroundTexture:
		filename = L"Resources/grass.dds";
		break;
	case KnightTexture:
		filename = L"Resources/knight.dds";
		break;
	}

	ID3D11ShaderResourceView* texture;
	result = CreateDDSTextureFromFile(m_pDevice, m_pImmediateContext, filename, nullptr, &texture, 0, nullptr);
	if (FAILED(result))
	{
		return result;
	}

	// Store texture in vector
	m_ddsTextures.push_back(texture);

	return result;
}

bool ResourceManager::LoadObjModel(ObjModelResource resource)
{
	// Reference:
	// RasterTek Tutorial 8: Loading Maya 2011 Models (http://www.rastertek.com/dx11tut08.html)

	// Create model
	ObjModel* model = nullptr;
	if (resource == SkyDomeModel)
	{
		m_pSkyDome = new SkyDome();
	}
	else
	{
		model = new ObjModel();
	}

	// Open file

	const char *filename = "";
	switch (resource)
	{
	case GroundModel:
		filename = "Resources/plane.txt";
		break;
	case SkyDomeModel:
		filename = "Resources/skydome.txt";
		break;
	}

	std::ifstream file;
	file.open(filename);
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
		model->SetVertexCount(iVertexCount);
		model->SetIndexCount(iVertexCount);
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
	VertexData* vertexData = new VertexData[iVertexCount];
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
		model->SetVertexData(vertexData);
	}

	// Close file
	file.close();

	if (resource != SkyDomeModel)
	{
		// Store model in vector
		m_objModels.push_back(model);
	}

	return true;
}

#pragma endregion

#pragma region Getters

ID3D11ShaderResourceView* ResourceManager::GetDdsTexture(DdsTextureResource resource)
{
	return m_ddsTextures[resource];
}

ObjModel* ResourceManager::GetObjModel(ObjModelResource resource)
{
	return m_objModels[resource];
}

SkyDome* ResourceManager::GetSkyDome()
{
	return m_pSkyDome;
}

#pragma endregion

#pragma region Render

void ResourceManager::RenderObjModel(ObjModelResource resource)
{
	if (resource == SkyDomeModel)
	{
		m_pSkyDome->Render(m_pImmediateContext);
	}
	else
	{
		m_objModels[resource]->Render(m_pImmediateContext);
	}
}

#pragma endregion
