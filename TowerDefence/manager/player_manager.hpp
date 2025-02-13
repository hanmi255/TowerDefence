#pragma once

#include "manager.hpp"
#include "coin_manager.hpp"
#include "enemy_manager.hpp"
#include "resource_manager.hpp"
#include "../basic/tile.hpp"
#include "../basic/facing.hpp"
#include "../util/animation.hpp"
#include "../util/vector2.hpp"

#include <SDL.h>
#include <memory>
#include <vector>
#include <functional>
#include <algorithm>

/**
* @brief 玩家管理器
* @details 管理玩家的生命值、位置、动画、状态等信息
*/
class PlayerManager : public Manager<PlayerManager>
{
	friend class Manager<PlayerManager>;

public:
	/**
	* @brief 处理玩家输入
	* @param event 事件
	* @details 处理玩家的输入，包括移动、攻击等
	*/
	void onInput(const SDL_Event& event)
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_a:
				is_move_left = true;
				break;
			case SDLK_d:
				is_move_right = true;
				break;
			case SDLK_w:
				is_move_up = true;
				break;
			case SDLK_s:
				is_move_down = true;
				break;
			case SDLK_j:
				onReleaseFlash();
				break;
			case SDLK_k:
				onReleaseImpact();
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_a:
				is_move_left = false;
				break;
			case SDLK_d:
				is_move_right = false;
				break;
			case SDLK_w:
				is_move_up = false;
				break;
			case SDLK_s:
				is_move_down = false;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	/**
	* @brief 更新玩家信息
	* @param delta_time 时间间隔
	* @details 主要更新玩家的位置、动画、状态等信息
	*/
	void onUpdate(double delta_time)
	{
		timer_auto_increase_mp.onUpdate(delta_time);
		timer_release_flash_cd.onUpdate(delta_time);

		auto direction = Vector2(is_move_right - is_move_left, is_move_down - is_move_up).normalize();
		velocity = direction * speed * TILE_SIZE;

		if (!is_releasing_flash && !is_releasing_impact) {
			pos_player += velocity * delta_time;

			const auto& rect_map = ConfigManager::instance()->rect_tile_map;
			clampPosition(pos_player, rect_map);

			if (velocity.y != 0) {
				facing = (velocity.y > 0) ? Facing::DOWN : Facing::UP;
			}
			if (velocity.x != 0) {
				facing = (velocity.x > 0) ? Facing::RIGHT : Facing::LEFT;
			}

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
		else {
			switch (facing)
			{
			case Facing::UP:
				anim_current = &anim_attack_up;
				break;
			case Facing::DOWN:
				anim_current = &anim_attack_down;
				break;
			case Facing::LEFT:
				anim_current = &anim_attack_left;
				break;
			case Facing::RIGHT:
				anim_current = &anim_attack_right;
				break;
			}
		}

		anim_current->onUpdate(delta_time);

		if (is_releasing_flash) {
			anim_effect_flash_current->onUpdate(delta_time);

			auto& enemy_list = EnemyManager::instance()->getEnemyList();
			for (auto* enemy : enemy_list) {
				if(enemy->canRemove()) continue;

				const auto& pos_enemy = enemy->getPosition();
				if (pos_enemy.x >= rect_hitbox_flash.x
					&& pos_enemy.x <= rect_hitbox_flash.x + rect_hitbox_flash.w
					&& pos_enemy.y >= rect_hitbox_flash.y
					&& pos_enemy.y <= rect_hitbox_flash.y + rect_hitbox_flash.h) {
					enemy->decreaseHP(ConfigManager::instance()->player_template.normal_attack_damage * delta_time);
					mp += 0.05;
				}
			}
		}
		else if (is_releasing_impact) {
			anim_effect_impact_current->onUpdate(delta_time);

			auto& enemy_list = EnemyManager::instance()->getEnemyList();
			for (auto* enemy : enemy_list) {
				if (enemy->canRemove()) continue;

				const auto& pos_enemy = enemy->getPosition();
				if (pos_enemy.x >= rect_hitbox_impact.x
					&& pos_enemy.x <= rect_hitbox_impact.x + rect_hitbox_impact.w
					&& pos_enemy.y >= rect_hitbox_impact.y
					&& pos_enemy.y <= rect_hitbox_impact.y + rect_hitbox_impact.h) {
					enemy->decreaseHP(ConfigManager::instance()->player_template.skill_damage * delta_time);
					enemy->slowDown();
				}
			}
		}

		auto& coin_prop_list = CoinManager::instance()->getCoinPropList();
		static const auto& sound_pool = ResourceManager::instance()->getSoundPool();
		for (auto* coin_prop : coin_prop_list) {
			if (coin_prop->canRemove()) continue;

			const auto& pos_coin = coin_prop->getPosition();
			if (pos_coin.x >= pos_player.x - size.x / 2
				&& pos_coin.x <= pos_player.x + size.x / 2
				&& pos_coin.y >= pos_player.y - size.y / 2
				&& pos_coin.y <= pos_player.y + size.y / 2) {
				coin_prop->makeInvalid();
				CoinManager::instance()->increaseCoin(10);

				Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_Coin)->second, 0);
			}
		}
	}

	/**
	* @brief 渲染玩家
	* @param renderer 渲染器
	*/
	void onRender(SDL_Renderer* renderer)
	{
		static SDL_Point point;
		point.x = (int)(pos_player.x - size.x / 2);
		point.y = (int)(pos_player.y - size.y / 2);
		anim_current->onRender(renderer, point);

		if (is_releasing_flash) {
			point.x = rect_hitbox_flash.x;
			point.y = rect_hitbox_flash.y;
			anim_effect_flash_current->onRender(renderer, point);
		}
		else if (is_releasing_impact) {
			point.x = rect_hitbox_impact.x;
			point.y = rect_hitbox_impact.y;
			anim_effect_impact_current->onRender(renderer, point);
		}
	}

	/**
	 * @brief 获取当前MP值
	 */
	double getCurrentMP() const { return mp; }

