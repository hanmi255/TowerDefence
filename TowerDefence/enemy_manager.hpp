#pragma once

#include "manager.h"
#include "enemy.hpp"
#include "config_manager.hpp"
#include "home_manager.hpp"

#include <vector>

class EnemyManager : public Manager<EnemyManager> {
	friend class Manager<EnemyManager>;

public:
	using EnemyList = std::vector<Enemy*>;

public:
	void onUpdate(double deltaTime)
	{
		for (auto& enemy : m_enemy_list) {
			enemy->onUpdate(deltaTime);
		}

		processHomeCollision();
		processBulletCollision();

		removeInvaliedEnemy();
	}

	void onRender(SDL_Renderer* renderer)
	{
		for (auto& enemy : m_enemy_list) {
			enemy->onRender(renderer);
		}
	}

protected:
	EnemyManager() = default;
	~EnemyManager()
	{
		for (auto& enemy : m_enemy_list) {
			delete enemy;
		}
	}

private:
	EnemyList m_enemy_list;

private:
	void processHomeCollision() 
	{
		static const SDL_Point& index_home = ConfigManager::instance()->map.getIndexHome();
		static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
		static const Vector2 position_home_tile = {
			(double)rect_tile_map.x + index_home.x * TILE_SIZE,
			(double)rect_tile_map.y + index_home.y * TILE_SIZE
		};

		for (auto& enemy : m_enemy_list) {
			if(enemy->canRemove()) continue;

			const Vector2& position_enemy = enemy->getPosition();
			if (position_enemy.x >= position_home_tile.x
				&& position_enemy.y >= position_home_tile.y
				&& position_enemy.x <= position_home_tile.x + TILE_SIZE
				&& position_enemy.y <= position_home_tile.y + TILE_SIZE) {
				enemy->makeInvalid();

				HomeManager::instance()->decreaseHP(enemy->getDamage());
			}
		}
	}

	void processBulletCollision()
	{

	}

	void removeInvaliedEnemy()
	{
		m_enemy_list.erase(std::remove_if(m_enemy_list.begin(), m_enemy_list.end(), 
			[](const Enemy* enemy) {
				if(enemy->canRemove()) delete enemy;
				return enemy->canRemove();
			}), m_enemy_list.end());
	}
};

