#pragma once

#include "bullet.hpp"
#include "resource_manager.hpp"

#include <vector>

/**
 * @brief 箭矢类型子弹
 * @details 继承自基础子弹类，实现了箭矢特有的视觉和音效效果
 */
class ArrowBullet : public Bullet
{
public:
    /**
     * @brief 构造函数
     * 
     * @details 初始化箭矢的贴图动画、旋转属性和碰撞体积:
     *          - 设置2帧循环动画
     *          - 启用旋转功能
     *          - 设置48x48的碰撞大小
     */
    ArrowBullet()
    {
        static auto* tex_arrow = ResourceManager::instance()->getTexturePool().find(ResID::Tex_BulletArrow)->second;

        static const std::vector<int> index_list = { 0, 1 };

        animation.setLoop(true);
        animation.setInterval(0.1);
        animation.setFrameData(tex_arrow, 2, 1, index_list);

        can_rotate = true;
        size.x = 48, size.y = 48;
    }

    /**
     * @brief 析构函数
     */
    ~ArrowBullet() = default;

    /**
     * @brief 碰撞响应函数
     * @param enemy 被击中的敌人指针
     * 
     * @details 当箭矢击中敌人时：
     *          1. 随机播放三种箭矢命中音效之一
     *          2. 调用基类的碰撞处理逻辑
     */
    void onCollide(Enemy* enemy) override
    {
        static const auto& sound_pool = ResourceManager::instance()->getSoundPool();

        switch (rand() % 3)
        {
        case 0:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_1)->second, 0);
            break;
        case 1:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_2)->second, 0);
            break;
        case 2:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_3)->second, 0);
            break;
        }

        Bullet::onCollide(enemy);
    }
};
