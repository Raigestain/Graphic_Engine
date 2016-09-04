/*!
 * \file main.cpp
 * \date 2016/05/21 15:49
 *
 * \author Raigestain
 * Contact: jpgomey@gmail.com
 *
 * \brief The main file. Where everything starts and ends.
 *
*/

/************************************************************************/
/* INCLUDES                                                             */
/************************************************************************/
#include "SystemClass.h"
//#include <vld.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool rightInit;
	
	
	// Create the system object.
	System = new SystemClass;
	if(!System)
	{
		return 0;
	}

	// Initialize and run the system object.
	rightInit = System->Initialize();
	if(rightInit)
	{
		System->Run();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}