#pragma once

#include "Timer.hpp"

#include <SDL.h>
#include <functional>
#include <vector>

/**
 * @brief 2D精灵动画控制类
 *
 * 用于管理和播放基于精灵图(Sprite Sheet)的2D动画
 * 支持循环/非循环播放、帧间隔控制、播放完成回调等功能
 */
class Animation
{
public:
    /// 动画播放完成的回调函数类型
    using PlayCallback = std::function<void()>;
public:
    /**
     * @brief 构造函数
     *
     * 初始化计时器并设置帧更新逻辑
     */
    Animation()
    {
        m_timer.setOneShot(false);
        m_timer.setOnTimeOut(
            [&]()
            {
                index_frame++;
                if (index_frame >= rect_src_list.size()) {
                    index_frame = is_loop ? 0 : rect_src_list.size() - 1;
                    if (!is_loop && on_finished) {
                        on_finished();
                    }
                }
            }
        );
    }
    ~Animation() = default;

    /**
     * @brief 重置动画状态
     *
     * 重启计时器并将帧索引设置为0
     */
    void reset()
    {
        m_timer.restart();
        index_frame = 0;
    }

    /**
     * @brief 设置动画帧数据
     *
     * @param texture 精灵图纹理
     * @param num_h 水平方向的帧数
     * @param num_v 垂直方向的帧数
     * @param index_list 动画帧序列索引列表
     */
    void setFrameData(SDL_Texture* texture, int num_h, int num_v, const std::vector<int>& index_list)
    {
        int tex_width, tex_height;

        this->texture = texture;
        SDL_QueryTexture(texture, nullptr, nullptr, &tex_width, &tex_height);
        width_frame = tex_width / num_h, height_frame = tex_height / num_v;

        rect_src_list.resize(index_list.size());
        for (size_t i = 0; i < index_list.size(); i++) {
            int index = index_list[i];
            SDL_Rect rect_src = rect_src_list[i];

            rect_src.x = (index % num_h) * width_frame;
            rect_src.y = (index / num_h) * height_frame;
            rect_src.w = width_frame;
            rect_src.h = height_frame;
        }
    }

    /**
     * @brief 设置动画是否循环播放
     * @param loop true为循环播放，false为播放一次
     */
    void setLoop(bool loop)
    {
        this->is_loop = loop;
    }

    /**
     * @brief 设置帧间隔时间
     * @param interval 帧间隔(秒)
     */
    void setInterval(double interval)
    {
        m_timer.setWaitTime(interval);
    }

    /**
     * @brief 设置动画播放完成回调
     * @param callback 回调函数
     */
    void setOnFinished(const PlayCallback& callback)
    {
        this->on_finished = callback;
    }

    /**
     * @brief 更新动画状态
     * @param delta_time 距离上次更新的时间间隔(秒)
     */
    void onUpdate(double delta_time)
    {
        m_timer.onUpdate(delta_time);
    }

    /**
     * @brief 渲染当前动画帧
     *
     * @param renderer SDL渲染器
     * @param posion_dst 目标位置
     * @param angle 旋转角度(默认为0)
     */
    void onRender(SDL_Renderer* renderer, const SDL_Point& posion_dst, double angle = 0) const
    {
        static SDL_Rect rect_dst;

        rect_dst.x = posion_dst.x;
        rect_dst.y = posion_dst.y;
        rect_dst.w = width_frame;
        rect_dst.h = height_frame;

        SDL_RenderCopyEx(renderer, texture, &rect_src_list[index_frame], &rect_dst, angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
    }

private:
    Timer m_timer;                          // 帧更新计时器
    bool is_loop = true;                    // 是否循环播放
    size_t index_frame = 0;                 // 当前帧索引
    PlayCallback on_finished;               // 播放完成回调
    SDL_Texture* texture = nullptr;         // 精灵图纹理
    std::vector<SDL_Rect> rect_src_list;    // 源矩形列表
    int width_frame = 0;                    // 单帧宽度
    int height_frame = 0;                   // 单帧高度
};
