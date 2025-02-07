#pragma once

#include "../util/vector2.hpp"
#include "../util/animation.hpp"
#include "../util/timer.hpp"
#include "../tower/tower_type.hpp"
#include "../basic/facing.hpp"
#include "../manager/bullet_manager.hpp"
#include "../manager/enemy_manager.hpp"

#include <functional>

/**
 * @class Tower
 * @brief 游戏中的防御塔基类，实现了防御塔的基本功能
 *
 * 包含以下主要功能：
 * - 防御塔的位置管理和渲染
 * - 多方向的静止和攻击动画
 * - 自动搜索和攻击范围内的敌人
 * - 不同类型防御塔的属性配置
 */
class Tower
{
public:
	/**
	 * @brief 构造函数，初始化防御塔的基本属性和动画
	 *
	 * 设置开火定时器和各个方向的动画属性
	 */
	Tower()
	{
		timer_fire.setOneShot(true);
		timer_fire.setOnTimeOut([&] { can_fire = true; });

		setAnimationProperties(anim_idle_up, true, 0.2);
		setAnimationProperties(anim_idle_down, true, 0.2);
		setAnimationProperties(anim_idle_left, true, 0.2);
		setAnimationProperties(anim_idle_right, true, 0.2);

		setAnimationProperties(anim_fire_up, false, 0.2, [&] { updateIdleAnimation(); });
		setAnimationProperties(anim_fire_down, false, 0.2, [&] { updateIdleAnimation(); });
		setAnimationProperties(anim_fire_left, false, 0.2, [&] { updateIdleAnimation(); });
		setAnimationProperties(anim_fire_right, false, 0.2, [&] { updateIdleAnimation(); });
	}

	~Tower() = default;

	/**
	 * @brief 设置防御塔的位置
	 * @param position 新的位置坐标
	 */
	void setPosition(const Vector2& position)
	{
		this->position = position;
	}

	/**
	 * @brief 获取防御塔的位置
	 * @return 返回防御塔的位置坐标
	 */
	const Vector2& getPosition() const
	{
		return position;
	}

	/**
	 * @brief 获取防御塔的大小
	 * @return 返回防御塔的大小
	 */
	const Vector2& getSize() const
	{
		return size;
	}

	/**
	 * @brief 更新防御塔的状态
	 * @param delta_time 时间增量
	 *
	 * 更新开火定时器和当前动画，检查是否可以开火
	 */
	void onUpdate(double delta_time)
	{
		timer_fire.onUpdate(delta_time);
		anim_current->onUpdate(delta_time);

		if (can_fire)
			onFire();
	}

	/**
	* @brief 渲染防御塔
	* @param renderer SDL渲染器指针
	*/
	void onRender(SDL_Renderer* renderer)
	{
		static SDL_Point point;
		point.x = (int)(position.x - size.x / 2);
		point.y = (int)(position.y - size.y / 2);
		anim_current->onRender(renderer, point);
	}

protected:
	/**
	 * @brief 设置动画
	 * @param anim 动画对象
	 * @param texture 动画纹理
	 * @param num_h 动画横向帧数
	 * @param num_v 动画纵向帧数
	 * @param index_list 动画帧索引列表
	 *
	 * 设置动画的贴图、帧索引列表等属性
	 */
	void setAnimation(Animation& anim, SDL_Texture* texture, int num_h, int num_v, const std::vector<int>& index_list)
	{
		anim.setFrameData(texture, num_h, num_v, index_list);
	}
protected:
	Vector2 size;								 // 防御塔的大小

	Animation anim_idle_up;						 // 向上静止动画
	Animation anim_idle_down;					 // 向下静止动画
	Animation anim_idle_left;					 // 向左静止动画
	Animation anim_idle_right;					 // 向右静止动画

	Animation anim_fire_up;						 // 向上开火动画
	Animation anim_fire_down;					 // 向下开火动画
	Animation anim_fire_left;					 // 向左开火动画
	Animation anim_fire_right;					 // 向右开火动画

	TowerType tower_type;						 // 防御塔类型
	double fire_speed;							 // 射击速度
	BulletType bullet_type;						 // 子弹类型

private:
	Timer timer_fire;							 // 开火计时器
	Vector2 position;							 // 防御塔位置
	bool can_fire;								 // 是否可以开火
	Facing facing;								 // 朝向
	Animation* anim_current = &anim_idle_right;  // 当前播放的动画

private:
	/**
	 * @brief 更新静止状态的动画
	 * 根据当前朝向更新对应的静止动画
	 */
	void updateIdleAnimation()
	{
		switch (facing)
		{
		case Facing::UP:
			anim_current = &anim_idle_up;
			break;
		case Facing::DOWN:
			anim_current = &anim_idle_down;
			break;
		case Facing::LEFT:
			anim_current = &anim_idle_left;
			break;
		case Facing::RIGHT:
			anim_current = &anim_idle_right;
			break;
		}
	}

