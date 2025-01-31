#pragma once

#include "enemy_type.h"
#include <vector>

/**
 * @brief 描述一波敌人的生成配置
 */
struct Wave
{
    /**
     * @brief 单个敌人生成事件的配置
     */
    struct SpawnEvent
    {
        double interval = 0;                      // 当前事件与上一个事件的时间间隔(秒)
        int spawn_point = 1;                      // 敌人的生成点编号
        EnemyType enemy_type = EnemyType::Slime;  // 要生成的敌人类型
    };

    double rewards = 0;                           // 完成该波次可获得的奖励
    double interval = 0;                          // 与下一波的时间间隔(秒)
    std::vector<SpawnEvent> spawn_event_list;     // 该波次包含的所有生成事件列表
};
