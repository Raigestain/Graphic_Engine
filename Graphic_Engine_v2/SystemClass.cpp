#include "SystemClass.h"



SystemClass::SystemClass()
{
	m_Input = nullptr;
	m_Graphics = nullptr;
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

/*
 *	Initialize()
 *	brief: This function initialized the members of the class and calls the initialize functions for the windows api.
 */
bool SystemClass::Initialize()
{
	int screenWidth = 0, screenHeight = 0; //The screen size variables initialized in 0.
	bool rightInit; 

	//Initialize windows api.
	InitializeWindows(screenWidth, screenHeight);

	//Create the input object that will control the users inputs.
	m_Input = new InputClass();
	if (!m_Input)
	{
		return false;
	}

	//Initialize the input object.
	m_Input->Initialize();

	//Create the graphics object. This object will handle the rendering of the graphics for the aplication.
	m_Graphics = new GraphicsClass();
	if (!m_Graphics)
	{
		return false;
	}

	//Initialize the graphics object.
		rightInit = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!rightInit)
	{
		return false;
	}

	return true;
}

/*
 *	Shutdown()
 *	brief: This function does the clean up of the SystemClass. It releases everything associated with the graphics and input objects.
 *		   It also releases and shuts down everything associated to the windows api.
 */
void SystemClass::Shutdown()
{
	//Cleanup of the graphics object.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	//Cleanup of the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = nullptr;
	}

	//Shutdown windows.
	ShutdownWindows();
}

/*
 *	Run()
 *	brief: This function runs the main loop of the application. Here the Frame() function is called each loop to render the changes.
 */
void SystemClass::Run()
{
	MSG msg; //The message to handle by windows.
	bool closeApp = false; //If the user press the exit button or if the application fails will be true.
	bool frameResult = false; //The result value of the Frame() function.

	//Initialize the message structure.
	ZeroMemory(&msg, sizeof(msg));

	while (!closeApp)
	{
		//Handle the windows message.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//If the close signal come from windows, time to close the application.
		if (msg.message == WM_QUIT)
		{
			closeApp = true;
		}
		else
		{
			//If not, then run the Frame function.
			frameResult = Frame();
			if (!frameResult)
			{
				closeApp = true;
			}
		}
	}
}

/*
 *	Frame()
 *	brief: This function will call the graphics object's frame function to render which will render the graphics in the window.
 *		   Also it will process the input object's messages for each of the users inputs.
 */
bool SystemClass::Frame()
{
	bool frameResult = false; //The result of the graphics object Frame() function.

	//Check if the user pressed escape and wants to quit the application.
	if (m_Input->isKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//Do the frame process for the graphics object.
	frameResult = m_Graphics->Frame();
	if(!frameResult)
	{
		return false;
	}

	return true;
}

/*
 *	MessageHandler()
 *	brief: This function processes the windows messages. It is used to catch the inputs from the user and pass them to the imput object.
 */
LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	//Check if a key has been pressed by the user.
	case WM_KEYDOWN:
	{
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}
	//Check if a key has been released by the user.
	case WM_KEYUP:
	{
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

/*
 *	InitializeWindow
 *	brief: This function will build the window where everything will be rendered.
 *	param screemWidht: an integer with the width of the window.
 *	param screenHeight: an integer with the height of the window.
 */
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc; 
	DEVMODE dmScreenSettings;
	int posX = 0, posY = 0;

	//Get an external pointer  to this object. TODO: Ask about this.
	ApplicationHandle = this;

	//Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	//Give the application a name.
	m_applicationName = L"Graphic Engine";

	//Setup the windows class with default settings.
	wc.style		 = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;	
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm		 = wc.hIcon;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize		 = sizeof(WNDCLASSEX);

	//Register the window class.
	RegisterClassEx(&wc);

	//Determine the resolution of the users screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//Setup the screen settings depending on whether is in full screen mode or not.
	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize		  = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields	  = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//Set the position of the screen to the top left corner.
		posX = posY = 0;
	}
	else
	{
		//If windowed then it's a 800x600 screen. TODO: Make this modifiable.
		screenWidth  = 800;
		screenHeight = 600;

		//Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	}

	//Create the window with the screen settings.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
							posX, posY,	screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//Bring the window to the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//Hide the mouse cursor.
	ShowCursor(false);
}

/*
 *	ShutdownWindows()
 *	brief: this function returns the screen settings to the default values and releases the window and the handles associated to it.
 */
void SystemClass::ShutdownWindows()
{
	//Show the mouse cursor.
	ShowCursor(true);

	//Fix the display settings if exiting the full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//Release the pointer to this class.
	ApplicationHandle = NULL;
}


/*
 *	WindProc()
 *	This function is the one that will receive the windows messages that will be handled by the MessageHandler function.
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	//Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	//Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	//Other messages will be passed to the message handler system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
	}

	}
}
