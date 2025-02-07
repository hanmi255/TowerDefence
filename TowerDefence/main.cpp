#define SDL_MAIN_HANDLED

#include "manager/game_manager.hpp"

int main(int argc, char** argv)
{
	return GameManager::instance()->run(argc, argv);
}