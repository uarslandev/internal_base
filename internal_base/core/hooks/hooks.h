#pragma once
#include <Windows.h>
#include <iostream>

DWORD WINAPI InitiateHooks(HMODULE hModule);
DWORD WINAPI RestoreHooks(HMODULE hModule);
