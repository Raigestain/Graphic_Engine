#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_ColorShader = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass &)
{
}


GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool bResult;

	//Create the Direct3D object.
	m_Direct3D = new D3DClass();
	if (!m_Direct3D)
	{
		return false;
	}

	//Initialize the Direct3D object.
	bResult = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

	if (!bResult)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//Create the camera object.
	m_Camera = new CameraClass();
	if(!m_Camera)
	{
		return false;
	}

	//Set the initial position of the camera
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	//Create the model object.
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return false;
	}

	//Initialize the model object.
	bResult = m_Model->Initialize(m_Direct3D->GetDevice());
	if (!bResult)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	//Create the color shader object.
	m_ColorShader = new ColorShader();
	if (!m_ColorShader)
	{
		return false;
	}

	//Initialize the color shader object.
	bResult = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!bResult)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = nullptr;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	//Release Direct3D object.
	if (m_Direct3D)
	{
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	bool bResult;
	
	//Render the graphics scene
	bResult = Render();
	if(!bResult)
	{
		return false;
	}
	return true;
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool bResult;

	//Clear buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Generate the view matrix based in the camera's position.
	m_Camera->Render();

	//Get the world, view and projection matrices from the camera and the d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//Render the object using the color shader.
	bResult = m_ColorShader->Render(m_Direct3D->GetDeviceContext(),
									m_Model->GetIndexCount(),
									worldMatrix,
									viewMatrix,
									projectionMatrix);
	if (!bResult)
	{
		return false;
	}

	//Present the renderer scene to the screen.
	m_Direct3D->EndScene();

	
	return true;
}
