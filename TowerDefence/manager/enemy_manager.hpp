#pragma once

#include "manager.hpp"
#include "config_manager.hpp"
#include "home_manager.hpp"
#include "bullet_manager.hpp"
#include "coin_manager.hpp"
#include "../enemy/enemy.hpp"
#include "../enemy/slime_enemy.hpp"
#include "../enemy/king_slime_enemy.hpp"
#include "../enemy/skeleton_enemy.hpp"
#include "../enemy/goblin_enemy.hpp"
#include "../enemy/goblin_priest_enemy.hpp"

#include <vector>
#include <memory>

/**
 * @brief 敌人管理器类，负责管理游戏中所有敌人单位
 * @details 继承自Manager模板类，使用单例模式实现
 */
class EnemyManager : public Manager<EnemyManager> 
{
    friend class Manager<EnemyManager>;

public:
    using EnemyList = std::vector<Enemy*>;  // 敌人列表类型别名

public:
    /**
     * @brief 更新所有敌人状态
     * @param deltaTime 帧间隔时间
     */
    void onUpdate(double delta_time)
    {
        // 更新每个敌人的状态
        for (auto* enemy : m_enemy_list) {
            enemy->onUpdate(delta_time);
        }

        processHomeCollision();      // 处理与基地的碰撞
        processBulletCollision();    // 处理与子弹的碰撞

        removeInvaliedEnemy();       // 移除无效的敌人
    }

    /**
     * @brief 渲染所有敌人
     * @param renderer SDL渲染器指针
     */
    void onRender(SDL_Renderer* renderer)
    {
        for (auto* enemy : m_enemy_list) {
            enemy->onRender(renderer);
        }
    }

    /**
     * @brief 在指定生成点生成指定类型的敌人
     * @param type 敌人类型枚举值
     * @param index_spawn_point 生成点的索引
     *
     * @details 函数功能：
     * 1. 根据给定的生成点索引获取路径信息
     * 2. 根据敌人类型创建对应的敌人实例
     * 3. 设置敌人的技能释放回调（处理治疗范围效果）
     * 4. 设置敌人的初始位置和移动路径
     */
    void spawnEnemy(EnemyType type, const int index_spawn_point)
    {
        static Vector2 position_spawn;  // 敌人生成位置
        static const auto& rect_tile_map = ConfigManager::instance()->rect_tile_map;  // 地图矩形区域
        static const auto& spawner_route_pool = ConfigManager::instance()->map.getSpawnerRoutePool();  // 生成点路径池

        const auto& itor = spawner_route_pool.find(index_spawn_point);
        if (itor == spawner_route_pool.end()) return;

        std::unique_ptr<Enemy> enemy = nullptr;
        switch (type) {
        case EnemyType::Slime:
            enemy = std::make_unique<SlimeEnemy>();
            break;
        case EnemyType::KingSlime:
            enemy = std::make_unique<KingSlimeEnemy>();
            break;
        case EnemyType::Skeleton:
            enemy = std::make_unique<SkeletonEnemy>();
            break;
        case EnemyType::Goblin:
            enemy = std::make_unique<GoblinEnemy>();
            break;
        case EnemyType::GoblinPriest:
            enemy = std::make_unique<GoblinPriestEnemy>();
            break;
        default:
            enemy = std::make_unique<SlimeEnemy>();
            break;
        }

        // 设置敌人的技能释放回调，处理治疗范围效果
        enemy->setOnSkillReleased(
            [&](Enemy* enemy_src) {
                double recover_radius = enemy_src->getRecoverRadius();
                if (recover_radius < 0) return;

                const Vector2 position_src = enemy_src->getPosition();
                for (auto* enemy_dst : m_enemy_list) {
                    if (enemy_dst == enemy_src) continue;

                    const Vector2& position_dst = enemy_dst->getPosition();
                    double distance = (position_dst - position_src).length();
                    if (distance <= recover_radius)
                        enemy_dst->increaseHP(enemy_src->getRecoverIntensity());
                }
            });

        // 计算生成位置坐标
        const auto& index_list = itor->second.getIndexList();
        position_spawn.x = rect_tile_map.x + index_list[0].x * TILE_SIZE + TILE_SIZE / 2;
        position_spawn.y = rect_tile_map.y + index_list[0].y * TILE_SIZE + TILE_SIZE / 2;

        enemy->setPosition(position_spawn);
        enemy->setRoute(&itor->second);

        m_enemy_list.push_back(enemy.release());
    }

