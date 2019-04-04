//
// Camera.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// DirectX-Engine (https://github.com/Pindrought/DirectX-Engine)
//

#pragma once

#include <directxmath.h>

#define DEFAULT_UP_VECTOR		{ 0, 1, 0 }
#define DEFAULT_FORWARD_VECTOR  { 0, 0, 1 }
#define DEFAULT_BACKWARD_VECTOR { 0, 0, -1 }
#define DEFAULT_LEFT_VECTOR		{ -1, 0, 0 }
#define DEFAULT_RIGHT_VECTOR	{ 1, 0, 0 }

using namespace DirectX;

class Camera
{
public:
	Camera(const XMFLOAT3 position, const float fAspectRatio);
	~Camera();

	XMFLOAT3 GetPosition();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

	void MoveForward(const float fDeltaTime, const float fValue);
	void MoveBackward(const float fDeltaTime, const float fValue);
	void MoveLeft(const float fDeltaTime, const float fValue);
	void MoveRight(const float fDeltaTime, const float fValue);
	void MoveUp(const float fDeltaTime, const float fValue);
	void MoveDown(const float fDeltaTime, const float fValue);
	void Rotate(const float fXOffset, const float fYOffset);

	void Update();

private:
	XMVECTOR m_vPosition;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

	XMVECTOR m_vForward;
	XMVECTOR m_vBackward;
	XMVECTOR m_vLeft;
	XMVECTOR m_vRight;

	void Move(const XMVECTOR vOffset);
};
