#pragma once

#include "manager.hpp"
#include "config_manager.hpp"
#include "enemy_manager.hpp"
#include "coin_manager.hpp"
#include "../basic/wave.hpp"
#include "../util/timer.hpp"

/**
 * @brief 游戏波次管理器，负责控制敌人波次的刷新和进程
 *
 * 该类继承自Manager模板类，使用单例模式实现
 * 管理游戏中的波次进程，包括波次间隔、敌人生成等
 */
class WaveManager : public Manager<WaveManager> 
{
    friend class Manager<WaveManager>;

public:
    /**
     * @brief 更新波次状态
     * @param deltaTime 时间增量
     *
     * 主要功能：
     * 1. 控制波次开始的计时
     * 2. 处理敌人生成事件
     * 3. 检查波次完成状态并处理奖励
     * 4. 处理游戏结束条件
     */
    void onUpdate(double delta_time)
    {
        static auto* config = ConfigManager::instance();
        if (config->is_game_over) return;

        if (!is_wave_started)
            timer_start_wave.onUpdate(delta_time);
        else
            timer_spawn_event.onUpdate(delta_time);

        if (is_spawned_last_event && EnemyManager::instance()->checkCleared()) {
            CoinManager::instance()->increaseCoin(config->wave_list[index_wave].rewards);
            index_wave++;

            if (index_wave >= config->wave_list.size()) {
                config->is_game_win = true;
                config->is_game_over = true;
                return;
            }
            else {
                is_wave_started = false;
                is_spawned_last_event = false;
                index_spawn_event = 0;

                timer_start_wave.setWaitTime(config->wave_list[index_wave].interval);
                timer_start_wave.restart();
            }
        }
    }

protected:
    /**
     * @brief 构造函数，初始化波次管理器
     *
     * 设置两个计时器：
     * 1. timer_start_wave: 控制波次开始的间隔
     * 2. timer_spawn_event: 控制敌人生成的间隔
     */
    WaveManager()
    {
        static const auto& wave_list = ConfigManager::instance()->wave_list;

        timer_start_wave.setOneShot(true);
        timer_start_wave.setWaitTime(wave_list[0].interval);
        timer_start_wave.setOnTimeOut(
            [&]() {
                is_wave_started = true;
                timer_spawn_event.setWaitTime(wave_list[index_wave].spawn_event_list[0].interval);
                timer_spawn_event.restart();
            });

        timer_spawn_event.setOneShot(true);
        timer_spawn_event.setOnTimeOut(
            [&]() {
                const auto& spawn_event_list = wave_list[index_wave].spawn_event_list;
                const auto& spawn_event = spawn_event_list[index_spawn_event];

                EnemyManager::instance()->spawnEnemy(spawn_event.enemy_type, spawn_event.spawn_point);

                index_spawn_event++;
                if (index_spawn_event >= spawn_event_list.size()) {
                    is_spawned_last_event = true;
                    return;
                }

                timer_spawn_event.setWaitTime(spawn_event_list[index_spawn_event].interval);
                timer_spawn_event.restart();
            });
    }
    ~WaveManager() = default;

private:
    int index_wave = 0;          // 当前波次索引
    int index_spawn_event = 0;    // 当前生成事件索引

    Timer timer_start_wave;       // 控制波次开始的计时器
    Timer timer_spawn_event;      // 控制敌人生成的计时器

    bool is_wave_started = false;         // 当前波次是否已开始
    bool is_spawned_last_event = false;   // 是否已生成当前波次的最后一个敌人
};
