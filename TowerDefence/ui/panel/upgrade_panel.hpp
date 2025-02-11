#pragma once

#include "panel.hpp"
#include "../../manager/resource_manager.hpp"
#include "../../manager/tower_manager.hpp"

class UpgradePanel : public Panel
{
public:
	UpgradePanel()
	{
		const auto& texture_pool = ResourceManager::instance()->getTexturePool();

		tex_idle.reset(texture_pool.find(ResID::Tex_UIUpgradeIdle)->second);
		tex_hovered_top.reset(texture_pool.find(ResID::Tex_UIUpgradeHoveredTop)->second);
		tex_hovered_left.reset(texture_pool.find(ResID::Tex_UIUpgradeHoveredLeft)->second);
		tex_hovered_right.reset(texture_pool.find(ResID::Tex_UIUpgradeHoveredRight)->second);
	}

	~UpgradePanel() = default;

	void onUpdate(SDL_Renderer* renderer) override
	{
		static auto* tower_manager = TowerManager::instance();

		value_top = (int)tower_manager->getUpgradeTowerCost(TowerType::Axeman);
		value_left = (int)tower_manager->getUpgradeTowerCost(TowerType::Archer);
		value_right = (int)tower_manager->getUpgradeTowerCost(TowerType::Gunner);

		Panel::onUpdate(renderer);
	}

protected:
	void onClickTopArea() override
	{
		auto* coin_manager = CoinManager::instance();

		if (value_top > 0 && value_top <= coin_manager->getCurrentCoinNum()) {
			TowerManager::instance()->upgradeTower(TowerType::Axeman);
			coin_manager->decreaseCoin(value_top);
		}
	}

	void onClickLeftArea() override
	{
		auto* coin_manager = CoinManager::instance();

		if (value_left > 0 && value_left <= coin_manager->getCurrentCoinNum()) {
			TowerManager::instance()->upgradeTower(TowerType::Archer);
			coin_manager->decreaseCoin(value_left);
		}
	}

	void onClickRightArea() override
	{
		auto* coin_manager = CoinManager::instance();

		if (value_right > 0 && value_right <= coin_manager->getCurrentCoinNum()) {
			TowerManager::instance()->upgradeTower(TowerType::Gunner);
			coin_manager->decreaseCoin(value_right);
		}
	}
};