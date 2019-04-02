//
// Camera.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#pragma once

#include <directxmath.h>

using namespace DirectX;

class Camera
{
public:
	Camera(const XMFLOAT3 position, const float fAspectRatio);
	~Camera();

	XMFLOAT3 GetPosition();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

	void MoveForward(const float& fDeltaTime, const float& fValue);
	void Strafe(const float& fDeltaTime, const float& fValue);
	void Rotate(const float fDeltaX, const float fDeltaY);

	void Update();

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
};
