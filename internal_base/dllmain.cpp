#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include "./core/dx11/dx.h"

DWORD WINAPI ConsoleThread(LPVOID lpParam) {
    HMODULE hModule = (HMODULE)lpParam;
	AllocConsole();
	FILE * fDummy;

	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);

	std::cout << ("Console allocated.\n") << std::endl;

	while (true)
	{
		if (GetAsyncKeyState(VK_END) & 0x8000)
			break;

		Sleep(10); // avoid 100% CPU
	}

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
		//CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ConsoleThread, hModule, 0, nullptr));
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)GUI, hModule, 0, nullptr));
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}