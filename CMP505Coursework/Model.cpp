//
// Model.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// C++ DirectX 11 Engine Tutorial 58 - Light Attenuation (https://youtu.be/RzsPqrmDzQg)
//

#include "Model.h"

#pragma region Init

Model::Model(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext, ID3D11ShaderResourceView *pDefaultTexture)
{
	m_pDevice = pDevice;
	m_pImmediateContext = pImmediateContext;
	m_pDefaultTexture = pDefaultTexture;
	m_ambientColor = COLOR_XMF4(51.0f, 51.0f, 51.0f, 1.0f); // Ambient should not be too bright otherwise the scene will appear overexposed and washed-out
	m_diffuseColor = COLOR_XMF4(180.0f, 100.0f, 255.0f, 1.0f);

	// To disable specular light, set the specular color to black (don't use 0.0 specular power)
	// Models with 1.0 specular power will appear overly bright and washed-out
	// 10.0 specular power is appropriate for dull surfaces; the specular light will still appear very large and bright so reduce the intensity of the specular color
	// 128.0 specular power is appropriate for smooth plastics, glass, or metal; the specular light will appear very small and focused
	m_specularColor = COLOR_XMF4(10.0f, 10.0f, 10.0f, 1.0f);
	m_fSpecularPower = 24.0f;

	m_lightDirection = XMFLOAT3(0.0f, -0.8f, 0.5f);
}

Model::~Model()
{
}