protected:
	PlayerManager()
	{
		timer_auto_increase_mp.setOneShot(false);
		timer_auto_increase_mp.setWaitTime(0.1);
		timer_auto_increase_mp.setOnTimeOut(
			[&]() {
				double interval = ConfigManager::instance()->player_template.skill_interval;
				mp = std::min(mp + 100 / (interval / 0.1), 100.0);
			});

		timer_release_flash_cd.setOneShot(true);
		timer_release_flash_cd.setWaitTime(ConfigManager::instance()->player_template.skill_interval);
		timer_release_flash_cd.setOnTimeOut(
			[&]() {
				can_release_flash = true;
			});

		static const auto& tex_pool = ResourceManager::instance()->getTexturePool();

		static auto* tex_player = tex_pool.find(ResID::Tex_Player)->second;

		// 定义空闲状态四个方向的动画帧索引
		static const std::vector<int> idx_list_idle_up = { 4, 5, 6, 7 };
		static const std::vector<int> idx_list_idle_down = { 0, 1, 2, 3 };
		static const std::vector<int> idx_list_idle_left = { 8, 9, 10, 11 };
		static const std::vector<int> idx_list_idle_right = { 12, 13, 14, 15 };

		// 定义攻击状态四个方向的动画帧索引
		static const std::vector<int> idx_list_attack_up = { 20, 21 };
		static const std::vector<int> idx_list_attack_down = { 16, 17 };
		static const std::vector<int> idx_list_attack_left = { 24, 25 };
		static const std::vector<int> idx_list_attack_right = { 28, 29 };

		// 定义闪电特效四个方向的动画帧索引
		static const std::vector<int> idx_list_effect_flash_up = { 0, 1, 2, 3, 4 };
		static const std::vector<int> idx_list_effect_flash_down = { 4, 3, 2, 1, 0 };
		static const std::vector<int> idx_list_effect_flash_left = { 4, 3, 2, 1, 0 };
		static const std::vector<int> idx_list_effect_flash_right = { 0, 1, 2, 3, 4 };

		// 定义冲击波特效四个方向的动画帧索引
		static const std::vector<int> idx_list_effect_impact_up = { 0, 1, 2, 3, 4 };
		static const std::vector<int> idx_list_effect_impact_down = { 4, 3, 2, 1, 0 };
		static const std::vector<int> idx_list_effect_impact_left = { 4, 3, 2, 1, 0 };
		static const std::vector<int> idx_list_effect_impact_right = { 0, 1, 2, 3, 4 };

		// 设置动画
		setAnimation(anim_idle_up, tex_player, true, 4, 8, idx_list_idle_up, 0.1);
		setAnimation(anim_idle_down, tex_player, true, 4, 8, idx_list_idle_down, 0.1);
		setAnimation(anim_idle_left, tex_player, true, 4, 8, idx_list_idle_left, 0.1);
		setAnimation(anim_idle_right, tex_player, true, 4, 8, idx_list_idle_right, 0.1);

		setAnimation(anim_attack_up, tex_player, true, 4, 8, idx_list_attack_up, 0.1);
		setAnimation(anim_attack_down, tex_player, true, 4, 8, idx_list_attack_down, 0.1);
		setAnimation(anim_attack_left, tex_player, true, 4, 8, idx_list_attack_left, 0.1);
		setAnimation(anim_attack_right, tex_player, true, 4, 8, idx_list_attack_right, 0.1);

		setAnimation(anim_effect_flash_up, tex_pool.find(ResID::Tex_EffectFlash_Up)->second, false, 5, 1, idx_list_effect_flash_up, 0.1, [&]() { is_releasing_flash = false; });
		setAnimation(anim_effect_flash_down, tex_pool.find(ResID::Tex_EffectFlash_Down)->second, false, 5, 1, idx_list_effect_flash_down, 0.1, [&]() { is_releasing_flash = false; });
		setAnimation(anim_effect_flash_left, tex_pool.find(ResID::Tex_EffectFlash_Left)->second, false, 1, 5, idx_list_effect_flash_left, 0.1, [&]() { is_releasing_flash = false; });
		setAnimation(anim_effect_flash_right, tex_pool.find(ResID::Tex_EffectFlash_Right)->second, false, 1, 5, idx_list_effect_flash_right, 0.1, [&]() { is_releasing_flash = false; });

		setAnimation(anim_effect_impact_up, tex_pool.find(ResID::Tex_EffectImpact_Up)->second, false, 5, 1, idx_list_effect_impact_up, 0.1, [&]() { is_releasing_impact = false; });
		setAnimation(anim_effect_impact_down, tex_pool.find(ResID::Tex_EffectImpact_Down)->second, false, 5, 1, idx_list_effect_impact_down, 0.1, [&]() { is_releasing_impact = false; });
		setAnimation(anim_effect_impact_left, tex_pool.find(ResID::Tex_EffectImpact_Left)->second, false, 1, 5, idx_list_effect_impact_left, 0.1, [&]() { is_releasing_impact = false; });
		setAnimation(anim_effect_impact_right, tex_pool.find(ResID::Tex_EffectImpact_Right)->second, false, 1, 5, idx_list_effect_impact_right, 0.1, [&]() { is_releasing_impact = false; });

		const auto& rect_map = ConfigManager::instance()->rect_tile_map;
		pos_player.x = rect_map.x + rect_map.w / static_cast<double>(2);
		pos_player.y = rect_map.y + rect_map.h / static_cast<double>(2);

		speed = ConfigManager::instance()->player_template.speed;

		size.x = 96;
		size.y = 96;
	}
	~PlayerManager() = default;

