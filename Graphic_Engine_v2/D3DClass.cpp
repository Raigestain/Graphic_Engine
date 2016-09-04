#include "D3DClass.h"



D3DClass::D3DClass()
{
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_rasterizerState = nullptr;
	m_renderTargetView = nullptr;
	m_swapChain = nullptr;
}

D3DClass::D3DClass(const D3DClass &)
{
}


D3DClass::~D3DClass()
{
}


/*
 *	Initialize()
 *	brief: This function does all the set up needed for DirectX11.
 *	param screenWidth: The window width.
 *	param screenWidth: The window height.
 *	param vsync: Whether the vsync is activated or not.
 *	param hwnd: The window handler.
 *	param fullscreen: Whether if the fullscreen mode is activated or not.
 *	param screenDepth: The setting to know how far our 3D environment will render.
 *	param screenNear: The setting to know how near our 3D environment will render.
 */
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
						float screenFar, float screenNear)
{
	//TODO: Split this function in smaller functions. Also, return an error message if failed the initialization to know the reason.
	HRESULT						  hResult;
	IDXGIFactory*				  dxgiFactory;
	IDXGIAdapter*				  dxgiAdapter;
	IDXGIOutput*				  adapterOutput;
	unsigned int				  numModes, numarator, denominator;
	unsigned long long			  stringLength;
	DXGI_MODE_DESC*				  displayModeList;
	DXGI_ADAPTER_DESC			  adapterDesc;
	int							  error;
	DXGI_SWAP_CHAIN_DESC		  swapChainDesc;
	D3D_FEATURE_LEVEL			  featureLevel;
	ID3D11Texture2D*			  backBufferPntr;
	D3D11_TEXTURE2D_DESC		  depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC	  depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC		  rasterizerDesc;
	D3D11_VIEWPORT				  viewport;
	float						  fieldOfView, screenAspect;

	m_vSyncEnabled = vsync;

	//Create DirectX graphics interface factory.
	hResult = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if (FAILED(hResult))
	{
		return false;
	}

	//Use a factory to create an adapter for the primary graphics interface.
	hResult = dxgiFactory->EnumAdapters(0, &dxgiAdapter);
	if (FAILED(hResult))
	{
		return false;
	}

	//Enumerate the primary adapter output (monitor).
	hResult = dxgiAdapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(hResult))
	{
		return false;
	}

	//Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	hResult = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(hResult))
	{
		return false;
	}

	//Create a list to hold all the possible display modes for this monitor/video card combinations.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//Fill the display mode list structures.
	hResult = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(hResult))
	{
		return false;
	}

	//Go through all the display modes and find the one that matches the screen width and height.
	//When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numarator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//Get the adapter (video card) descriptor.
	hResult = dxgiAdapter->GetDesc(&adapterDesc);
	{
		if (FAILED(hResult))
		{
			return false;
		}
	}

	//Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024) / 1024; //Not sure if this is like this, but the parenthesis at the end didn't make sence to me.

	//Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	/*We have stored the numerator and denominator for the refresh rate, and we have video card information, time to release the structures
	  used for that.*/

	//Release display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	//Release the adapter output.
	adapterOutput->Release();
	adapterOutput = nullptr;

	//Release the factory.
	dxgiFactory->Release();
	dxgiFactory = nullptr;

	//With the refresh rate saved, it's time to initialize DirectX.

