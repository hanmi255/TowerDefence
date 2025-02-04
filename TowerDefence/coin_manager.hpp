#pragma once

#include "manager.hpp"

/**
 * @brief 金币管理器类，负责管理游戏中的金币数量
 * @details 继承自Manager模板类，使用单例模式实现
 */
class CoinManager : public Manager<CoinManager> 
{
    friend class Manager<CoinManager>;

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

protected:
    CoinManager() = default;
    ~CoinManager() = default;

private:
    double m_num_coin = 0;  // 当前金币数量
};