private:
	Vector2 size;									  // 玩家大小
	Vector2 pos_player;								  // 玩家位置
	Vector2 velocity;								  // 玩家速度

	SDL_Rect rect_hitbox_flash = { 0 };				  // 闪电特效的碰撞矩形
	SDL_Rect rect_hitbox_impact = { 0 };			  // 冲击波特效的碰撞矩形

	double mp = 100;								  // 当前魔法值
	double speed = 0;								  // 玩家移动速度

	bool can_release_flash = true;					  // 是否可以释放闪电特效
	bool is_releasing_flash = false;				  // 是否正在释放闪电特效
	bool is_releasing_impact = false;				  // 是否正在释放冲击波特效

	bool is_move_up = false;						  // 是否向上移动
	bool is_move_down = false;						  // 是否向下移动
	bool is_move_left = false;						  // 是否向左移动
	bool is_move_right = false;						  // 是否向右移动

	Animation anim_idle_up;							  // 空闲状态向上动画
	Animation anim_idle_down;						  // 空闲状态向下动画
	Animation anim_idle_left;						  // 空闲状态向左动画
	Animation anim_idle_right;						  // 空闲状态向右动画

	Animation anim_attack_up;						  // 攻击状态向上动画
	Animation anim_attack_down;						  // 攻击状态向下动画
	Animation anim_attack_left;						  // 攻击状态向左动画
	Animation anim_attack_right;					  // 攻击状态向右动画
	Animation* anim_current = &anim_idle_right;		  // 当前动画

	Animation anim_effect_flash_up;					  // 闪电特效向上动画
	Animation anim_effect_flash_down;				  // 闪电特效向下动画
	Animation anim_effect_flash_left;				  // 闪电特效向左动画
	Animation anim_effect_flash_right;				  // 闪电特效向右动画
	Animation* anim_effect_flash_current = nullptr;   // 当前闪电特效动画

	Animation anim_effect_impact_up;				  // 冲击波特效向上动画
	Animation anim_effect_impact_down;				  // 冲击波特效向下动画
	Animation anim_effect_impact_left;				  // 冲击波特效向左动画
	Animation anim_effect_impact_right;				  // 冲击波特效向右动画
	Animation* anim_effect_impact_current = nullptr;  // 当前冲击波特效动画

	Timer timer_release_flash_cd;					  // 闪电特效冷却计时器
	Timer timer_auto_increase_mp;					  // 自动增加魔法值计时器

	Facing facing = Facing::LEFT;					  // 当前朝向

