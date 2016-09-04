/*!
* \class SystemClass
*
* \brief
*
* \author Raigestain
* \date mayo 2016
*/

#pragma once

#ifndef GRAPHIC_CLASS
#define GRAPHIC_CLASS

/************************************************************************/
/* GLOBALS                                                              */
/************************************************************************/
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

/************************************************************************/
/* INCLUDES                                                             */
/************************************************************************/
#include <windows.h>
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColorShader.h"

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ColorShader* m_ColorShader;
};

#endif