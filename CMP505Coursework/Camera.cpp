//
// Camera.cpp
// Copyright � 2019 Diel Barnes. All rights reserved.
//
// Reference:
// DirectX-Engine (https://github.com/Pindrought/DirectX-Engine)
//

#include "Camera.h"

#pragma region Init

Camera::Camera(XMFLOAT3 position, float fAspectRatio)
{
	// Set the initial camera position
	m_vPosition = XMVectorSet(position.x, position.y, position.z, 0);
	m_rotation = XMFLOAT3(20 / 180 * XM_PI, 0.0f, 0.0f);

	// Initialize the projection matrix
	m_projectionMatrix = XMMatrixPerspectiveFovLH( // Build a left-handed perspective projection matrix	
								XM_PI / 2.5,	// Field of view angle Y; XM_PIDIV2 (90 degrees) is top-down view
								fAspectRatio,	// Aspect ratio of view-space X:Y
								0.1f,			// Near Z (distance to the near clipping plane); must be greater than 0
								1000.0f);		// Far Z (distance to the far clipping plane); must be greater than 0
}

Camera::~Camera()
{
}

#pragma endregion

#pragma region Getters

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(m_vPosition.m128_f32[0], m_vPosition.m128_f32[1], m_vPosition.m128_f32[2]);
}

XMMATRIX Camera::GetViewMatrix()
{
	return m_viewMatrix;
}

XMMATRIX Camera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}

#pragma endregion

#pragma region Move/Rotate

void Camera::Move(Direction direction, float fOffset)
{
	XMVECTOR vOffset;

	switch (direction)
	{
	case Direction::Forward:
		m_vForward.m128_f32[1] = 0;
		vOffset = m_vForward * fOffset;
		break;
	case Direction::Backward:
		m_vBackward.m128_f32[1] = 0;
		vOffset = m_vBackward * fOffset;
		break;
	case Direction::Left:
		m_vLeft.m128_f32[1] = 0;
		vOffset = m_vLeft * fOffset;
		break;
	case Direction::Right:
		m_vRight.m128_f32[1] = 0;
		vOffset = m_vRight * fOffset;
		break;
	case Direction::Up:
	case Direction::Down:
		vOffset = { 0.0f, fOffset, 0.0f };
		break;
	}

	m_vPosition += vOffset;
	m_vPosition.m128_f32[3] = 0;
}

void Camera::Rotate(const float fXOffset, const float fYOffset)
{
	m_rotation.x += fYOffset;
	m_rotation.y += fXOffset;

	if (m_rotation.x < -XM_PIDIV2)
	{
		m_rotation.x = -XM_PIDIV2;
	}
	else if (m_rotation.x > XM_PIDIV2)
	{
		m_rotation.x = XM_PIDIV2;
	}

	if (m_rotation.y < 0)
	{
		m_rotation.y += XM_2PI;
	}
	else if (m_rotation.y > XM_2PI)
	{
		m_rotation.y -= XM_2PI;
	}
}

#pragma endregion

#pragma region Update

void Camera::Update()
{
	// Apply rotation
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
	XMVECTOR vLook = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, rotationMatrix); // Where the camera is looking
	XMVECTOR vUp = XMVector3TransformCoord(DEFAULT_UP_VECTOR, rotationMatrix);

	// Apply translation
	vLook += m_vPosition;

	// Build view matrix for a left-handed coordinate system
	m_viewMatrix = XMMatrixLookAtLH(m_vPosition, vLook, vUp);

	// Update direction vectors
	rotationMatrix = XMMatrixRotationRollPitchYaw(0, m_rotation.y, m_rotation.z);
	m_vForward = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, rotationMatrix);
	m_vBackward = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, rotationMatrix);
	m_vLeft = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, rotationMatrix);
	m_vRight = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, rotationMatrix);
}

#pragma endregion
