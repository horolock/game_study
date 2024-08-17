#include "Game.h"

int main(int argc, char** argv)
{
	Game game;
	bool res = game.Initialize();

	if (res)
	{
		game.RunLoop();
	}

	game.Shutdown();

	return 0;
}