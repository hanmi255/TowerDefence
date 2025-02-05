#pragma once

#include "manager.hpp"
#include "tower.hpp"
#include "tower_type.hpp"
#include "archer_tower.hpp"
#include "axeman_tower.hpp"
#include "gunner_tower.hpp"
#include "config_manager.hpp"
#include "resource_manager.hpp"

#include <vector>
#include <memory>

class TowerManager : public Manager<TowerManager>
{
	friend class Manager<TowerManager>;
public:
	using TowerList = std::vector<Tower*>;

public:
	void onUpdate(double delta_time)
	{
		for (auto* tower : m_tower_list) {
			tower->onUpdate(delta_time);
		}
	}
	
	void onRender(SDL_Renderer* renderer)
	{
		for (auto* tower : m_tower_list) {
			tower->onRender(renderer);
		}
	}

	double getPlaceTowerCost(TowerType type) const
	{
		static auto* config = ConfigManager::instance();

		switch (type) {
		case TowerType::Archer:
			return config->archer_template.cost[config->level_archer];
			break;
		case TowerType::Axeman:
			return config->axeman_template.cost[config->level_axeman];
			break;
		case TowerType::Gunner:
			return config->gunner_template.cost[config->level_gunner];
			break;
		}

		return 0;
	}

	double getUpgradeTowerCost(TowerType type) const
	{
		static auto* config = ConfigManager::instance();

		switch (type) {
		case TowerType::Archer:
			return config->level_archer == 9 ? -1 : 
				config->archer_template.upgrade_cost[config->level_archer];
			break;
		case TowerType::Axeman:
			return config->level_axeman == 9 ? -1 :
				config->axeman_template.upgrade_cost[config->level_axeman];
			break;
		case TowerType::Gunner:
			return config->level_gunner == 9 ? -1 :
				config->gunner_template.upgrade_cost[config->level_gunner];
			break;
		}

		return 0;
	}

	double getViewRange(TowerType type) const
	{
		static auto* config = ConfigManager::instance();

		switch (type) {
		case TowerType::Archer:
			return config->archer_template.view_range[config->level_archer];
			break;
		case TowerType::Axeman:
			return config->axeman_template.view_range[config->level_axeman];
			break;
		case TowerType::Gunner:
			return config->gunner_template.view_range[config->level_gunner];
			break;
		}

		return 0;
	}

	void placeTower(TowerType type, const SDL_Point& index)
	{
		std::unique_ptr<Tower> tower = nullptr;

		switch (type) {
		case TowerType::Archer:
			tower = std::make_unique<ArcherTower>();
			break;
		case TowerType::Axeman:
			tower = std::make_unique<AxemanTower>();
			break;
		case TowerType::Gunner:
			tower = std::make_unique<GunnerTower>();
			break;
		default:
			break;
		}

		static Vector2 position;
		static const auto& rect = ConfigManager::instance()->rect_tile_map;

		position.x = rect.x + index.x * TILE_SIZE + TILE_SIZE / 2;
		position.y = rect.y + index.y * TILE_SIZE + TILE_SIZE / 2;
		tower->setPosition(position);

		m_tower_list.push_back(tower.release());
		ConfigManager::instance()->map.placeTower(index);

		static const auto& sound_pool = ResourceManager::instance()->getSoundPool();

		Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_PlaceTower)->second, 0);
	}

	void upgradeTower(TowerType type)
	{
		static auto* config = ConfigManager::instance();

		switch (type) {
		case TowerType::Archer:
			config->level_archer = config->level_archer < 9 ? config->level_archer + 1 : 9;
			break;
		case TowerType::Axeman:
			config->level_axeman = config->level_axeman < 9 ? config->level_axeman + 1 : 9;
			break;
		case TowerType::Gunner:
			config->level_gunner = config->level_gunner < 9 ? config->level_gunner + 1 : 9;
			break;
		}

		static const auto& sound_pool = ResourceManager::instance()->getSoundPool();

		Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_TowerLevelUp)->second, 0);
	}

protected:
	TowerManager() = default;
	~TowerManager() = default;

private:
	TowerList m_tower_list;
};

