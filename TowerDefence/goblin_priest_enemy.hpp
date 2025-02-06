﻿#pragma once

#include "enemy.hpp"
#include "config_manager.hpp"
#include "resource_manager.hpp"

#include <vector>

/**
 * @brief 哥布林祭司敌人类，继承自Enemy基类
 * @details 实现了哥布林祭司的动画、属性和行为特征
 */
class GoblinPriestEnemy : public Enemy
{
public:
    /**
     * @brief 构造函数，初始化哥布林祭司的所有属性和动画
     * 
     * @details
     * - 加载并设置哥布林祭司的普通和素描纹理
     * - 配置四个方向(上下左右)的动画序列
     * - 从配置模板中初始化哥布林祭司的各项属性
     */
    GoblinPriestEnemy()
    { 
        // 获取纹理资源
        static const auto& texture_pool = ResourceManager::instance()->getTexturePool();
        static auto* tex_goblin_priest = texture_pool.find(ResID::Tex_GoblinPriest)->second;
        static auto* tex_goblin_priest_sketch = texture_pool.find(ResID::Tex_GoblinPriestSketch)->second;
        static auto& goblin_priest_template = ConfigManager::instance()->goblin_priest_template;

        // 定义四个方向的动画帧索引
        static const std::vector<int> index_list_up = { 5, 6, 7, 8, 9 };
        static const std::vector<int> index_list_down = { 0, 1, 2, 3, 4 };
        static const std::vector<int> index_list_left = { 15, 16, 17, 18, 19 };
        static const std::vector<int> index_list_right = { 10, 11, 12, 13, 14 };

        // 设置普通纹理的四向动画
        setAnimation(anim_up, tex_goblin_priest, 5, 4, index_list_up, 0.15);
        setAnimation(anim_down, tex_goblin_priest, 5, 4, index_list_down, 0.15);
        setAnimation(anim_left, tex_goblin_priest, 5, 4, index_list_left, 0.15);
        setAnimation(anim_right, tex_goblin_priest, 5, 4, index_list_right, 0.15);

        // 设置素描纹理的四向动画
        setAnimation(anim_up_sketch, tex_goblin_priest_sketch, 5, 4, index_list_up, 0.15);
        setAnimation(anim_down_sketch, tex_goblin_priest_sketch, 5, 4, index_list_down, 0.15);
        setAnimation(anim_left_sketch, tex_goblin_priest_sketch, 5, 4, index_list_left, 0.15);
        setAnimation(anim_right_sketch, tex_goblin_priest_sketch, 5, 4, index_list_right, 0.15);

        // 从配置模板中初始化基础属性
        max_hp = goblin_priest_template.hp;                            // 最大生命值
        max_speed = goblin_priest_template.speed;                      // 最大生命值
        damage = goblin_priest_template.damage;                        // 伤害值
        reward_ratio = goblin_priest_template.reward_ratio;            // 奖励系数
        recover_interval = goblin_priest_template.recover_interval;    // 治疗间隔
        recover_range = goblin_priest_template.recover_range;          // 治疗范围
        recover_intensity = goblin_priest_template.recover_intensity;  // 治疗强度

        // 设置初始状态
        size.x = 48, size.y = 48;  // 设置碰撞箱大小
        hp = max_hp;               // 初始化当前生命值
        speed = max_speed;         // 初始化当前速度
    }

    /**
     * @brief 析构函数
     */
    ~GoblinPriestEnemy() = default;
};
