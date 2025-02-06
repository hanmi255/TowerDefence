#pragma once

#include "manager.hpp"
#include "config_manager.hpp"
#include "coin_prop.hpp"

#include <vector>
#include <memory>

/**
 * @brief 金币管理器类，负责管理游戏中的金币数量
 * @details 继承自Manager模板类，使用单例模式实现
 */
class CoinManager : public Manager<CoinManager> 
{
    friend class Manager<CoinManager>;

public:
    using CoinPropList = std::vector<CoinProp*>;

public:
    /**
     * @brief 增加金币数量
     * @param num 要增加的金币数量
     */
    void increaseCoin(double num) { m_num_coin += num; }

    /**
     * @brief 减少金币数量
     * @param num 要减少的金币数量
     * @details 金币数量不会小于0
     */
    void decreaseCoin(double num)
    {
        m_num_coin -= num;
        if (m_num_coin < 0) m_num_coin = 0;
    }

    void onUpdate(double delta_time)
    {
        for (auto* coin_prop : m_coin_prop_list) {
            coin_prop->onUpdate(delta_time);
        }

        m_coin_prop_list.erase(std::remove_if(
            m_coin_prop_list.begin(), m_coin_prop_list.end(),
            [](CoinProp* coin_prop) {
                bool deletable = coin_prop->canRemove();
                if (deletable) delete coin_prop;

                return deletable;
            }), m_coin_prop_list.end());
    }

    void onRender(SDL_Renderer* renderer)
    {
        for (auto* coin_prop : m_coin_prop_list) {
            coin_prop->onRender(renderer);
        }
    }

    double getCurrentCoinNum() const 
    { 
        return m_num_coin; 
    }

    CoinPropList& getCoinPropList() 
    { 
        return m_coin_prop_list; 
    }

    void spawnCoinProp(const Vector2& position)
    {
        std::unique_ptr<CoinProp> coin_prop = std::make_unique<CoinProp>();
        coin_prop->setPosition(position);

        m_coin_prop_list.push_back(coin_prop.release());
    }

protected:
    CoinManager()
    {
        m_num_coin = ConfigManager::instance()->num_initial_coin;
    }

    ~CoinManager()
    {
        for (auto* coin_prop : m_coin_prop_list) {
            delete coin_prop;
        }
    }

private:
    double m_num_coin = 0;          // 当前金币数量
    CoinPropList m_coin_prop_list;  // 金币道具列表
};