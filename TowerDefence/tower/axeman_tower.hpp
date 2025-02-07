#pragma once

#include "tower.hpp"	
#include "../manager/resource_manager.hpp"

/**
 * @brief 斧头兵塔类，继承自基础防御塔类
 * @details 实现了斧头兵塔的动画、属性和行为特征
 */
class AxemanTower : public Tower
{
public:
	/**
	 * @brief 构造函数，初始化斧头兵塔的动画和属性
	 * @details
	 * - 加载并设置斧头兵塔的空闲和攻击纹理
	 * - 配置四个方向(上下左右)的动画序列
	 * - 初始化斧头兵塔的各项属性
	 */
	AxemanTower()
	{
		// 获取纹理资源
		static auto* tex_axeman = ResourceManager::instance()->getTexturePool().find(ResID::Tex_Axeman)->second;

		// 定义空闲状态四个方向的动画帧索引
		static const std::vector<int> idx_list_idle_up = { 3, 4 };
		static const std::vector<int> idx_list_idle_down = { 0, 1 };
		static const std::vector<int> idx_list_idle_left = { 9, 10 };
		static const std::vector<int> idx_list_idle_right = { 6, 7 };

		// 定义攻击状态四个方向的动画帧索引
		static const std::vector<int> idx_list_fire_up = { 15, 16, 17 };
		static const std::vector<int> idx_list_fire_down = { 12, 13, 14 };
		static const std::vector<int> idx_list_fire_left = { 21, 22, 23 };
		static const std::vector<int> idx_list_fire_right = { 18, 19, 20 };

		// 设置空闲状态动画
		setAnimation(anim_idle_up, tex_axeman, 3, 8, idx_list_idle_up);
		setAnimation(anim_idle_down, tex_axeman, 3, 8, idx_list_idle_down);
		setAnimation(anim_idle_left, tex_axeman, 3, 8, idx_list_idle_left);
		setAnimation(anim_idle_right, tex_axeman, 3, 8, idx_list_idle_right);

		// 设置攻击状态动画
		setAnimation(anim_fire_up, tex_axeman, 3, 8, idx_list_fire_up);
		setAnimation(anim_fire_down, tex_axeman, 3, 8, idx_list_fire_down);
		setAnimation(anim_fire_left, tex_axeman, 3, 8, idx_list_fire_left);
		setAnimation(anim_fire_right, tex_axeman, 3, 8, idx_list_fire_right);

		size.x = 48, size.y = 48;		  // 设置碰撞箱大小

		tower_type = TowerType::Axeman;	  // 设置塔的类型为斧头兵塔

		fire_speed = 5;					  // 设置攻击速度
		bullet_type = BulletType::Axe;  // 设置子弹类型为斧头
	}

	/**
	 * @brief 析构函数
	 */
	~AxemanTower() = default;
};

