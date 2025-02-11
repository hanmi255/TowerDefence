#pragma once

#include "tower.hpp"
#include "../manager/resource_manager.hpp"

/**
 * @brief 枪手塔类，继承自基础防御塔类
 * @details 实现了枪手塔的动画、属性和行为特征
 */
class GunnerTower : public Tower
{
public:
	/**
	 * @brief 构造函数，初始化枪手塔的动画和属性
	 * @details
	 * - 加载并设置枪手塔的空闲和攻击纹理
	 * - 配置四个方向(上下左右)的动画序列
	 * - 初始化枪手塔的各项属性
	 */
	GunnerTower()
	{
		// 获取纹理资源
		static auto* tex_gunner = ResourceManager::instance()->getTexturePool().find(ResID::Tex_Gunner)->second;

		// 定义空闲状态四个方向的动画帧索引
		static const std::vector<int> idx_list_idle_up = { 4, 5 };
		static const std::vector<int> idx_list_idle_down = { 0, 1 };
		static const std::vector<int> idx_list_idle_left = { 12, 13 };
		static const std::vector<int> idx_list_idle_right = { 8, 9 };

		// 定义攻击状态四个方向的动画帧索引
		static const std::vector<int> idx_list_fire_up = { 20, 21, 22, 23 };
		static const std::vector<int> idx_list_fire_down = { 16, 17, 18, 19 };
		static const std::vector<int> idx_list_fire_left = { 28, 29, 30, 31 };
		static const std::vector<int> idx_list_fire_right = { 24, 25, 26, 27 };

		// 设置空闲状态动画
		setAnimation(anim_idle_up, tex_gunner, 4, 8, idx_list_idle_up);
		setAnimation(anim_idle_down, tex_gunner, 4, 8, idx_list_idle_down);
		setAnimation(anim_idle_left, tex_gunner, 4, 8, idx_list_idle_left);
		setAnimation(anim_idle_right, tex_gunner, 4, 8, idx_list_idle_right);

		// 设置攻击状态动画
		setAnimation(anim_fire_up, tex_gunner, 4, 8, idx_list_fire_up);
		setAnimation(anim_fire_down, tex_gunner, 4, 8, idx_list_fire_down);
		setAnimation(anim_fire_left, tex_gunner, 4, 8, idx_list_fire_left);
		setAnimation(anim_fire_right, tex_gunner, 4, 8, idx_list_fire_right);

		size.x = 48, size.y = 48;		  // 设置碰撞箱大小

		tower_type = TowerType::Gunner;	  // 设置塔的类型为枪手塔

		fire_speed = 7;					  // 设置攻击速度
		bullet_type = BulletType::Shell;  // 设置子弹类型为炮弹
	}

	/**
	 * @brief 析构函数
	 */
	~GunnerTower() = default;
};

