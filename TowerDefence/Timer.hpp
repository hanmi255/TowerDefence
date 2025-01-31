#pragma once

#include <functional>

/**
 * @brief 计时器类，用于处理定时触发事件
 *
 * 支持单次/循环触发模式，可设置回调函数，支持暂停/恢复功能
 */
class Timer
{
public:
    Timer() = default;
    ~Timer() = default;

    /**
     * @brief 重置计时器状态
     *
     * 将已过时间清零，重置触发状态
     */
    void restart()
    {
        pass_time = 0.0;
        shotted = false;
    }

    /**
     * @brief 设置等待时间
     * @param wait_time 需要等待的时间（秒）
     */
    void setWaitTime(double wait_time)
    {
        this->wait_time = wait_time;
    }

    /**
     * @brief 设置是否为单次触发模式
     * @param one_shot true表示单次触发，false表示循环触发
     */
    void setOneShot(bool one_shot)
    {
        this->one_shot = one_shot;
    }

    /**
     * @brief 设置超时回调函数
     * @param on_time_out 时间到达时要执行的回调函数
     */
    void setOnTimeOut(std::function<void()> on_time_out)
    {
        this->onTimeOut = on_time_out;
    }

    /**
     * @brief 暂停计时器
     *
     * 暂停后计时器将停止累加时间
     */
    void pause()
    {
        paused = true;
    }

    /**
     * @brief 恢复计时器
     *
     * 恢复计时器的计时功能
     */
    void resume()
    {
        paused = false;
    }

    /**
     * @brief 更新计时器状态
     * @param delta_time 距离上次更新的时间间隔（秒）
     *
     * 累加经过的时间，当达到等待时间时触发回调函数
     * 在单次触发模式下，回调函数只会被触发一次
     */
    void onUpdate(double delta_time)
    {
        if (paused)	return;
        pass_time += delta_time;
        if (pass_time >= wait_time)
        {
            bool can_shot = (!one_shot || (one_shot && !shotted));
            shotted = true;
            if (can_shot && onTimeOut)
                onTimeOut();

            pass_time -= wait_time;
        }
    }

private:
    double wait_time = 0.0;                     // 需要等待的时间
    bool paused = false;                        // 是否暂停
    bool shotted = false;                       // 是否已经触发
    bool one_shot = false;                      // 是否为单次触发模式
    std::function<void()> onTimeOut = nullptr;  // 超时回调函数
};
