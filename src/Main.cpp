#include "pch.h"

#include "Game.h"

int main(void)
{
	Game game;
	
	if (!game.CreateWindowAndContext("Minecraft", 854, 480))
		return -1;
	
	game.Initialize();
	game.StartLoop();
	
	return 0;
}
