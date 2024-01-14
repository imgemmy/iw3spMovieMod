#include <Windows.h>

#include "gameFunctions.h"
#include "offsets.h"

GameInit GameInitialize;
void GameInit::Initialize()
{
	this->SetBinds();
	this->SetCheats();
}

void GameInit::SetBinds()
{
	//we set dvars manually to avoid users who dont have setathereisacow 1337 active.

	//bind noclip to 'N'
	if (GetAsyncKeyState(0x4E) & 1)
	{
		*(int*)P_Dvar.noclip = 1 - *(int*)P_Dvar.noclip;
	}

	//left/right arrow rotation
	if (GetAsyncKeyState(0x25) & 1) //left
	{
		P_Angles()->Angles.z -= 1;
	}
	else if (GetAsyncKeyState(0x27) & 1) //right
	{
		P_Angles()->Angles.z += 1;
	}

}

void GameInit::SetCheats()
{
	//force god
	*(int*)P_Dvar.godmode = 3073;
}