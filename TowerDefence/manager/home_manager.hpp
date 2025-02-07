#pragma once

#include "manager.hpp"
#include "config_manager.hpp"
#include "resource_manager.hpp"

/**
 * @brief 家园管理器类，负责处理基地血量相关的逻辑
 * @details 继承自Manager单例模板类，管理基地的生命值系统
 */
class HomeManager : public Manager<HomeManager> 
{
    friend class Manager<HomeManager>;

public:
    /**
     * @brief 获取当前基地血量
     * @return double 返回当前血量值
     */
    double getCurrentHPNum() const { return num_hp; }

    /**
     * @brief 减少基地血量并播放受伤音效
     * @param damage 受到的伤害值
     * @details 血量不会低于0，受伤时会播放对应音效
     */
    void decreaseHP(double damage) {
        num_hp -= damage;
        if (num_hp < 0) num_hp = 0;

        // 获取音效资源池的静态引用
        static const ResourceManager::SoundPool& sound_pool = ResourceManager::instance()->getSoundPool();

        // 播放基地受伤音效
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_HomeHurt)->second, 0);
    }

protected:
    HomeManager() {
        num_hp = ConfigManager::instance()->num_initial_hp;
    }
    ~HomeManager() = default;

private:
    double num_hp = 0;  // 当前基地血量值
};
