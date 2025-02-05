#pragma once

#include "enemy.hpp"
#include "config_manager.hpp"
#include "resource_manager.hpp"

/**
 * @brief 皇家史莱姆敌人类，继承自基础敌人类
 * @details 实现了皇家史莱姆的动画、属性和行为特征
 */
class KingSlimeEnemy : public Enemy
{
public:
	/**
	 * @brief 构造函数，初始化皇家史莱姆祭司的所有属性和动画
	 *
	 * @details
	 * - 加载并设置皇家史莱姆祭司的普通和素描纹理
	 * - 配置四个方向(上下左右)的动画序列
	 * - 从配置模板中初始化皇家史莱姆祭司的各项属性
	 */
	KingSlimeEnemy()
	{
		// 获取纹理资源
		static const auto& texture_pool = ResourceManager::instance()->getTexturePool();
		static auto* tex_king_slime = texture_pool.find(ResID::Tex_KingSlime)->second;
		static auto* tex_king_slime_sketch = texture_pool.find(ResID::Tex_KingSlimeSketch)->second;
		static auto& king_slime_template = ConfigManager::instance()->king_slime_template;

		// 定义四个方向的动画帧索引
		static const std::vector<int> index_list_down = { 0, 1, 2, 3, 4, 5 };
		static const std::vector<int> index_list_up = { 6, 7, 8, 9, 10, 11 };
		static const std::vector<int> index_list_right = { 12, 13, 14, 15, 16, 17 };
		static const std::vector<int> index_list_left = { 18, 19, 20, 21, 22, 23 };

		// 设置普通纹理的四向动画
		setAnimation(anim_up, tex_king_slime, 6, 4, index_list_up, 0.1);
		setAnimation(anim_down, tex_king_slime, 6, 4, index_list_down, 0.1);
		setAnimation(anim_left, tex_king_slime, 6, 4, index_list_left, 0.1);
		setAnimation(anim_right, tex_king_slime, 6, 4, index_list_right, 0.1);

		// 设置素描纹理的四向动画
		setAnimation(anim_up_sketch, tex_king_slime_sketch, 6, 4, index_list_up, 0.1);
		setAnimation(anim_down_sketch, tex_king_slime_sketch, 6, 4, index_list_down, 0.1);
		setAnimation(anim_left_sketch, tex_king_slime_sketch, 6, 4, index_list_left, 0.1);
		setAnimation(anim_right_sketch, tex_king_slime_sketch, 6, 4, index_list_right, 0.1);

		// 从配置模板中初始化基础属性
		max_hp = king_slime_template.hp;                            // 最大生命值
		max_speed = king_slime_template.speed;                      // 最大生命值
		damage = king_slime_template.damage;                        // 伤害值
		reward_ratio = king_slime_template.reward_ratio;            // 奖励系数
		recover_interval = king_slime_template.recover_interval;    // 治疗间隔
		recover_range = king_slime_template.recover_range;          // 治疗范围
		recover_intensity = king_slime_template.recover_intensity;  // 治疗强度

		size.x = 48, size.y = 48;  // 设置碰撞箱大小
		hp = max_hp;               // 初始化当前生命值
		speed = max_speed;         // 初始化当前速度
	}

	/**
	 * @brief 析构函数
	 */
	~KingSlimeEnemy() = default;
};