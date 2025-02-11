#pragma once

#include "../../manager/resource_manager.hpp"
#include "../../basic/tile.hpp"

#include <SDL.h>
#include <memory>
#include <string>

/**
 * @brief 面板类
 * @details 处理面板的显示、输入和渲染，包括选择光标和交互区域
 */
class Panel
{
public:
	Panel()
	{
		tex_select_cursor.reset(ResourceManager::instance()->getTexturePool().find(ResID::Tex_UISelectCursor)->second);
	} 
	~Panel() = default;

	/**
	 * @brief 显示面板
	 */
	void show()
	{
		visible = true;
	}

	/**
	 * @brief 设置选中的瓦片索引
	 * @param index 选中的瓦片索引
	 */
	void setIndexTile(const SDL_Point& index)
	{
		index_tile_selected = index;
	}

	/**
	 * @brief 设置中心位置
	 * @param position 中心位置
	 */
	void setCenterPosition(const SDL_Point& position)
	{
		center_position = position;
	}

	/**
	 * @brief 处理输入事件
	 * @param event SDL事件
	 * @details 处理鼠标点击事件，根据鼠标位置判断点击的区域
	 * 点击顶部区域：触发onClickTopArea()
	 * 点击左侧区域：触发onClickLeftArea()
	 * 点击右侧区域：触发onClickRightArea()
	 * 点击其他区域：隐藏面板
	 */
	void onInput(const SDL_Event& event)
	{
		if (!visible) return;

		switch (event.type)
		{
		case SDL_MOUSEMOTION:
		{
			SDL_Point pos_cursor = { event.motion.x, event.motion.y };
			SDL_Rect rect_target = { 0, 0, size_button, size_button };

			rect_target.x = center_position.x - width / 2 + offset_top.x;
			rect_target.y = center_position.y - height / 2 + offset_top.y;
			if (SDL_PointInRect(&pos_cursor, &rect_target))
			{
				hovered_target = HoveredTarget::TOP;
				return;
			}
			
			rect_target.x = center_position.x - width / 2 + offset_left.x;
			rect_target.y = center_position.y - height / 2 + offset_left.y;
			if (SDL_PointInRect(&pos_cursor, &rect_target))
			{
				hovered_target = HoveredTarget::LEFT;
				return;
			}

			rect_target.x = center_position.x - width / 2 + offset_right.x;
			rect_target.y = center_position.y - height / 2 + offset_right.y;
			if (SDL_PointInRect(&pos_cursor, &rect_target))
			{
				hovered_target = HoveredTarget::RIGHT;
				return;
			}

			hovered_target = HoveredTarget::NONE;
		}
			break;
		case SDL_MOUSEBUTTONUP:
		{
			switch (hovered_target)
			{
			case Panel::HoveredTarget::TOP:
				onClickTopArea();
				break;
			case Panel::HoveredTarget::LEFT:
				onClickLeftArea();
				break;
			case Panel::HoveredTarget::RIGHT:
				onClickRightArea();
				break;
			}

			visible = false;
		}
			break;
		default:
			break;
		}
	}

	/**
	 * @brief 更新面板状态
	 * @param renderer SDL渲染器指针
	 * @details 更新面板状态，包括更新文本内容
	 */
	virtual void onUpdate(SDL_Renderer* renderer)
	{
		static auto* font = ResourceManager::instance()->getFontPool().find(ResID::Font_Main)->second;

		if (hovered_target == HoveredTarget::NONE) return;

		int value = 0;
		switch (hovered_target)
		{
		case Panel::HoveredTarget::TOP:
			value = value_top;
			break;
		case Panel::HoveredTarget::LEFT:
			value = value_left;
			break;
		case Panel::HoveredTarget::RIGHT:
			value = value_right;
			break;
		}

		SDL_DestroyTexture(tex_text_background.release());
		SDL_DestroyTexture(tex_text_foreground.release());

		std::string str_value = value < 0 ? "MAX" : std::to_string(value);
		std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> surface_text_background(TTF_RenderText_Blended(font, str_value.c_str(), color_text_background), SDL_FreeSurface);
		std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> surface_text_foreground(TTF_RenderText_Blended(font, str_value.c_str(), color_text_foreground), SDL_FreeSurface);

		width_text = surface_text_background->w, height_text = surface_text_background->h;

		tex_text_background.reset(SDL_CreateTextureFromSurface(renderer, surface_text_background.get()));
		tex_text_foreground.reset(SDL_CreateTextureFromSurface(renderer, surface_text_foreground.get()));
	}