	/**
	 * @brief 更新开火状态的动画
	 * 根据当前朝向更新对应的开火动画
	 */
	void updateFireAnimation()
	{
		switch (facing)
		{
		case Facing::UP:
			anim_current = &anim_fire_up;
			break;											
		case Facing::DOWN:									
			anim_current = &anim_fire_down;
			break;											
		case Facing::LEFT:									
			anim_current = &anim_fire_left;
			break;											
		case Facing::RIGHT:									
			anim_current = &anim_fire_right;
			break;
		}
	}

	/**
	 * @brief 设置动画属性
	 * @param anim 要设置的动画
	 * @param loop 是否循环播放
	 * @param interval 帧间隔
	 * @param call_back 动画结束回调函数
	 */
	void setAnimationProperties(Animation& anim, bool loop, double interval, std::function<void()> call_back = nullptr)
	{
		anim.setLoop(loop);
		anim.setInterval(interval);
		if (call_back)
			anim.setOnFinished(call_back);
	}

	/**
	 * @brief 寻找目标敌人
	 * @return 返回找到的目标敌人指针，如果没找到返回nullptr
	 *
	 * 在视野范围内寻找最接近终点的敌人作为攻击目标
	 */
	Enemy* findTargetEnemy() const
	{
		double process = -1.0;
		double view_range = 0.0;
		Enemy* target_enemy = nullptr;

		static auto* config = ConfigManager::instance();

		switch (tower_type) {
		case TowerType::Archer:
			view_range = config->archer_template.view_range[config->level_archer];
			break;
		case TowerType::Axeman:
			view_range = config->axeman_template.view_range[config->level_axeman];
			break;
		case TowerType::Gunner:
			view_range = config->gunner_template.view_range[config->level_gunner];
			break;
		}

		auto& enemy_list = EnemyManager::instance()->getEnemyList();

		for (auto* enemy : enemy_list) {
			if ((enemy->getPosition() - position).length() <= view_range * TILE_SIZE) {
				double new_process = enemy->getRouteProcess();
				if (new_process > process) {
					target_enemy = enemy;
					process = new_process;
				}
			}
		}

		return target_enemy;
	}

	/**
	 * @brief 处理开火逻辑
	 *
	 * 包含以下步骤：
	 * 1. 寻找目标敌人
	 * 2. 计算攻击间隔和伤害
	 * 3. 播放音效
	 * 4. 发射子弹
	 * 5. 更新动画状态
	 */
	void onFire()
	{
		auto target_enemy = findTargetEnemy();
		if (!target_enemy) return;

		can_fire = false;
		static auto* config = ConfigManager::instance();
		static const auto& sound_pool = ResourceManager::instance()->getSoundPool();

		double interval = 0.0, damage = 0.0;

		switch (tower_type) {
		case TowerType::Archer:
			interval = config->archer_template.interval[config->level_archer];
			damage = config->archer_template.damage[config->level_archer];

			switch (rand() % 2) {
			case 0:
				Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowFire_1)->second, 0);
				break;
			case 1:
				Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowFire_2)->second, 0);
				break;
			}

			break;
		case TowerType::Axeman:
			interval = config->axeman_template.interval[config->level_axeman];
			damage = config->axeman_template.damage[config->level_axeman];
			Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeFire)->second, 0);
			break;
		case TowerType::Gunner:
			interval = config->gunner_template.interval[config->level_gunner];
			damage = config->gunner_template.damage[config->level_gunner];
			Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ShellFire)->second, 0);
			break;
		}

		timer_fire.setWaitTime(interval);
		timer_fire.restart();

		Vector2 direction = target_enemy->getPosition() - position;
		BulletManager::instance()->fireBullet(bullet_type, position, direction.normalize() * fire_speed * TILE_SIZE, damage);

		bool is_show_x_anim = abs(direction.x) >= abs(direction.y);
		if (is_show_x_anim) 
			facing = direction.x > 0 ? Facing::RIGHT : Facing::LEFT;
		else 
			facing = direction.y > 0 ? Facing::DOWN : Facing::UP;

		updateFireAnimation();
		anim_current->reset();
	}
};

