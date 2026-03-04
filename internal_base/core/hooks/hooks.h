#pragma once
#include <Windows.h>
#include <iostream>

class Player;

DWORD WINAPI InitiateHooks(HMODULE hModule);
DWORD WINAPI RestoreHooks(HMODULE hModule);

void UpdateEntityList();
void PrintEntityList();

Player* GetEntityByIndex(int index);
int GetTrackedEntityCount();
