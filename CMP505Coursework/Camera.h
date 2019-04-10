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

enum Direction
{
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down
};

using namespace DirectX;

class Camera
{
public:
	Camera(XMFLOAT3 position, float fAspectRatio);
	~Camera();

	XMFLOAT3 GetPosition();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

	void Move(Direction direction, float fOffset);
	void Rotate(float fXOffset, float fYOffset);

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
};
