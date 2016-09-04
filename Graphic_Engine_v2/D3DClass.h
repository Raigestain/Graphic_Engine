#pragma once

#ifndef D3D_CLASS
#define D3D_CLASS

/************************************************************************/
/* LINKING																*/
/* This libraries allows us to use the D3D objects module.				*/
/* The first contains the functionality for setting up and drawing 3D	*/
/* graphics in DirectX11.												*/
/* The second contains tools to communicate with the hardware to obtain	*/
/* specific information like refresh rate and graphics card info.		*/
/* The third one allows us to compile the shaders.						*/
/************************************************************************/
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

/************************************************************************/
/* INCLUDES                                                             */
/************************************************************************/
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, 
					float screenFar, float screenNear);
	void Shutdown();

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX &projectionMatrix);
	void GetOrthographicMatrix(XMMATRIX &orthographicMatrix);
	void GetWorldMatrix(XMMATRIX &worldMatrix);

	void GetVideoCardInfo(char* cardName, int& memory);

private:
	bool					 m_vSyncEnabled;
	int						 m_videoCardMemory;
	char					 m_videoCardDescription[128];
	IDXGISwapChain*			 m_swapChain;
	ID3D11Device*			 m_device;
	ID3D11DeviceContext*	 m_deviceContext;
	ID3D11RenderTargetView*  m_renderTargetView;
	ID3D11Texture2D*		 m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView*  m_depthStencilView;
	ID3D11RasterizerState*	 m_rasterizerState;
	XMMATRIX				 m_projectionMatrix;
	XMMATRIX				 m_worldMatrix;
	XMMATRIX				 m_orthographicMatrix;


};
#endif

