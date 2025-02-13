#pragma once

#include "../util/vector2.hpp"
#include "../util/timer.hpp"
#include "../manager/config_manager.hpp"
#include "../manager/resource_manager.hpp"

#include <SDL.h>
#include <memory>

/**
 * @brief 显示游戏结束画面
 * 
 * 显示游戏结束画面，包括游戏胜利或失败的文字，以及游戏结束的进度条。
 */
class Banner
{
public:
	Banner()
	{
		size_foreground = { 646, 215 };
		size_background = { 1282, 209 };

		timer_display.setOneShot(true);
		timer_display.setWaitTime(5.0);
		timer_display.setOnTimeOut(
			[&]() {
				is_end_display = true;
			});
	}

	~Banner() = default;

	/**
	 * @brief 设置显示位置
	 * @param pos 显示位置
	 */
	void setCenterPosition(const Vector2& pos)
	{
		pos_center = pos;
	}

	/**
	 * @brief 更新状态
	 * @param delta_time 时间间隔
	 */
	void onUpdate(double delta_time)
	{
		timer_display.onUpdate(delta_time);

		const auto& tex_pool = ResourceManager::instance()->getTexturePool();
		const auto* config = ConfigManager::instance();

		tex_foreground =
			tex_pool.find(config->is_game_win ? ResID::Tex_UIWinText : ResID::Tex_UILossText)->second;
		tex_background = tex_pool.find(ResID::Tex_UIGameOverBar)->second;
	}
	
	/**
	 * @brief 渲染
	 * @param renderer 渲染器
	 */
	void onRender(SDL_Renderer* renderer)
	{
		static SDL_Rect rect_dst;
		rect_dst.x = (int)(pos_center.x - size_background.x / 2);
		rect_dst.y = (int)(pos_center.y - size_background.y / 2);
		rect_dst.w = (int)size_background.x;
		rect_dst.h = (int)size_background.y;

		SDL_RenderCopy(renderer, tex_background, nullptr, &rect_dst);

		rect_dst.x = (int)(pos_center.x - size_foreground.x / 2);
		rect_dst.y = (int)(pos_center.y - size_foreground.y / 2);
		rect_dst.w = (int)size_foreground.x;
		rect_dst.h = (int)size_foreground.y;

		SDL_RenderCopy(renderer, tex_foreground, nullptr, &rect_dst);
	}

	/**
	 * @brief 检查是否显示结束画面
	 * @return true 显示结束画面，false 继续游戏
	 */
	bool checkEndDisplay() const { return is_end_display; }

private:
	Vector2 pos_center;															 // 横幅中心位置
	Vector2 size_foreground;													 // 前景图像的尺寸
	Vector2 size_background;													 // 背景图像的尺寸

	SDL_Texture* tex_foreground{ nullptr };										 // 前景纹理
	SDL_Texture* tex_background{ nullptr };										 // 背景纹理

	Timer timer_display;														 // 定时器
	bool is_end_display = false;												 // 是否结束显示的标志
};