	/**
	 * @brief 渲染面板内容
	 * @param renderer SDL渲染器指针
	 * @details 渲染面板内容，包括渲染面板背景、渲染面板边框、渲染文本
	 */
	virtual void onRender(SDL_Renderer* renderer)
	{
		if (!visible) return;

		SDL_Rect rect_dst_cursor =
		{
			center_position.x - TILE_SIZE / 2,
			center_position.y - TILE_SIZE / 2,
			TILE_SIZE,
			TILE_SIZE
		};
		SDL_RenderCopy(renderer, tex_select_cursor.get(), nullptr, &rect_dst_cursor);

		SDL_Rect rect_dst_panel =
		{
			center_position.x - width / 2,
			center_position.y - height / 2,
			width,
			height
		};

		std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> tex_panel{ nullptr, SDL_DestroyTexture };
		switch (hovered_target)
		{
		case Panel::HoveredTarget::NONE:
			tex_panel = std::move(tex_idle);
			break;
		case Panel::HoveredTarget::TOP:
			tex_panel = std::move(tex_hovered_top);
			break;
		case Panel::HoveredTarget::LEFT:
			tex_panel = std::move(tex_hovered_left);
			break;
		case Panel::HoveredTarget::RIGHT:
			tex_panel = std::move(tex_hovered_right);
			break;
		}

		SDL_RenderCopy(renderer, tex_panel.get(), nullptr, &rect_dst_panel);

		if (hovered_target != HoveredTarget::NONE) return;

		SDL_Rect rect_dst_text;

		rect_dst_text.x = center_position.x - width_text / 2 + offset_shadow.x;
		rect_dst_text.y = center_position.y + height_text / 2 + offset_shadow.y;
		rect_dst_text.w = width_text, rect_dst_text.h = height_text;
		SDL_RenderCopy(renderer, tex_text_background.get(), nullptr, &rect_dst_text);

		rect_dst_text.x -= offset_shadow.x;
		rect_dst_text.y -= offset_shadow.y;
		SDL_RenderCopy(renderer, tex_text_foreground.get(), nullptr, &rect_dst_text);
	}

protected:
	/**
	 * @brief 悬停目标枚举
	 */
	enum class HoveredTarget
	{
		NONE,  // 无
		TOP,   // 顶部
		LEFT,  // 左侧
		RIGHT  // 右侧
	};

protected:
	bool visible = false;					   // 可见性

	SDL_Point index_tile_selected = { 0, 0 };  // 选中的瓦片索引
	SDL_Point center_position = { 0, 0 };	   // 中心位置

	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> tex_idle{ nullptr, SDL_DestroyTexture };		  // 空闲纹理
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> tex_hovered_top{ nullptr, SDL_DestroyTexture };   // 悬停在顶部纹理
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> tex_hovered_left{ nullptr, SDL_DestroyTexture };  // 悬停在左侧纹理
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> tex_hovered_right{ nullptr, SDL_DestroyTexture }; // 悬停在右侧纹理
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> tex_select_cursor{ nullptr, SDL_DestroyTexture }; // 选择光标纹理

	int value_top = 0, value_left = 0, value_right = 0;  // 各个方向的值

	HoveredTarget hovered_target = HoveredTarget::NONE;  // 当前悬停目标

protected:
	/**
	 * @brief 点击顶部区域的虚拟函数
	 */
	virtual void onClickTopArea() = 0;

	/**
	 * @brief 点击左侧区域的虚拟函数
	 */
	virtual void onClickLeftArea() = 0;

	/**
	 * @brief 点击右侧区域的虚拟函数
	 */
	virtual void onClickRightArea() = 0;

private:
	const int size_button = 48;           // 按钮大小
	const int width = 144, height = 144;  // 面板宽高

	const SDL_Point offset_top = { 48, 6 };     // 顶部偏移
	const SDL_Point offset_left = { 8, 80 };    // 左侧偏移
	const SDL_Point offset_right = { 90, 80 };  // 右侧偏移
	const SDL_Point offset_shadow = { 3, 3 };   // 阴影偏移
	const SDL_Color color_text_background = { 175, 175, 175, 255 };  // 文本背景颜色
	const SDL_Color color_text_foreground = { 255, 255, 255, 255 };  // 文本前景颜色

	int width_text = 0;   // 文本宽度
	int height_text = 0;  // 文本高度

	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> tex_text_background{ nullptr, SDL_DestroyTexture };  // 文本背景纹理
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> tex_text_foreground{ nullptr, SDL_DestroyTexture };  // 文本前景纹理
};