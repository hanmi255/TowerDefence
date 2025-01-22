#pragma once

#include "Tile.h"

#include <SDL.h>
#include <vector>

class Route
{
public:
	typedef std::vector<SDL_Point> IndexList;	// 索引列表

public:
	Route() = default;

	Route(const TileMap& tile_map, const SDL_Point& index_origin)
	{
		size_t width = tile_map[0].size();										// 地图宽度
		size_t height = tile_map.size();										// 地图高度

		SDL_Point index_next = index_origin;									//初始化起始索引

		while (true) {
			if (index_next.x >= width || index_next.y >= height)				// 超出地图边界
				break;

			if (checkDuplicateIndex(index_next))								// 重复索引
				break;
			else
				m_index_list.push_back(index_next);

			bool is_next_direction_valid = true;								// 下一个方向是否有效
			const Tile& current_tile = tile_map[index_next.y][index_next.x];	// 当前位置的瓦片

			if (current_tile.special_flag == 0)
				break;
			switch (current_tile.direction) {
				case Tile::Direction::UP:
					index_next.y--;
					break;
				case Tile::Direction::DOWN:
					index_next.y++;
					break;
				case Tile::Direction::LEFT:
					index_next.x--;
					break;
				case Tile::Direction::RIGHT:
					index_next.x++;
					break;
				default:
					is_next_direction_valid = false;
					break;
			}

			if (!is_next_direction_valid)										// 方向无效
				break;
		}
	}

	~Route() = default;

	const IndexList& getIndexList() const { return m_index_list; }

private:
	IndexList m_index_list;

private:
	// 检查是否有重复的索引
	bool checkDuplicateIndex(const SDL_Point& target_index) const
	{
		for (const auto& index : m_index_list) {
			if (index.x == target_index.x && index.y == target_index.y) 
				return true;
		}
		return false;
	}

};