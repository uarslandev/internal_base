#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include "dx11/dx.h"
#include "hooks/hooks.h"

#include "vars/vars.h"
#include "mem/mem.h"

DWORD WINAPI ConsoleThread(HMODULE hModule) {
	AllocConsole();
	FILE * fDummy;

	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	//freopen_s(&fDummy, "CONOUT$", "w", stderr);
	//freopen_s(&fDummy, "CONIN$", "r", stdin);

	std::cout << "Internal Base loaded!" << std::endl;

	InitiateHooks(hModule);

	while (true)
	{
		if (GetAsyncKeyState(VK_END))
			break;
		Sleep(10); // avoid 100% CPU
	}

	//RestoreHooks(hModule);
	fclose(fDummy);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ConsoleThread, hModule, 0, nullptr));
		//CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)GUI, hModule, 0, nullptr));
		//CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InitiateHooks, hModule, 0, nullptr));
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}