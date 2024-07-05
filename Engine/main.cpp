#include "systemclass.hpp"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System = new SystemClass();

	if(System->isInitialized) { System->Run(); }

	delete System;
	System = 0;

	return 0;
}