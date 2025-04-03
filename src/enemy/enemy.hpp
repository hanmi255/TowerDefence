#pragma once

#include "../util/vector2.hpp"
#include "../util/timer.hpp"
#include "../util/animation.hpp"
#include "../basic/route.hpp"
#include "../manager/config_manager.hpp"

#include <memory>
#include <functional>

/**
 * @brief 游戏中的敌人类，实现敌人的移动、动画、状态管理等功能
 *
 * 该类管理敌人的：
 * - 生命值和伤害系统
 * - 移动和路径跟踪
 * - 动画状态
 * - 技能系统
 */
class Enemy
{
public:
	/** 技能回调函数类型定义 */
	using SkillCallback = std::function<void(Enemy* enemy)>;

public:
	/**
	 * @brief 构造函数，初始化定时器和动画系统
	 */
	Enemy()
	{
		timer_skill.setOneShot(false);
		timer_skill.setOnTimeOut([&]() { on_skill_released(this); });

		timer_sketch.setOneShot(true);
		timer_sketch.setWaitTime(0.075);
		timer_sketch.setOnTimeOut([&]() { is_show_sketch = false; });

		timer_restore_speed.setOneShot(true);
		timer_restore_speed.setOnTimeOut([&]() { speed = max_speed; });
	}

	~Enemy() = default;

	/**
	 * @brief 更新敌人状态
	 * @param delta_time 帧间隔时间
	 *
	 * 更新内容包括：
	 * - 定时器状态
	 * - 位置和移动
	 * - 动画状态
	 */
	void onUpdate(double delta_time)
	{
		timer_skill.onUpdate(delta_time);
		timer_sketch.onUpdate(delta_time);
		timer_restore_speed.onUpdate(delta_time);

		Vector2 move_distance = velocity * delta_time;
		Vector2 target_distace = target_position - position;
		position += move_distance < target_distace ? move_distance : target_distace;

		if (target_distace.approxZero()) {
			index_target++;
			refreshPositionTarget();

			direction = (target_position - position).normalize();
		}

		velocity.x = direction.x * speed * TILE_SIZE;
		velocity.y = direction.y * speed * TILE_SIZE;

		bool is_show_x_anim = abs(velocity.x) >= abs(velocity.y);

		if (is_show_sketch) {
			if (is_show_x_anim)
				anim_current = velocity.x > 0 ? &anim_right_sketch : &anim_left_sketch;
			else
				anim_current = velocity.y > 0 ? &anim_down_sketch : &anim_up_sketch;
		}
		else {
			if (is_show_x_anim)
				anim_current = velocity.x > 0 ? &anim_right : &anim_left;
			else
				anim_current = velocity.y > 0 ? &anim_down : &anim_up;
		}

		anim_current->onUpdate(delta_time);
	}

	/**
	 * @brief 渲染敌人
	 * @param renderer SDL渲染器指针
	 *
	 * 渲染内容：
	 * - 敌人精灵
	 * - 生命值条
	 */
	void onRender(SDL_Renderer* renderer)
	{
		static SDL_Point point;
		static SDL_Rect rect;
		static const int offset_y = 2;
		static const Vector2 size_hp_bar = { 40, 8 };
		static const SDL_Color color_border = { 116, 185, 124, 255 };
		static const SDL_Color color_content = { 226, 255, 194, 255 };

		point.x = (int)(position.x - size.x / 2);
		point.y = (int)(position.y - size.y / 2);
		anim_current->onRender(renderer, point);

		if (hp < max_hp) {
			rect.x = (int)(position.x - size_hp_bar.x / 2);
			rect.y = (int)(position.y - size.y / 2 - size_hp_bar.y - offset_y);
			rect.w = (int)(size_hp_bar.x * (hp / max_hp));
			rect.h = (int)size_hp_bar.y;
			SDL_SetRenderDrawColor(renderer, color_content.r, color_content.g, color_content.b, color_content.a);
			SDL_RenderFillRect(renderer, &rect);

			rect.w = (int)size_hp_bar.x;
			SDL_SetRenderDrawColor(renderer, color_border.r, color_border.g, color_border.b, color_border.a);
			SDL_RenderDrawRect(renderer, &rect);
		}
	}

	/**
	 * @brief 设置技能释放回调
	 * @param callback 技能释放时的回调函数
	 */
	void setOnSkillReleased(SkillCallback callback)
	{
		this->on_skill_released = callback;
	}

	/**
	 * @brief 增加生命值
	 * @param value 要增加的生命值数量
	 *
	 * 增加生命值但不超过最大生命值上限
	 */
	void increaseHP(double value)
	{
		hp += value;
		if (hp > max_hp)
			hp = max_hp;
	}

	/**
	 * @brief 减少生命值
	 * @param value 要减少的生命值数量
	 *
	 * 减少生命值并检查死亡状态
	 * 同时触发受击特效显示
	 */
	void decreaseHP(double value)
	{
		hp -= value;

		if (hp <= 0) {
			hp = 0;
			is_valid = false;
		}

		is_show_sketch = true;
		timer_sketch.restart();
	}

	/**
	 * @brief 降低移动速度
	 *
	 * 临时降低速度并启动速度恢复计时器
	 * 速度降低0.5个单位，1秒后恢复
	 */
	void slowDown()
	{
		speed = max_speed - 0.5;
		timer_restore_speed.setWaitTime(1.0);
		timer_restore_speed.restart();
	}

