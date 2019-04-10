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
#include "Camera.h"

struct CameraBuffer // For vertex shader
{
	XMFLOAT3 cameraPosition;
	int textureTileCountX;
	int textureTileCountY;
	XMFLOAT3 padding;
};

struct LightBuffer // For pixel shader
{
	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT4 specularColor;
	float specularPower;
	XMFLOAT3 lightDirection;
};

class LightShader : public Shader
{
public:
	LightShader(ID3D11Device *device, ID3D11DeviceContext *immediateContext);
	~LightShader();

	HRESULT Initialize();
	bool Render(TxtModel *pModel, Camera *pCamera);

private:
	ID3D11VertexShader *m_pInstanceVertexShader;
	ID3D11InputLayout *m_pInstanceVertexInputLayout;
	ID3D11Buffer *m_pCameraBuffer;
	ID3D11Buffer *m_pLightBuffer;
	ID3D11SamplerState *m_pSamplerState;
};