private:
	/**
	 * @brief 释放闪电特效
	 * @note 闪电特效会造成一定范围内的敌人受到伤害
	 */
	void onReleaseFlash()
	{
		if (!can_release_flash || is_releasing_flash) return;

		switch (facing)
		{
		case Facing::UP:
			anim_effect_flash_current = &anim_effect_flash_up;
			rect_hitbox_flash.x = (int)(pos_player.x - static_cast<double>(68) / 2);
			rect_hitbox_flash.y = (int)(pos_player.y - size.x / 2 - 300);
			rect_hitbox_flash.w = 68, rect_hitbox_flash.h = 300;
			break;
		case Facing::DOWN:
			anim_effect_flash_current = &anim_effect_flash_down;
			rect_hitbox_flash.x = (int)(pos_player.x - static_cast<double>(68) / 2);
			rect_hitbox_flash.y = (int)(pos_player.y + size.x / 2);
			rect_hitbox_flash.w = 68, rect_hitbox_flash.h = 300;
			break;
		case Facing::LEFT:
			anim_effect_flash_current = &anim_effect_flash_left;
			rect_hitbox_flash.x = (int)(pos_player.x - size.x / 2 - 300);
			rect_hitbox_flash.y = (int)(pos_player.y - static_cast<double>(68) / 2);
			rect_hitbox_flash.w = 300, rect_hitbox_flash.h = 68;
			break;
		case Facing::RIGHT:
			anim_effect_flash_current = &anim_effect_flash_right;
			rect_hitbox_flash.x = (int)(pos_player.x + size.x / 2);
			rect_hitbox_flash.y = (int)(pos_player.y - static_cast<double>(68) / 2);
			rect_hitbox_flash.w = 300, rect_hitbox_flash.h = 68;
			break;
		}

		is_releasing_flash = true;
		anim_effect_flash_current->reset();
		timer_release_flash_cd.restart();

		Mix_PlayChannel(-1, ResourceManager::instance()->getSoundPool().find(ResID::Sound_Flash)->second, 0);
	}

	/**
	 * @brief 释放冲击波特效
	 * @note 冲击波特效需要消耗MP
	 * @note 冲击波特效会造成一定伤害，并使敌人减速
	 */
	void onReleaseImpact()
	{
		if(mp < 100 || is_releasing_impact) return;

		switch (facing)
		{
		case Facing::UP:
			anim_effect_impact_current = &anim_effect_impact_up;
			rect_hitbox_impact.x = (int)(pos_player.x - static_cast<double>(140) / 2);
			rect_hitbox_impact.y = (int)(pos_player.y - size.x / 2 - 60);
			rect_hitbox_impact.w = 140, rect_hitbox_impact.h = 60;
			break;
		case Facing::DOWN:
			anim_effect_impact_current = &anim_effect_impact_down;
			rect_hitbox_impact.x = (int)(pos_player.x - static_cast<double>(140) / 2);
			rect_hitbox_impact.y = (int)(pos_player.y + size.x / 2);
			rect_hitbox_impact.w = 140, rect_hitbox_impact.h = 60;
			break;
		case Facing::LEFT:
			anim_effect_impact_current = &anim_effect_impact_left;
			rect_hitbox_impact.x = (int)(pos_player.x - size.x / 2 - 60);
			rect_hitbox_impact.y = (int)(pos_player.y - static_cast<double>(140) / 2);
			rect_hitbox_impact.w = 60, rect_hitbox_impact.h = 140;
			break;
		case Facing::RIGHT:
			anim_effect_impact_current = &anim_effect_impact_right;
			rect_hitbox_impact.x = (int)(pos_player.x + size.x / 2);
			rect_hitbox_impact.y = (int)(pos_player.y - static_cast<double>(140) / 2);
			rect_hitbox_impact.w = 60, rect_hitbox_impact.h = 140;
			break;
		}

		mp = 0;
		is_releasing_impact = true;
		anim_effect_impact_current->reset();

		Mix_PlayChannel(-1, ResourceManager::instance()->getSoundPool().find(ResID::Sound_Impact)->second, 0);
	}

	/**
	 * @brief 设置动画
	 * @param anim 动画对象
	 * @param texture 动画纹理
	 * @param loop 是否循环播放
	 * @param num_h 动画横向帧数
	 * @param num_v 动画纵向帧数
	 * @param index_list 动画帧索引列表
	 * @param interval 帧间隔
	 * @param call_back 动画结束回调函数
	 */
	void setAnimation(Animation& anim, SDL_Texture* texture, bool loop, int num_h, int num_v, const std::vector<int>& index_list, double interval, std::function<void()> call_back = nullptr)
	{
		anim.setLoop(loop);
		anim.setInterval(interval);
		anim.setFrameData(texture, num_h, num_v, index_list);
		if (call_back)
			anim.setOnFinished(call_back);
	}

	/**
	 * @brief 限制位置在矩形范围内
	 * @param pos 位置
	 * @param rect 矩形范围
	 */
	void clampPosition(Vector2& pos, const SDL_Rect& rect)
	{
		pos.x = std::clamp(pos.x, static_cast<double>(rect.x), static_cast<double>(rect.x + rect.w));
		pos.y = std::clamp(pos.y, static_cast<double>(rect.y), static_cast<double>(rect.y + rect.h));
	}
};