#pragma once

#include "panel.hpp"
#include "../../manager/resource_manager.hpp"
#include "../../manager/tower_manager.hpp"
#include "../../manager/coin_manager.hpp"

#include <SDL2_gfxPrimitives.h>

class PlacePanel : public Panel
{
public:
    PlacePanel()
    {
        const auto& texture_pool = ResourceManager::instance()->getTexturePool();

        tex_idle.reset(texture_pool.find(ResID::Tex_UIPlaceIdle)->second);
        tex_hovered_top.reset(texture_pool.find(ResID::Tex_UIPlaceHoveredTop)->second);
        tex_hovered_left.reset(texture_pool.find(ResID::Tex_UIPlaceHoveredLeft)->second);
        tex_hovered_right.reset(texture_pool.find(ResID::Tex_UIPlaceHoveredRight)->second);
    }

    ~PlacePanel() = default;

    void onUpdate(SDL_Renderer* renderer) override
    {
        static auto* tower_manager = TowerManager::instance();

        value_top = (int)tower_manager->getPlaceTowerCost(TowerType::Axeman);
        value_left = (int)tower_manager->getPlaceTowerCost(TowerType::Archer);
        value_right = (int)tower_manager->getPlaceTowerCost(TowerType::Gunner);

        region_top = (int)tower_manager->getViewRange(TowerType::Axeman) * TILE_SIZE;
        region_left = (int)tower_manager->getViewRange(TowerType::Archer) * TILE_SIZE;
        region_right = (int)tower_manager->getViewRange(TowerType::Gunner) * TILE_SIZE;

        Panel::onUpdate(renderer);
    }

    void onRender(SDL_Renderer* renderer) override
    {
        if(!visible) return;

        int region = 0;
        switch (hovered_target)
        {
        case Panel::HoveredTarget::TOP:
            region = region_top;
            break;
        case Panel::HoveredTarget::LEFT:
            region = region_left;
            break;
        case Panel::HoveredTarget::RIGHT:
            region = region_right;
            break;
        }

        if (region > 0) {
            filledCircleRGBA(renderer, center_position.x, center_position.y, region, color_region_content.r, color_region_content.g, color_region_content.b, color_region_content.a);
            aacircleRGBA(renderer, center_position.x, center_position.y, region, color_region_frame.r, color_region_frame.g, color_region_frame.b, color_region_frame.a);
        }

        Panel::onRender(renderer);
    }

protected:
    void onClickTopArea() override
    {
        auto* coin_manager = CoinManager::instance();

        if (value_top <= coin_manager->getCurrentCoinNum()) {
            TowerManager::instance()->placeTower(TowerType::Axeman, index_tile_selected);
            coin_manager->decreaseCoin(value_top);
        }
    }

    void onClickLeftArea() override
    {
        auto* coin_manager = CoinManager::instance();

        if (value_left <= coin_manager->getCurrentCoinNum()) {
            TowerManager::instance()->placeTower(TowerType::Archer, index_tile_selected);
            coin_manager->decreaseCoin(value_left);
        }
    }

    void onClickRightArea() override
    {
        auto* coin_manager = CoinManager::instance();

        if (value_right <= coin_manager->getCurrentCoinNum()) {
            TowerManager::instance()->placeTower(TowerType::Gunner, index_tile_selected);
            coin_manager->decreaseCoin(value_right);
        }
    }

private:
    const SDL_Color color_region_frame = { 30, 80, 162, 175 };
    const SDL_Color color_region_content = { 0, 149, 217, 175 };

    int region_top = 0, region_left = 0, region_right = 0;
};