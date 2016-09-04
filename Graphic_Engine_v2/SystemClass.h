/*!
 * \class SystemClass
 *
 * \brief 
 *
 * \author Raigestain
 * \date mayo 2016
 */

#pragma once

#ifndef SYSTEM_CLASS
#define SYSTEM_CLASS

/************************************************************************/
/* PREPROCESSING DIRECTIVES                                             */
/************************************************************************/
#define WIN32_LEAN_AND_MEAN

/************************************************************************/
/* INCLUDES                                                             */
/************************************************************************/
#include <windows.h>
#include "InputClass.h"
#include "GraphicsClass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

private:

	bool Frame();
	void InitializeWindows(int& screenWidth, int& screenHeight);
	void ShutdownWindows();

private:
	LPCTSTR			m_applicationName;
	HINSTANCE		m_hinstance;
	HWND			m_hwnd;

	InputClass*		m_Input;
	GraphicsClass*	m_Graphics;

};

/************************************************************************/
/*  FUNCTION PROTOTYPES                                                 */
/************************************************************************/
static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

/************************************************************************/
/* GLOBALS                                                              */
/************************************************************************/
static SystemClass* ApplicationHandle = 0;

#endif