#pragma once

#include "EnemyType.h"

#include <vector>

struct Wave
{
	struct SpawnEvent
	{
		double interval = 0;						// 事件间隔
		int spawn_point = 1;						// 生成点
		EnemyType enemy_type = EnemyType::Slim;		// 敌人类型
	};

	double rewards = 0;								// 击杀奖励
	double interval = 0;							// 下一波间隔
	std::vector<SpawnEvent> spawn_event_list;			//生成事件
};