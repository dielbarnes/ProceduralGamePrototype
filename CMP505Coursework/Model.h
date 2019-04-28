//
// Model.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// C++ DirectX 11 Engine Tutorial 58 - Light Attenuation (https://youtu.be/RzsPqrmDzQg)
//

#pragma once

#include <directxmath.h>
#include <DirectXTK/GeometricPrimitive.h>
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
	int GetInstanceCount();
	void SetWorldMatrix(XMMATRIX worldMatrix);
	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT4 GetSpecularColor();
	float GetSpecularPower();
	void SetLightDirection(XMFLOAT3 lightDirection);
	XMFLOAT3 GetLightDirection();
	void SetPointLightColor(XMFLOAT4 color);
	XMFLOAT3 GetPointLightColor();
	void SetPointLightStrength(float fStrength);
	float GetPointLightStrength();
	void SetPointLightPosition(XMFLOAT3 position);
	XMFLOAT3 GetPointLightPosition();

	bool Initialize(std::string strFilePath, int iInstanceCount, Instance *instances = nullptr);
	static HRESULT Create1x1ColorTexture(ID3D11Device *pDevice, unsigned char color[4], ID3D11ShaderResourceView **pTexture);

	void GenerateCogwheel(); // Test function
	void AddTubeMesh(float fInnerRadius, float fOuterRadius, float fHeight, UINT uiSubdivisions, XMMATRIX transformMatrix);
	void AddCylinderMesh(float fRadius, float fHeight, UINT uiSubdivisions, XMMATRIX transformMatrix);
	void AddBoxMesh(XMFLOAT3 size, XMMATRIX transformMatrix);

private:
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pImmediateContext;
	ID3D11ShaderResourceView *m_pDefaultTexture;
	std::string m_strDirectory;
	std::vector<Mesh*> m_meshes;
	int m_iInstanceCount;
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT4 m_specularColor;
	float m_fSpecularPower;
	XMFLOAT3 m_lightDirection;
	XMFLOAT3 m_pointLightColor;
	float m_fPointLightStrength;
	XMFLOAT3 m_pointLightPosition;

	void ProcessNode(aiNode *pNode, const aiScene *pScene, XMMATRIX parentTransformMatrix, int iInstanceCount, Instance *instances = nullptr);
	Mesh* ProcessMesh(aiMesh *pAiMesh, const aiScene *pScene, XMMATRIX transformMatrix, int iInstanceCount, Instance *instances = nullptr);
	std::vector<ID3D11ShaderResourceView*> LoadMaterialTextures(aiMaterial *pMaterial, aiTextureType textureType, const aiScene *pScene);
	void LoadEmbeddedTexture(const uint8_t *pData, size_t size, ID3D11ShaderResourceView *pTexture);
	void LoadDiskTexture(std::string strFilePath, ID3D11ShaderResourceView **pTexture);
};
