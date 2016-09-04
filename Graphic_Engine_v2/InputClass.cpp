#include "InputClass.h"



InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass &)
{
}


InputClass::~InputClass()
{
}

/*
 *	Initialize()
 *	brief: This function initializes the member attributes to default settings.
 */
void InputClass::Initialize()
{
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
}

void InputClass::Shutdown()
{
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int input)
{
	//If a key is being pressed down, then save that state in the key array.
	m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	//If a key is released, then clear the state.
	m_keys[input] = false;
}

bool InputClass::isKeyDown(unsigned int key)
{
	return m_keys[key];
}
