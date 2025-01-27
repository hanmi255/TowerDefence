#define SDL_MAIN_HANDLED

#include "GameManager.hpp"

int main(int argc, char** argv)
{
	return GameManager::instance()->run(argc, argv);
}