#pragma once
#include <Windows.h>
#include "mathHelpers.h"

struct ViewAngles //0x00868728
{
	Vector3 Angles;
	//float x; //0x0000
	//float y; //0x0004
	//float z; //0x0008
}; //Size: 0x000C

struct Position //0x00e0da1c
{
	Vector3 Position;
	//float x; //0x0000
	//float y; //0x0004
	//float z; //0x0008
}; //Size: 0x000C

struct Screen
{
	uintptr_t ScreenX = 0x006e17fc;
	uintptr_t ScreenY = 0x006e1800;
	uintptr_t ViewMatrix = 0x006E1804;
};

struct Dvars
{
	DWORD noclip = 0x00e18d54; // bool
	DWORD godmode = 0x00c81548; //int, 3072 = off; 3073 = on


	//not really dvars but i guess they can go here
	DWORD frameTime = 0x00714b94;
};

ViewAngles* P_Angles();
Position* P_Position();
extern Dvars P_Dvar;
extern Screen P_Screen;