#pragma once

#include "manager.h"
#include "config_manager.hpp"
#include "resource_manager.hpp"

class HomeManager : public Manager<HomeManager>	{
	friend class Manager<HomeManager>;

public:
	double getCurrentHPNum() const { return num_hp; }

	void decreaseHP(double damage)
	{ 
		num_hp -= damage;
		if (num_hp < 0) num_hp = 0;

		static const ResourceManager::SoundPool& sound_pool = ResourceManager::instance()->getSoundPool();

		Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_HomeHurt)->second, 0);
	}

protected:
	HomeManager() 
	{
		num_hp = ConfigManager::instance()->num_initial_hp;
	}
	~HomeManager() = default;

private:
	double num_hp = 0;
};