    /**
     * @brief 检查是否清除了所有敌人
     * @return true 如果没有剩余敌人，false 否则
     */
    bool checkCleared()
    {
        return m_enemy_list.empty();
    }

    /**
     * @brief 获取敌人列表
     * @return EnemyList& 敌人列表引用
     */
    EnemyManager::EnemyList& getEnemyList()
    {
        return m_enemy_list;
    }

protected:
    EnemyManager() = default;
    ~EnemyManager()
    {
        for (auto* enemy : m_enemy_list) {
            delete enemy;
        }
    }

private:
    EnemyList m_enemy_list;  // 存储所有敌人对象的容器

private:
    /**
     * @brief 处理敌人与基地的碰撞检测
     * @details 检查每个敌人是否与基地位置重叠，如果重叠则造成伤害
     */
    void processHomeCollision()
    {
        // 获取基地相关的静态配置
        static const auto& index_home = ConfigManager::instance()->map.getIndexHome();
        static const auto& rect_tile_map = ConfigManager::instance()->rect_tile_map;
        static const Vector2 position_home_tile = {
            (double)rect_tile_map.x + index_home.x * TILE_SIZE,
            (double)rect_tile_map.y + index_home.y * TILE_SIZE
        };

        // 检测每个敌人与基地的碰撞
        for (auto* enemy : m_enemy_list) {
            if (enemy->canRemove()) continue;

            const Vector2& position_enemy = enemy->getPosition();
            // 检查敌人是否在基地范围内
            if (position_enemy.x >= position_home_tile.x
                && position_enemy.y >= position_home_tile.y
                && position_enemy.x <= position_home_tile.x + TILE_SIZE
                && position_enemy.y <= position_home_tile.y + TILE_SIZE) {
                enemy->makeInvalid();
                HomeManager::instance()->decreaseHP(enemy->getDamage());
            }
        }
    }

    /**
     * @brief 处理敌人与子弹的碰撞检测
     * @note 待实现
     */
    void processBulletCollision()
    {
        static const auto& bullet_list = BulletManager::instance()->getBulletList();

        for (auto* enemy : m_enemy_list) {
            if (enemy->canRemove()) continue;

            const Vector2& size_enemy = enemy->getSize();
            const Vector2& position_enemy = enemy->getPosition();

            for (auto* bullet : bullet_list) {
                if (!bullet->canCollide()) continue;

                const Vector2& position_bullet = bullet->getPosition();

                if (position_bullet.x >= position_enemy.x - size_enemy.x / 2
                    && position_bullet.x <= position_enemy.x + size_enemy.x / 2
                    && position_bullet.y >= position_enemy.y - size_enemy.y / 2
                    && position_bullet.y <= position_enemy.y + size_enemy.y / 2) {
                    double damage = bullet->getDamage();
                    double damage_range = bullet->getDamageRange();

                    if (damage_range < 0) {
                        enemy->decreaseHP(damage);
                        if (enemy->canRemove()) 
                            trySpawnCoinProp(position_enemy, enemy->getRewardRatio());
                    }
                    else {
                        for (auto* target_enemy : m_enemy_list) {
                            const Vector2& position_target_enemy = target_enemy->getPosition();

                            if ((position_target_enemy - position_bullet).length() <= damage_range) {
                                target_enemy->decreaseHP(damage);
                                if (target_enemy->canRemove())
                                    trySpawnCoinProp(position_target_enemy, target_enemy->getRewardRatio());
                            }
                        }
                    }

                    bullet->onCollide(enemy);
                }
            }
        }
    }

    /**
     * @brief 移除标记为无效的敌人对象
     * @details 使用remove_if算法清理已经标记为可移除的敌人
     */
    void removeInvaliedEnemy()
    {
        m_enemy_list.erase(std::remove_if(
            m_enemy_list.begin(), m_enemy_list.end(),
            [](const Enemy* enemy) { 
                bool deletable = enemy->canRemove();
                if (deletable) 
                    delete enemy;

                return deletable;
            }), m_enemy_list.end());
    }

    /**
     * @brief 尝试生成金币道具
     * @param position 道具生成位置
     * @param ratio 道具生成概率
     * @details 随机生成一个0-1之间的浮点数，如果小于等于ratio，则生成一个金币道具
     */
    void trySpawnCoinProp(const Vector2& position, double ratio)
    {
        static auto* coin = CoinManager::instance();

        if((double)(rand() % 100) / 100 <= ratio)
            coin->spawnCoinProp(position);
    }
};