	/**
	 * @brief 设置敌人位置
	 * @param position 新的位置向量
	 */
	void setPosition(const Vector2& position)
	{
		this->position = position;
	}

	/**
	 * @brief 设置敌人移动路径
	 * @param route 新的路径对象
	 *
	 * 设置新路径并刷新目标位置
	 */
	void setRoute(const Route* route)
	{
		this->route = std::make_unique<Route>(*route);

		refreshPositionTarget();
	}

	/**
	 * @brief 使敌人失效
	 *
	 * 将敌人标记为无效状态，通常用于死亡处理
	 */
	void makeInvalid()
	{
		is_valid = false;
	}

	/**
	 * @brief 检查敌人是否可以移除
	 * @return 如果敌人无效则可以移除，返回true
	 */
	bool canRemove() const 
	{ 
		return !is_valid; 
	}

	/**
	 * @brief 获取敌人生命值
	 * @return 生命值
	 */
	double getHp() const{ return hp;}

	/**
	 * @brief 获取敌人尺寸
	 * @return 尺寸向量
	 */
	const Vector2& getSize() const{ return size;}

	/**
	 * @brief 获取敌人位置
	 * @return 位置向量
	 */
	const Vector2& getPosition() const{ return position;}

	/**
	 * @brief 获取敌人速度
	 * @return 速度向量
	 */
	const Vector2& getVelocity() const{ return velocity;}

	/**
	 * @brief 获取敌人伤害值
	 * @return 伤害值
	 */
	double getDamage() const{ return damage;}

	/**
	 * @brief 获取奖励系数
	 * @return 奖励系数
	 */
	double getRewardRatio() const{ return reward_ratio;}

	/**
	 * @brief 获取恢复范围半径
	 * @return 恢复范围（以像素为单位）
	 */
	double getRecoverRadius() const{ return TILE_SIZE * recover_range;}

	/**
	 * @brief 获取恢复强度
	 * @return 恢复强度值
	 */
	double getRecoverIntensity() const{ return recover_intensity;}

	/**
	 * @brief 获取路径完成进度
	 * @return 返回0.0到1.0之间的值，表示路径完成百分比
	 *
	 * 如果路径只有一个点，直接返回1.0
	 * 否则返回当前目标点索引占总路径点数的比例
	 */
	double getRouteProcess() const
	{
		if(route->getIndexList().size() == 1)
			return 1.0;

		return (double)index_target / (route->getIndexList().size() - 1);
	}

private:
	/**
	 * @brief 刷新目标位置
	 *
	 * 根据路径点列表和当前索引更新目标位置
	 */
	void refreshPositionTarget()
	{
		const auto& index_list = route->getIndexList();
		if (index_target < index_list.size()) {
			const SDL_Point& point = index_list[index_target];
			static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
			target_position.x = rect_tile_map.x + point.x * TILE_SIZE + TILE_SIZE / 2;
			target_position.y = rect_tile_map.y + point.y * TILE_SIZE + TILE_SIZE / 2;
		}
	}

protected:
	/**
	 * @brief 设置动画
	 * @param anim 动画对象
	 * @param texture 动画纹理
	 * @param loop 是否循环播放
	 * @param num_h 动画横向帧数
	 * @param num_v 动画纵向帧数
	 * @param index_list 动画帧索引列表
	 * @param interval 动画播放间隔
	 *
	 * 设置动画的播放间隔、贴图、帧索引列表、循环播放等属性
	 */
	void setAnimation(Animation& anim, SDL_Texture* texture, bool loop, int num_h, int num_v, const std::vector<int>& index_list, double interval)
	{
		anim.setLoop(loop);
		anim.setInterval(interval);
		anim.setFrameData(texture, num_h, num_v, index_list);
	}

protected:
	Vector2 size;								// 敌人尺寸
	Timer timer_skill;							// 技能计时器

	Animation anim_up;							// 向上移动动画
	Animation anim_down;						// 向下移动动画
	Animation anim_left;						// 向左移动动画
	Animation anim_right;						// 向右移动动画
	Animation anim_up_sketch;					// 向上移动受击动画
	Animation anim_down_sketch;					// 向下移动受击动画
	Animation anim_left_sketch;					// 向左移动受击动画
	Animation anim_right_sketch;				// 向右移动受击动画

	double hp = 0;								// 当前生命值
	double max_hp = 0;							// 最大生命值
	double speed = 0;							// 当前速度
	double max_speed = 0;						// 最大速度
	double damage = 0;							// 伤害值
	double reward_ratio = 0;					// 奖励系数
	double recover_interval = 0;				// 恢复间隔
	double recover_range = 0;					// 恢复范围
	double recover_intensity = 0;				// 恢复强度

private:
	Vector2 position;							// 当前位置
	Vector2 velocity;							// 当前速度向量				
	Vector2 direction;							// 移动方向

	bool is_valid = true;						// 敌人是否有效

	Timer timer_sketch;							// 受击特效计时器
	bool is_show_sketch = false;				// 是否显示受击特效

	Animation* anim_current = nullptr;			// 当前播放的动画

	SkillCallback on_skill_released;			// 技能释放回调

	Timer timer_restore_speed;					// 速度恢复计时器

	std::unique_ptr<Route> route;				// 移动路径
	int index_target = 0;						// 当前目标点索引
	Vector2 target_position;					// 目标位置
};
