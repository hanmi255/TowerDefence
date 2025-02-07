#pragma once

#include "tower.hpp"	
#include "../manager/resource_manager.hpp"

/**
 * @brief 弓箭手塔类，继承自基础防御塔类
 * @details 实现了弓箭手塔的动画、属性和行为特征
 */
class ArcherTower : public Tower
{
public:
	/**
	 * @brief 构造函数，初始化弓箭手塔的动画和属性
	 * @details
	 * - 加载并设置弓箭手塔的空闲和攻击纹理
	 * - 配置四个方向(上下左右)的动画序列
	 * - 初始化弓箭手塔的各项属性
	 */
	ArcherTower()
	{
		// 获取纹理资源
		static const auto& texture_pool = ResourceManager::instance()->getTexturePool();
		static auto* tex_archer = texture_pool.find(ResID::Tex_Archer)->second;

		// 定义空闲状态四个方向的动画帧索引
		static const std::vector<int> idx_list_idle_up = { 3, 4 };
		static const std::vector<int> idx_list_idle_down = { 0, 1 };
		static const std::vector<int> idx_list_idle_left = { 6, 7 };
		static const std::vector<int> idx_list_idle_right = { 9, 10 };

		// 定义攻击状态四个方向的动画帧索引
		static const std::vector<int> idx_list_fire_up = { 15, 16, 17 };
		static const std::vector<int> idx_list_fire_down = { 12, 13, 14 };
		static const std::vector<int> idx_list_fire_left = { 18, 19, 20 };
		static const std::vector<int> idx_list_fire_right = { 21, 22, 23 };

		// 设置空闲状态动画
		setAnimation(anim_idle_up, tex_archer, 3, 8, idx_list_idle_up);
		setAnimation(anim_idle_down, tex_archer, 3, 8, idx_list_idle_down);
		setAnimation(anim_idle_left, tex_archer, 3, 8, idx_list_idle_left);
		setAnimation(anim_idle_right, tex_archer, 3, 8, idx_list_idle_right);

		// 设置攻击状态动画
		setAnimation(anim_fire_up, tex_archer, 3, 8, idx_list_fire_up);
		setAnimation(anim_fire_down, tex_archer, 3, 8, idx_list_fire_down);
		setAnimation(anim_fire_left, tex_archer, 3, 8, idx_list_fire_left);
		setAnimation(anim_fire_right, tex_archer, 3, 8, idx_list_fire_right);

		size.x = 48, size.y = 48;		  // 设置碰撞箱大小

		tower_type = TowerType::Archer;	  // 设置塔的类型为弓箭塔

		fire_speed = 6;					  // 设置攻击速度
		bullet_type = BulletType::Arrow;  // 设置子弹类型为箭矢
	}

	/**
	 * @brief 析构函数
	 */
	~ArcherTower() = default;
};

