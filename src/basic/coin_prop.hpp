#pragma once

#include "../util/vector2.hpp"
#include "../util/timer.hpp"
#include "../manager/resource_manager.hpp"
#include "tile.hpp"


#include <SDL.h>

/**
 * @brief 金币道具类
 * @details 实现了金币的掉落、弹跳和漂浮动画效果
 *          金币生成后会先进行一个跳跃动作，然后悬浮一段时间后消失
 */
class CoinProp
{
public:
    /**
     * @brief 构造函数，初始化金币的物理属性和计时器
     * @details 设置两个计时器：
     *          - 跳跃计时器：控制跳跃动作的持续时间
     *          - 消失计时器：控制金币的存在时间
     */
    CoinProp()
    {
        // 配置跳跃计时器
        timer_jump.setOneShot(true);
        timer_jump.setWaitTime(interval_jump);
        timer_jump.setOnTimeOut([&]() { is_jumping = false; });

        // 配置消失计时器
        timer_disappear.setOneShot(true);
        timer_disappear.setWaitTime(interval_disappear);
        timer_disappear.setOnTimeOut([&]() { is_valid = false; });

        // 设置初始速度：水平随机方向，垂直向上
        velocity.x = (rand() % 2 ? 1 : -1) * 2 * TILE_SIZE;
        velocity.y = -3 * TILE_SIZE;
    }

    ~CoinProp() = default;

    /**
     * @brief 设置金币位置
     * @param position 新的位置坐标
     */
    void setPosition(const Vector2& position)
    {
        this->position = position;
    }

    /**
     * @brief 获取金币当前位置
     * @return 金币位置坐标
     */
    const Vector2& getPosition() const
    {
        return position;
    }

    /**
     * @brief 获取金币大小
     * @return 金币尺寸
     */
    const Vector2& getSize() const
    {
        return size;
    }

    /**
     * @brief 使金币失效
     */
    void makeInvalid()
    {
        is_valid = false;
    }

    /**
     * @brief 检查金币是否可以被移除
     * @return 如果金币无效则返回true
     */
    bool canRemove() const
    {
        return !is_valid;
    }

    /**
     * @brief 更新金币状态
     * @param delta_time 帧间隔时间
     * @details 处理金币的物理运动：
     *          - 跳跃阶段：受重力影响
     *          - 漂浮阶段：进行正弦运动
     */
    void onUpdate(double delta_time)
    {
        // 更新计时器
        timer_jump.onUpdate(delta_time);
        timer_disappear.onUpdate(delta_time);

        if (is_jumping) {
            // 跳跃阶段：应用重力
            velocity.y += gravity * delta_time;
        }
        else {
            // 漂浮阶段：执行正弦运动
            velocity.x = 0;
            velocity.y = sin(SDL_GetTicks64() / 1000.0 * 4) * 30;
        }

        // 更新位置
        position += velocity * delta_time;
    }

    /**
     * @brief 渲染金币
     * @param renderer SDL渲染器指针
     */
    void onRender(SDL_Renderer* renderer) const
    {
        static SDL_Rect rect = { 0, 0, (int)size.x, (int)size.y };
        static auto* tex_coin = ResourceManager::instance()->getTexturePool().find(ResID::Tex_Coin)->second;

        // 计算渲染位置（居中显示）
        rect.x = (int)(position.x - size.x / 2);
        rect.y = (int)(position.y - size.y / 2);

        SDL_RenderCopy(renderer, tex_coin, nullptr, &rect);
    }

private:
    Vector2 position;                 // 金币位置
    Vector2 velocity;                 // 金币速度
    Vector2 size = { 16, 16 };        // 金币大小

    Timer timer_jump;                 // 跳跃计时器
    Timer timer_disappear;            // 消失计时器

    bool is_valid = true;             // 金币是否有效
    bool is_jumping = true;           // 是否处于跳跃状态

    double gravity = 490;             // 重力加速度
    double interval_jump = 0.75;      // 跳跃持续时间
    double interval_disappear = 5.0;  // 金币存在时间
};
