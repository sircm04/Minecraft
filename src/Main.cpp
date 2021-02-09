#include "pch.h"

#include "Game.h"

Game* g_Game;

int main(void)
{
	Game game;
	g_Game = &game;
	
	if (!game.CreateWindowAndContext("Minecraft", 854, 480))
		return -1;
	
	game.Initialize();
	game.StartLoop();
	
	return 0;
}