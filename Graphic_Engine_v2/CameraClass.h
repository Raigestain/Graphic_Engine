#pragma once

#ifndef CAMERA_CLASS
#define CAMERA_CLASS

#include <DirectXMath.h>
using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass& Camera);
	~CameraClass();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX& viewMatrix);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;

};

#endif