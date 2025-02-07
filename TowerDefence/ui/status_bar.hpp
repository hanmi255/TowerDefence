#pragma once

#include "../manager/home_manager.hpp"
#include "../manager/coin_manager.hpp"
#include "../manager/resource_manager.hpp"

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <memory>
#include <string>

/**
 * @brief 状态栏类
 * @details 显示玩家的生命值、金币数量、魔法值等信息，并渲染相关图标和文本
 */
class StatusBar
{
public:
    StatusBar() = default;  
    ~StatusBar() = default; 

    /**
     * @brief 设置状态栏的位置
     * @param x X坐标
     * @param y Y坐标
     * @details 设置状态栏的显示位置
     */
    void setPosition(int x, int y)
    {
        position.x = x, position.y = y;
    }

    /**
     * @brief 更新状态栏显示内容
     * @param renderer SDL渲染器指针
     * @details 更新金币数量文本，并刷新相关纹理
     */
    void onUpdate(SDL_Renderer* renderer)
    {
        static auto* font = ResourceManager::instance()->getFontPool().find(ResID::Font_Main)->second;

        // 保存当前文本内容，用于检测是否发生变化
        static std::string last_str_val = "";
        std::string str_val = std::to_string((int)CoinManager::instance()->getCurrentCoinNum());

        // 如果文本没有变化，则直接跳过更新
        if (str_val == last_str_val)
            return;

        // 更新文本内容
        last_str_val = str_val;

        // 销毁旧的纹理
        SDL_DestroyTexture(tex_text_background.get());
        tex_text_background = nullptr;
        SDL_DestroyTexture(tex_text_foreground.get());
        tex_text_foreground = nullptr;

        // 创建新的表面和纹理
        std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> surface_text_background(
            TTF_RenderText_Blended(font, str_val.c_str(), color_text_background), SDL_FreeSurface);
        std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> surface_text_foreground(
            TTF_RenderText_Blended(font, str_val.c_str(), color_text_foreground), SDL_FreeSurface);

        // 设置文本的宽高
        width_text = surface_text_background->w;
        height_text = surface_text_background->h;

        // 创建新的纹理
        tex_text_background = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>(
            SDL_CreateTextureFromSurface(renderer, surface_text_background.get()), SDL_DestroyTexture);
        tex_text_foreground = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>(
            SDL_CreateTextureFromSurface(renderer, surface_text_foreground.get()), SDL_DestroyTexture);
    }

    /**
     * @brief 渲染状态栏内容
     * @param renderer SDL渲染器指针
     * @details 渲染房屋头像、生命值、金币数量、魔法值条及玩家头像
     */
    void onRender(SDL_Renderer* renderer)
    {
        static SDL_Rect rect_dst;
        static const auto& tex_pool = ResourceManager::instance()->getTexturePool();
        static auto* tex_coin = tex_pool.find(ResID::Tex_UICoin)->second;
        static auto* tex_heart = tex_pool.find(ResID::Tex_UIHeart)->second;
        static auto* tex_home_avatar = tex_pool.find(ResID::Tex_UIHomeAvatar)->second;
        static auto* tex_player_avatar = tex_pool.find(ResID::Tex_UIPlayerAvatar)->second;

        // 绘制房屋头像
        rect_dst.x = position.x, rect_dst.y = position.y;
        rect_dst.w = 78, rect_dst.h = 78;
        SDL_RenderCopy(renderer, tex_home_avatar, nullptr, &rect_dst);

        // 绘制生命值
        for (int i = 0; i < (int)HomeManager::instance()->getCurrentHPNum(); i++) {
            rect_dst.x = position.x + 78 + 15 + i * (32 + 2);
            rect_dst.y = position.y;
            rect_dst.w = 32, rect_dst.h = 32;
            SDL_RenderCopy(renderer, tex_heart, nullptr, &rect_dst);
        }

        // 绘制金币数量
        rect_dst.x = position.x + 78 + 15;
        rect_dst.y = position.y + 78 - 32;
        rect_dst.w = 32, rect_dst.h = 32;
        SDL_RenderCopy(renderer, tex_coin, nullptr, &rect_dst);

        // 绘制文本背景
        rect_dst.x += 32 + 10 + offset_shadow.x;
        rect_dst.y = rect_dst.y + (32 - height_text) / 2 + offset_shadow.y;
        rect_dst.w = width_text, rect_dst.h = height_text;
        SDL_RenderCopy(renderer, tex_text_background.get(), nullptr, &rect_dst);

        // 绘制文本前景
        rect_dst.x -= offset_shadow.x;
        rect_dst.y -= offset_shadow.y;
        SDL_RenderCopy(renderer, tex_text_foreground.get(), nullptr, &rect_dst);

        // 绘制玩家头像
        rect_dst.x = position.x + (78 - 65) / 2;
        rect_dst.y = position.y + 78 + 5;
        rect_dst.w = 65, rect_dst.h = 65;
        SDL_RenderCopy(renderer, tex_player_avatar, nullptr, &rect_dst);

        // 绘制魔法值条背景
        rect_dst.x = position.x + 78 + 15;
        rect_dst.y += 10;
        roundedBoxRGBA(renderer, rect_dst.x, rect_dst.y, rect_dst.x + width_mp_bar, rect_dst.y + height_mp_bar, 4, color_mp_bar_background.r, color_mp_bar_background.g, color_mp_bar_background.b, color_mp_bar_background.a);

        // 绘制魔法值条前景
        rect_dst.x += width_border_mp_bar;
        rect_dst.y += width_border_mp_bar;
        rect_dst.w = width_mp_bar - 2 * width_border_mp_bar;
        rect_dst.h = height_mp_bar - 2 * width_border_mp_bar;
        roundedBoxRGBA(renderer, rect_dst.x, rect_dst.y, rect_dst.x + rect_dst.w, rect_dst.y + rect_dst.h, 2, color_mp_bar_foreground.r, color_mp_bar_foreground.g, color_mp_bar_foreground.b, color_mp_bar_foreground.a);
    }

private:
    const int size_heart = 32;                                                                                      // 生命值图标大小
    const int width_mp_bar = 200;                                                                                   // 魔法值条宽度
    const int height_mp_bar = 20;                                                                                   // 魔法值条高度
    const int width_border_mp_bar = 4;                                                                              // 魔法值条边框宽度
    const SDL_Point offset_shadow = { 2, 2 };                                                                       // 文本阴影偏移量
    const SDL_Color color_text_background = { 175, 175, 175, 175 };                                                 // 文本背景色
    const SDL_Color color_text_foreground = { 255, 255, 255, 255 };                                                 // 文本前景色
    const SDL_Color color_mp_bar_background = { 48, 40, 51, 255 };                                                  // 魔法值条背景色
    const SDL_Color color_mp_bar_foreground = { 144, 121, 173, 255 };                                               // 魔法值条前景色

private:
    SDL_Point position = { 0, 0 };                                                                                  // 状态栏位置
    int width_text = 0, height_text = 0;                                                                            // 文本宽高
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> tex_text_background{ nullptr, SDL_DestroyTexture }; // 文本背景纹理
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> tex_text_foreground{ nullptr, SDL_DestroyTexture }; // 文本前景纹理
};
