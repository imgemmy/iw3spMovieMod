#include "gameLoop.h"
#include "gameFunctions.h"
#include "dollyCam.h"
#include <thread>
GameLoop Game;
void GameLoop::Run()
{
	//init before we do anything else
	GameInitialize.Initialize();

	//Dolly Camera
	DollyCam.Run();
}