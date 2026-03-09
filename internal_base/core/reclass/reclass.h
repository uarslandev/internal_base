#pragma once
#include "math/math.h"

class Player
{
public:
	char pad_0000[84]; //0x0000
	Vector3 coordinates; //0x0054
	char pad_0060[3304]; //0x0060
}; //Size: 0x0D48