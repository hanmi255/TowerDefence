#pragma once

#include "bullet.hpp"
#include "resource_manager.hpp"

#include <vector>

/**
 * @brief 斧头子弹类，继承自Bullet基类
 *
 * 实现了一个带旋转动画效果的斧头形态子弹，
 * 具有减速敌人的特殊效果
 */
class AxeBullet : public Bullet
{
public:
    /**
     * @brief 构造函数
     *
     * @details 初始化斧头子弹的贴图动画、大小等属性：
     *          - 设置9帧循环动画
     *          - 动画间隔0.1秒
     *          - 禁用旋转
     *          - 设置48x48像素大小
     */
    AxeBullet()
    {
        static auto* tex_axe = ResourceManager::instance()->getTexturePool().find(ResID::Tex_BulletAxe)->second;

        static const std::vector<int> index_list = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

        animation.setLoop(true);
        animation.setInterval(0.1);
        animation.setFrameData(tex_axe, 4, 2, index_list);

        can_rotate = false;
        size.x = 48, size.y = 48;
    }

    /**
     * @brief 析构函数
     */
    ~AxeBullet() = default;

    /**
     * @brief 碰撞处理函数
     * @param enemy 被击中的敌人指针
     *
     * @details 当斧头击中敌人时：
     *           - 1. 随机播放3种斧头击中音效之一
     *           - 2. 对敌人施加减速效果
     *           - 3. 调用基类的碰撞处理
     */
    void onCollide(Enemy* enemy) override
    {
        static const auto& sound_pool = ResourceManager::instance()->getSoundPool();

        switch (rand() % 3)
        {
        case 0:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_1)->second, 0);
            break;
        case 1:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_2)->second, 0);
            break;
        case 2:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_3)->second, 0);
            break;
        }

        enemy->slowDown();

        Bullet::onCollide(enemy);
    }
};
