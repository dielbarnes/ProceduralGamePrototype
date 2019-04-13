//
// Model.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// C++ DirectX 11 Engine Tutorial 58 - Light Attenuation (https://youtu.be/RzsPqrmDzQg)
//

#pragma once

#include <directxmath.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <Assimp/scene.h>
#include "Mesh.h"
#include "Utils.h"

using namespace DirectX;

class Model
{
public:
	Model(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext, ID3D11ShaderResourceView *pDefaultTexture);
	~Model();

	std::vector<Mesh*> GetMeshes();
	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT4 GetSpecularColor();
	float GetSpecularPower();
	XMFLOAT3 GetLightDirection();

	bool Initialize(std::string strFilePath);
	static HRESULT Create1x1ColorTexture(ID3D11Device *pDevice, unsigned char color[4], ID3D11ShaderResourceView **pTexture);

private:
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pImmediateContext;
	ID3D11ShaderResourceView *m_pDefaultTexture;
	std::string m_strDirectory;
	std::vector<Mesh*> m_meshes;
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT4 m_specularColor;
	float m_fSpecularPower;
	XMFLOAT3 m_lightDirection;

	void ProcessNode(aiNode *pNode, const aiScene *pScene, XMMATRIX parentTransformMatrix);
	Mesh* ProcessMesh(aiMesh *pMesh, const aiScene *pScene, XMMATRIX transformMatrix);
	std::vector<ID3D11ShaderResourceView*> LoadMaterialTextures(aiMaterial *pMaterial, aiTextureType textureType, const aiScene *pScene);
	void LoadEmbeddedTexture(const uint8_t *pData, size_t size, ID3D11ShaderResourceView *pTexture);
	void LoadDiskTexture(std::string strFilePath, ID3D11ShaderResourceView **pTexture);
};
