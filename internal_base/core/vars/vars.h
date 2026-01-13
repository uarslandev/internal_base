#pragma once
#include <windows.h>
#include <iostream>
#include "reclass/reclass.h"

extern DWORD64 moduleBase;
extern DWORD64 localPlayerBaseAddress;
extern DWORD64 hookedLocalPlayerBaseAddress;
extern Player* localPlayerPtr;