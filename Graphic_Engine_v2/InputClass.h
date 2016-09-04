/*!
* \class SystemClass
*
* \brief
*
* \author Raigestain
* \date mayo 2016
*/

#pragma once

#ifndef INPUT_CLASS
#define INPUT_CLASS

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();
	void Shutdown();

	void KeyDown(unsigned int input);
	void KeyUp(unsigned int input);

	bool isKeyDown(unsigned int key);

private:
	bool m_keys[256]; //The key array where the state will be saved.
};

#endif