bool Model::Initialize(std::string strFilePath)
{
	m_strDirectory = Utils::GetDirectoryFromPath(strFilePath);

	Assimp::Importer importer;
	const aiScene *pScene = importer.ReadFile(strFilePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (pScene == nullptr)
	{
		return false;
	}

	ProcessNode(pScene->mRootNode, pScene, XMMatrixIdentity());

	return true;
}

void Model::ProcessNode(aiNode *pNode, const aiScene *pScene, XMMATRIX parentTransformMatrix)
{
	XMMATRIX nodeTransformMatrix = XMMatrixTranspose(XMMATRIX(&pNode->mTransformation.a1)) * parentTransformMatrix;

	for (UINT i = 0; i < pNode->mNumMeshes; i++)
	{
		aiMesh *pAiMesh = pScene->mMeshes[pNode->mMeshes[i]];
		Mesh *pMesh = ProcessMesh(pAiMesh, pScene, nodeTransformMatrix);
		m_meshes.push_back(pMesh);
	}

	for (UINT i = 0; i < pNode->mNumChildren; i++)
	{
		ProcessNode(pNode->mChildren[i], pScene, nodeTransformMatrix);
	}
}

Mesh* Model::ProcessMesh(aiMesh *pAiMesh, const aiScene *pScene, XMMATRIX transformMatrix)
{
	// Get vertices
	std::vector<Vertex> vertices;
	for (UINT i = 0; i < pAiMesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.position.x = pAiMesh->mVertices[i].x;
		vertex.position.y = pAiMesh->mVertices[i].y;
		vertex.position.z = pAiMesh->mVertices[i].z;

		if (pAiMesh->mTextureCoords[0])
		{
			vertex.textureCoordinates.x = (float)pAiMesh->mTextureCoords[0][i].x;
			vertex.textureCoordinates.y = (float)pAiMesh->mTextureCoords[0][i].y;
		}

		vertex.normal.x = pAiMesh->mNormals[i].x;
		vertex.normal.y = pAiMesh->mNormals[i].y;
		vertex.normal.z = pAiMesh->mNormals[i].z;

		vertices.push_back(vertex);
	}

	// Get indices
	std::vector<DWORD> indices;
	for (UINT i = 0; i < pAiMesh->mNumFaces; i++)
	{
		aiFace face = pAiMesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Get textures
	aiMaterial *pMaterial = pScene->mMaterials[pAiMesh->mMaterialIndex];
	std::vector<ID3D11ShaderResourceView*> textures = LoadMaterialTextures(pMaterial, aiTextureType::aiTextureType_DIFFUSE, pScene);
	
	// Create mesh
	Mesh *pMesh = new Mesh(textures, transformMatrix);
	if (!pMesh->InitializeBuffers(m_pDevice, vertices, indices))
	{
		MessageBox(0, "Failed to initialize mesh vertex and index buffers.", "", 0);
	}

	return pMesh;
}

std::vector<ID3D11ShaderResourceView*> Model::LoadMaterialTextures(aiMaterial *pMaterial, aiTextureType textureType, const aiScene *pScene)
{
	std::vector<ID3D11ShaderResourceView*> textures;

	unsigned int uiTextureCount = pMaterial->GetTextureCount(textureType);
	if (uiTextureCount == 0)
	{
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
		if (aiColor.IsBlack())
		{
			textures.push_back(m_pDefaultTexture);
		}
		else
		{
			unsigned char color[] = { aiColor.r * 255, aiColor.g * 255, aiColor.b * 255, 255 };
			ID3D11ShaderResourceView *pTexture;
			Create1x1ColorTexture(m_pDevice, color, &pTexture);
			textures.push_back(pTexture);
		}
	}
	else
	{
		for (UINT i = 0; i < uiTextureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			std::string strPath = path.C_Str();

			if (strPath[0] == '*') // Check if the path is an index number
			{
				if (pScene->mTextures[0]->mHeight == 0 && path.length >= 2)
				{
					// Compressed indexed embedded texture
					int index = atoi(&strPath[1]);
					ID3D11ShaderResourceView *pTexture = nullptr;
					LoadEmbeddedTexture(reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData), pScene->mTextures[index]->mWidth, pTexture);
					textures.push_back(pTexture);
				}
				else
				{
					// Non-compressed indexed embedded texture (not supported)
					textures.push_back(m_pDefaultTexture);
				}
			}
			else {
				const aiTexture *pAiTexture = pScene->GetEmbeddedTexture(strPath.c_str());
				if (pAiTexture != nullptr) // Check if the path is a texture's name
				{
					if (pAiTexture->mHeight == 0)
					{
						// Compressed non-indexed embedded texture
						ID3D11ShaderResourceView *pTexture = nullptr;
						LoadEmbeddedTexture(reinterpret_cast<uint8_t*>(pAiTexture->pcData), pAiTexture->mWidth, pTexture);
						textures.push_back(pTexture);
					}
					else
					{
						// Non-compressed non-indexed embedded texture (not supported)
						textures.push_back(m_pDefaultTexture);
					}
				}
				else if(strPath.find('.') != std::string::npos) // Check for the period before the file extension
				{
					// Disk texture
					std::string strFilePath = m_strDirectory + '/' + strPath;
					ID3D11ShaderResourceView *pTexture = nullptr;
					LoadDiskTexture(strFilePath, &pTexture);
					textures.push_back(pTexture);
				}
			}
		}
	}

	return textures;
}

void Model::LoadEmbeddedTexture(const uint8_t *pData, size_t size, ID3D11ShaderResourceView *pTexture)
{
	HRESULT result = CreateWICTextureFromMemory(m_pDevice, pData, size, nullptr, &pTexture);
	if (FAILED(result))
	{
		pTexture = m_pDefaultTexture;
	}
}

void Model::LoadDiskTexture(std::string strFilePath, ID3D11ShaderResourceView **pTexture)
{
	std::wstring wstrFilePath(strFilePath.begin(), strFilePath.end());

	HRESULT result = S_OK;
	if (Utils::GetFileExtension(strFilePath) == "dds")
	{
		result = CreateDDSTextureFromFile(m_pDevice, wstrFilePath.c_str(), nullptr, pTexture);
	}
	else {
		result = CreateWICTextureFromFile(m_pDevice, wstrFilePath.c_str(), nullptr, pTexture);
	}
	if (FAILED(result))
	{
		pTexture = &m_pDefaultTexture;
	}
}

HRESULT Model::Create1x1ColorTexture(ID3D11Device *pDevice, unsigned char color[4], ID3D11ShaderResourceView **pTexture)
{
	HRESULT result = S_OK;

	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1); // 1x1 size

	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = color;
	subresourceData.SysMemPitch = sizeof(color);

	ID3D11Texture2D *p2DTexture;
	result = pDevice->CreateTexture2D(&textureDesc, &subresourceData, &p2DTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to initialize texture from color data.", result);
		return result;
	}

	ID3D11Resource *pResource = static_cast<ID3D11Texture2D*>(p2DTexture);

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);

	result = pDevice->CreateShaderResourceView(pResource, &shaderResourceViewDesc, pTexture);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create shader resource view from texture generated from color data.", result);
		return result;
	}
}

#pragma endregion

#pragma region Setters/Getters

std::vector<Mesh*> Model::GetMeshes()
{
	return m_meshes;
}

XMFLOAT4 Model::GetAmbientColor()
{
	return m_ambientColor;
}

XMFLOAT4 Model::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT4 Model::GetSpecularColor()
{
	return m_specularColor;
}

float Model::GetSpecularPower()
{
	return m_fSpecularPower;
}

XMFLOAT3 Model::GetLightDirection()
{
	return m_lightDirection;
}

#pragma endregion