#pragma region SWAP_CHAIN_DESC_INIT

	//Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//Set a single back buffer.
	swapChainDesc.BufferCount = 1;

	//Set width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//Set the refresh rate of the back buffer, using the refresh rate obtained from before and checking the vsync setting.
	if (m_vSyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numarator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//Set the handler of the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	//Turn multi-sampling off. *
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//Set to full screen or windowed mode.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//Set scan line ordering and scaling to unspecified. *
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Discard the back buffer content after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//Don't set the advanced flags.
	swapChainDesc.Flags = 0;

#pragma endregion 

	//Set feature level to DirectX. For now will be 11. TODO: Make it variable.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//Create the swap chain and the DirectX11 device and device context.
	hResult = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION,
											&swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(hResult))
	{
		return false;
	}

	//Get the pointer to the back buffer.
	hResult = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPntr);
	if (FAILED(hResult))
	{
		return false;
	}

	//Create the render target view with the back buffer.
	hResult = m_device->CreateRenderTargetView(backBufferPntr, NULL, &m_renderTargetView);
	if (FAILED(hResult))
	{
		return false;
	}

	//Release the pointer to the back buffer as we no longer need it.
	backBufferPntr->Release();
	backBufferPntr = nullptr;

	//Time to set the depth buffer description. Also will attach a stencil buffer.

	//Initialize the description of the back buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//Create the texture for the depth buffer using the filled out description.
	hResult = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(hResult))
	{
		return false;
	}

	//Time to setup the depth stencil descriptor.
	//Initialize  the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//Setup the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//Stencil operation if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Stencil operation if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Now, with the description filled, let's create the depth stencil state.
	hResult = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(hResult))
	{
		return false;
	}

	//With the depth stencil created we can set it so it takes effect. Here the device context is used for it.
	//Set the depth stencil state.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//Initialize the depth stencil view descriptor.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//Setup the stencil view descriptor.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//Create the depth stencil view.
	hResult = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(hResult))
	{
		return false;
	}

	//Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	/*By setting up a raster state we can choose which polygons are rendered and how. With that control
	  we can draw in wire frame mode or draw both faces of the polygon. DirectX has one default raster state but
	  we have no control over it and can't change it unless you create one.*/

	//Setup the raster description that will determine how and what polygons will be rendered.
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID; //change for wire frame mode.
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	//Create the rasterizer state from the description.
	hResult = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	if (FAILED(hResult))
	{
		return false;
	}

	//Set the rasterizer state.
	m_deviceContext->RSSetState(m_rasterizerState);

	/*The viewport also needs to be setup so that Direct3D can map clip space coordinates to the render target space.
	  Set this to be the entire size of the window.*/

	//Setup the view port for rendering.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//Create the viewport.
	m_deviceContext->RSSetViewports(1, &viewport);

	//Setup the projection matrix.
	fieldOfView = 3.141592654f / 4.0f; //This here is a fixed number. It may be good to play with it to see what happens.
	screenAspect = (float)screenWidth / (float)screenHeight;

	//Create the projection matrix for 3D rendering.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenFar);

	//Setup the world matrix.
	m_worldMatrix = XMMatrixIdentity();

	/*Here would be the view matrix, but because it is the matrix which will determine our point of view
	  it seems to be better in the camera class.*/

	/*The last thing to set in the initialize function is the orthographic matrix. This matrix will be used
	  to render the 2D elements (like the GUI) on the screens.*/

	//Create an orthographic projection matrix for 2D rendering.
	m_orthographicMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenFar);

	return true;
}

//The shut down function is really simple and straight forward. Just release and set to null all the pointers.
void D3DClass::Shutdown()
{
	//If the window is in full screen mode, then the windowed mode is forced to avoid some exceptions thrown by the swap chain.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterizerState)
	{
		m_rasterizerState->Release();
		m_rasterizerState = nullptr;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = nullptr;	
	}

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}
}


/*
*	BeginScene()
*	brief: This function clears the buffers at the beginning of the frame for the drwing of the new scene.
*	param red: The red value for the render.
*	param green: The green value for the render.
*	param blue: The blue value for the render.
*	param alpha: The alpha value for the render.
*/
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];
	
	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

/*
*	BeginScene()
*	brief: This function tells the swap chain to render the 3D scene once all the drawing has completed
*		   at the end of each frame.
*	param red: The red value for the render.
*/
void D3DClass::EndScene()
{
	//Present the back buffer to the screen since the rendering is complete.
	if (m_vSyncEnabled)
	{
		//Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		m_swapChain->Present(0, 0);
	}
}

ID3D11Device * D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext * D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX &projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetOrthographicMatrix(XMMATRIX &orthographicMatrix)
{
	orthographicMatrix = m_orthographicMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX &worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

void D3DClass::GetVideoCardInfo(char *cardName, int & memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
}
