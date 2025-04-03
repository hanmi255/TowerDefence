#pragma once

#include "manager.hpp"
#include "config_manager.hpp"
#include "../basic/coin_prop.hpp"

#include <vector>
#include <memory>
#include <algorithm>

/**
 * @brief 金币管理器类，负责管理游戏中的金币数量
 * @details 继承自Manager模板类，使用单例模式实现
 *          负责处理金币的增减、金币道具的生成和管理等功能
 */
class CoinManager : public Manager<CoinManager>
{
    friend class Manager<CoinManager>;

public:
    using CoinPropList = std::vector<CoinProp*>;  // 金币道具列表类型别名

public:
    /**
     * @brief 增加金币数量
     * @param num 要增加的金币数量
     */
    void increaseCoin(double num) { m_num_coin += num; }

    /**
     * @brief 减少金币数量
     * @param num 要减少的金币数量
     * @details 金币数量不会小于0，如果减少后小于0则设为0
     */
    void decreaseCoin(double num)
    {
        m_num_coin -= num;
        if (m_num_coin < 0) m_num_coin = 0;
    }

    /**
     * @brief 更新金币系统
     * @param delta_time 帧间隔时间
     * @details 更新所有金币道具的状态，并清理可移除的道具
     */
    void onUpdate(double delta_time)
    {
        // 更新所有金币道具
        for (auto* coin_prop : m_coin_prop_list) {
            coin_prop->onUpdate(delta_time);
        }

        // 移除并删除标记为可删除的金币道具
        m_coin_prop_list.erase(std::remove_if(
            m_coin_prop_list.begin(), m_coin_prop_list.end(),
            [](CoinProp* coin_prop) {
                bool deletable = coin_prop->canRemove();
                if (deletable) delete coin_prop;
                return deletable;
            }), m_coin_prop_list.end());
    }

    /**
     * @brief 渲染金币系统
     * @param renderer SDL渲染器指针
     * @details 渲染所有活跃的金币道具
     */
    void onRender(SDL_Renderer* renderer)
    {
        for (auto* coin_prop : m_coin_prop_list) {
            coin_prop->onRender(renderer);
        }
    }

    /**
     * @brief 获取当前金币数量
     * @return 当前金币数量
     */
    double getCurrentCoinNum() const
    {
        return m_num_coin;
    }

    /**
     * @brief 获取金币道具列表的引用
     * @return 金币道具列表引用
     */
    CoinPropList& getCoinPropList()
    {
        return m_coin_prop_list;
    }

    /**
     * @brief 在指定位置生成金币道具
     * @param position 生成位置的坐标
     * @details 使用智能指针确保内存安全，将所有权转移到列表中
     */
    void spawnCoinProp(const Vector2& position)
    {
        std::unique_ptr<CoinProp> coin_prop = std::make_unique<CoinProp>();
        coin_prop->setPosition(position);
        m_coin_prop_list.push_back(coin_prop.release());
    }

protected:
    /**
     * @brief 构造函数
     * @details 从配置管理器获取初始金币数量
     */
    CoinManager()
    {
        m_num_coin = ConfigManager::instance()->num_initial_coin;
    }

    /**
     * @brief 析构函数
     * @details 清理所有动态分配的金币道具
     */
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
