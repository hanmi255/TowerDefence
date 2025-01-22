#pragma once

#include <vector>

constexpr auto TILE_SIZE = 48;				// 瓦片大小

struct Tile
{
	enum class Direction					// 方向
	{
		NONE = 0,
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	int terrian = 0;						// 地形
	int decoration = -1;					// 装饰
	int special_flag = -1;					// 特殊标记

	bool has_tower = false;					// 是否有塔

	Direction direction = Direction::NONE;	// 默认方向
};

typedef std::vector<std::vector<Tile>> TileMap;