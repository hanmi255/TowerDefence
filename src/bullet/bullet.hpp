#pragma once

#include "../util/vector2.hpp"
#include "../util/animation.hpp"
#include "../enemy/enemy.hpp"
#include "../manager/config_manager.hpp"

/**
 * @class Bullet
 * @brief 游戏中的子弹类，处理子弹的移动、碰撞和渲染逻辑
 *
 * 该类实现了子弹的基本功能，包括：
 * - 位置和速度控制
 * - 伤害值设定
 * - 碰撞检测
 * - 动画渲染
 */
class Bullet
{
public:
	Bullet() = default;
	~Bullet() = default;

	/**
	 * @brief 设置子弹速度并计算旋转角度
	 * @param velocity 速度向量
	 */
	void setVelocity(const Vector2& velocity)
	{
		this->velocity = velocity;

		if (can_rotate) {
			double randian = std::atan2(velocity.y, velocity.x);
			angle_anim_rotate = randian * 180 / 3.14159265358979323846;
		}
	}

	/**
	 * @brief 设置子弹位置
	 * @param position 位置向量
	 */
	void setPosition(const Vector2& position)
	{
		this->position = position;
	}
	
	/**
	 * @brief 设置子弹伤害值
	 * @param damage 伤害值
	 */
	void setDamage(double damage)
	{
		this->damage = damage;
	}

	/**
	 * @brief 获取子弹大小
	 * @return 子弹大小向量
	 */
	const Vector2& getSize() const
	{
		return size;
	}

	/**
	 * @brief 获取子弹位置
	 * @return 子弹位置向量
	 */
	const Vector2& getPosition() const
	{
		return position;
	}

	/**
	 * @brief 获取子弹伤害值
	 * @return 伤害值
	 */
	double getDamage() const
	{
		return damage;
	}

	/**
	 * @brief 获取子弹伤害范围
	 * @return 伤害范围
	 */
	double getDamageRange() const
	{
		return damage_range;
	}

	/**
	 * @brief 禁用子弹碰撞
	 */
	void disableCollide()
	{
		is_collisional = false;
	}

	/**
	 * @brief 检查子弹是否可碰撞
	 * @return 是否可碰撞
	 */
	bool canCollide() const
	{
		return is_collisional;
	}

	/**
	 * @brief 使子弹失效
	 */
	void makeInvalid()
	{
		is_valid = false;
		is_collisional = false;
	}

	/**
	 * @brief 检查子弹是否可移除
	 * @return 是否可移除
	 */
	bool canRemove() const
	{
		return !is_valid;
	}

	/**
	 * @brief 更新子弹状态
	 * @param delta_time 时间增量
	 *
	 * 更新子弹动画和位置，检查地图边界碰撞
	 */
	virtual void onUpdate(double delta_time)
	{
		animation.onUpdate(delta_time);
		position += velocity * delta_time;

		static const SDL_Rect& rect_map = ConfigManager::instance()->rect_tile_map;

		if (position.x - size.x / 2 <= rect_map.x
			|| position.x + size.x / 2 >= rect_map.x + rect_map.w
			|| position.y - size.y / 2 <= rect_map.y
			|| position.y + size.y / 2 >= rect_map.y + rect_map.h) {
			is_valid = false;
		}
	}

	/**
	 * @brief 渲染子弹
	 * @param renderer SDL渲染器指针
	 */
	virtual void onRender(SDL_Renderer* renderer)
	{
		static SDL_Point point;

		point.x = (int)(position.x - size.x / 2);
		point.y = (int)(position.y - size.y / 2);

		animation.onRender(renderer, point, angle_anim_rotate);
	}

	/**
	 * @brief 处理与敌人的碰撞
	 * @param enemy 敌人指针
	 */
	virtual void onCollide(Enemy* enemy)
	{
		is_valid = false;
		is_collisional = false;
	}

protected:
	Vector2 size;					 // 子弹大小
	Vector2 position;				 // 子弹位置
	Vector2 velocity;				 // 子弹速度

	Animation animation;			 // 子弹动画

	bool can_rotate = false;		 // 是否可旋转

	double damage = 0.0;			 // 伤害值
	double damage_range = -1;		 // 伤害范围

private:
	bool is_valid = true;			 // 子弹是否有效
	bool is_collisional = true;		 // 是否可碰撞
	double angle_anim_rotate = 0.0;  // 动画旋转角度
};
