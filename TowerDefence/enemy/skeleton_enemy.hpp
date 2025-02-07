#pragma once

#include "enemy.hpp"
#include "../manager/config_manager.hpp"
#include "../manager/resource_manager.hpp"

#include <vector>

/**
 * @brief 骷髅兵敌人类，继承自Enemy基类
 * @details 实现了骷髅兵的动画、属性和行为特征
 */
class SkeletonEnemy : public Enemy
{
public:
    /**
     * @brief 构造函数，初始化骷髅兵的所有属性和动画
     *
     * @details
     * - 加载并设置骷髅兵的普通和素描纹理
     * - 配置四个方向(上下左右)的动画序列
     * - 从配置模板中初始化骷髅兵的各项属性
     */
    SkeletonEnemy()
    {
        // 获取纹理资源
        static const auto& texture_pool = ResourceManager::instance()->getTexturePool();
        static auto* tex_skeleton = texture_pool.find(ResID::Tex_Skeleton)->second;
        static auto* tex_skeleton_sketch = texture_pool.find(ResID::Tex_SkeletonSketch)->second;
        static auto& skeleton_template = ConfigManager::instance()->skeleton_template;

        // 定义四个方向的动画帧索引
        static const std::vector<int> index_list_up = { 5, 6, 7, 8, 9 };
        static const std::vector<int> index_list_down = { 0, 1, 2, 3, 4 };
        static const std::vector<int> index_list_left = { 15, 16, 17, 18, 19 };
        static const std::vector<int> index_list_right = { 10, 11, 12, 13, 14 };

        // 设置普通纹理的四向动画
        setAnimation(anim_up, tex_skeleton, 5, 4, index_list_up, 0.15);
        setAnimation(anim_down, tex_skeleton, 5, 4, index_list_down, 0.15);
        setAnimation(anim_left, tex_skeleton, 5, 4, index_list_left, 0.15);
        setAnimation(anim_right, tex_skeleton, 5, 4, index_list_right, 0.15);

        // 设置素描纹理的四向动画
        setAnimation(anim_up_sketch, tex_skeleton_sketch, 5, 4, index_list_up, 0.15);
        setAnimation(anim_down_sketch, tex_skeleton_sketch, 5, 4, index_list_down, 0.15);
        setAnimation(anim_left_sketch, tex_skeleton_sketch, 5, 4, index_list_left, 0.15);
        setAnimation(anim_right_sketch, tex_skeleton_sketch, 5, 4, index_list_right, 0.15);

        // 从配置模板中初始化基础属性
        max_hp = skeleton_template.hp;                            // 最大生命值
        max_speed = skeleton_template.speed;                      // 最大生命值
        damage = skeleton_template.damage;                        // 伤害值
        reward_ratio = skeleton_template.reward_ratio;            // 奖励系数
        recover_interval = skeleton_template.recover_interval;    // 治疗间隔
        recover_range = skeleton_template.recover_range;          // 治疗范围
        recover_intensity = skeleton_template.recover_intensity;  // 治疗强度

        size.x = 48, size.y = 48;  // 设置碰撞箱大小
        hp = max_hp;               // 初始化当前生命值
        speed = max_speed;         // 初始化当前速度
    }

    /**
     * @brief 析构函数
     */
    ~SkeletonEnemy() = default;
};
