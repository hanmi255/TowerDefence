#pragma once

#include "manager.hpp"
#include "../bullet/bullet.hpp"
#include "../bullet/bullet_type.hpp"
#include "../bullet/arrow_bullet.hpp"
#include "../bullet/axe_bullet.hpp"
#include "../bullet/shell_bullet.hpp"

#include <vector>
#include <memory>
#include <algorithm>

/**
 * @brief 子弹管理器类，负责管理游戏中所有子弹的生命周期
 * @details 继承自Manager单例模板类，实现了子弹的更新、渲染和发射等功能
 */
class BulletManager : public Manager<BulletManager>
{
	friend class Manager<BulletManager>;

public:
	using BulletList = std::vector<Bullet *>; // 子弹列表类型别名

public:
	/**
	 * @brief 更新所有子弹的状态
	 * @param delta_time 帧间隔时间
	 * @details 遍历所有子弹进行更新，并清理需要移除的子弹
	 */
	void onUpdate(double delta_time)
	{
		for (auto *bullet : m_bullet_list)
		{
			bullet->onUpdate(delta_time);
		}

		m_bullet_list.erase(std::remove_if(
								m_bullet_list.begin(), m_bullet_list.end(),
								[](const Bullet *bullet)
								{
									bool deletable = bullet->canRemove();
									if (deletable)
										delete bullet;

									return deletable;
								}),
							m_bullet_list.end());
	}

	/**
	 * @brief 渲染所有子弹
	 * @param renderer SDL渲染器指针
	 */
	void onRender(SDL_Renderer *renderer)
	{
		for (auto *bullet : m_bullet_list)
		{
			bullet->onRender(renderer);
		}
	}

	/**
	 * @brief 获取子弹列表
	 * @return 返回子弹列表的引用
	 */
	BulletList &getBulletList()
	{
		return m_bullet_list;
	}

	/**
	 * @brief 发射新子弹
	 * @param type 子弹类型
	 * @param position 初始位置
	 * @param velocity 初始速度
	 * @param damage 伤害值
	 * @details 根据类型创建对应的子弹对象，设置其属性后加入管理列表
	 */
	void fireBullet(BulletType type, const Vector2 &position, const Vector2 &velocity, double damage)
	{
		std::unique_ptr<Bullet> bullet = nullptr;
		switch (type)
		{
		case BulletType::Arrow:
			bullet = std::make_unique<ArrowBullet>();
			break;
		case BulletType::Axe:
			bullet = std::make_unique<AxeBullet>();
			break;
		case BulletType::Shell:
			bullet = std::make_unique<ShellBullet>();
			break;
		default:
			bullet = std::make_unique<ArrowBullet>();
			break;
		}

		bullet->setPosition(position);
		bullet->setVelocity(velocity);
		bullet->setDamage(damage);

		m_bullet_list.push_back(bullet.release());
	}

protected:
	BulletManager() = default;
	~BulletManager()
	{
		for (auto &bullet : m_bullet_list)
		{
			delete bullet;
		}
	}

private:
	BulletList m_bullet_list; // 存储所有活动子弹的列表
};
