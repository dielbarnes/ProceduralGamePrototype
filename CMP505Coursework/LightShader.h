//
// LightShader.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// RasterTek Tutorial 7: 3D Model Rendering (http://www.rastertek.com/dx11tut07.html)
// RasterTek Tutorial 10: Specular Lighting (http://www.rastertek.com/dx11tut10.html)
//

#pragma once

#include "Shader.h"
#include "TxtModel.h"
#include "Model.h"
#include "Camera.h"

struct LightVSBuffer // For vertex shader
{
	XMFLOAT3 cameraPosition; // For specular light
	int textureTileCountX;
	int textureTileCountY;
	XMFLOAT3 padding;
};

struct LightPSBuffer // For pixel shader
{
	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT4 specularColor;
	float specularPower;
	XMFLOAT3 lightDirection;
	XMFLOAT3 pointLightColor;
	float pointLightStrength;
	XMFLOAT3 pointLightPosition;
	UINT instanceCount;
};

class LightShader : public Shader
{
public:
	LightShader(ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext);
	~LightShader();

	HRESULT Initialize();
	bool Render(TxtModel *pModel, Camera *pCamera);
	bool PreRender(Model *pModel, Camera *pCamera);
	void Render(Mesh *pMesh, Camera *pCamera);

private:
	ID3D11VertexShader *m_pInstanceVertexShader;
	ID3D11InputLayout *m_pInstanceVertexInputLayout;
	ID3D11Buffer *m_pLightVSBuffer;
	ID3D11Buffer *m_pLightPSBuffer;
	ID3D11SamplerState *m_pSamplerState;

	bool PreRender(int iInstanceCount, XMINT2 textureTileCount, XMFLOAT4 ambientColor,
				   XMFLOAT4 diffuseColor, XMFLOAT4 specularColor, float specularPower,
				   XMFLOAT3 lightDirection, XMFLOAT3 pointLightColor, float pointLightStrength, 
				   XMFLOAT3 pointLightPosition, Camera *pCamera);
	void Render(int iInstanceCount, XMMATRIX worldMatrix, std::vector<ID3D11ShaderResourceView*> textures, 
				int iIndexCount, Camera *pCamera);
};
