#pragma once

#include "bullet.hpp"
#include "../manager/resource_manager.hpp"

#include <vector>

/**
 * @brief 炮弹类，继承自基础子弹类
 *
 * 实现了一种特殊的炮弹行为，具有爆炸动画效果和范围伤害
 */
class ShellBullet : public Bullet
{
public:
    /**
     * @brief 构造函数，初始化炮弹的基本属性和动画
     *
     * @details 设置炮弹的飞行动画和爆炸动画，并初始化基本参数：
     *          - 加载炮弹和爆炸效果的纹理资源
     *          - 配置动画帧序列和播放参数
     *          - 设置伤害范围和大小属性
     */
    ShellBullet()
    {
        static auto* tex_shell = ResourceManager::instance()->getTexturePool().find(ResID::Tex_BulletShell)->second;
        static auto* tex_explode = ResourceManager::instance()->getTexturePool().find(ResID::Tex_EffectExplode)->second;

        static const std::vector<int> index_list = { 0, 1 };                  // 炮弹动画帧索引
        static const std::vector<int> index_explode_list = { 0, 1, 2, 3, 4 }; // 爆炸动画帧索引

        // 配置炮弹飞行动画
        animation.setLoop(true);
        animation.setInterval(0.1);
        animation.setFrameData(tex_shell, 2, 1, index_list);

        // 配置爆炸动画
        anim_explode.setLoop(false);
        anim_explode.setInterval(0.1);
        anim_explode.setFrameData(tex_explode, 5, 1, index_explode_list);
        anim_explode.setOnFinished([&]() {
            makeInvalid();
            });

        damage_range = 96;   
        can_rotate = false;
        size.x = 48, size.y = 48;
    }

    ~ShellBullet() = default;

    /**
     * @brief 更新炮弹状态
     * @param delta_time 帧间隔时间
     *
     * 根据炮弹是否可碰撞来更新不同的动画状态
     */
    void onUpdate(double delta_time) override
    {
        if (canCollide()) {
            Bullet::onUpdate(delta_time);
            return;
        }

        anim_explode.onUpdate(delta_time);
    }

    /**
     * @brief 渲染炮弹
     * @param renderer SDL渲染器指针
     *
     * 根据炮弹状态渲染飞行或爆炸动画
     */
    void onRender(SDL_Renderer* renderer) override
    {
        if (canCollide()) {
            Bullet::onRender(renderer);
            return;
        }

        static SDL_Point point;
        point.x = (int)position.x - 96 / 2;
        point.y = (int)position.y - 96 / 2;

        anim_explode.onRender(renderer, point);
    }

    /**
     * @brief 处理与敌人的碰撞
     * @param enemy 被碰撞的敌人指针
     *
     * 播放碰撞音效并禁用后续碰撞
     */
    void onCollide(Enemy* enemy) override
    {
        static const auto& sound_pool = ResourceManager::instance()->getSoundPool();
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ShellHit)->second, 0);
        disableCollide();
    }

private:
    Animation anim_explode; // 爆炸动画
};
