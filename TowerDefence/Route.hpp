#pragma once

#include "tile.h"

#include <SDL.h>
#include <vector>
//
/**
 * @brief 路径规划类，用于在瓦片地图上根据方向标记生成路径
 *
 * 该类通过读取瓦片地图上的方向标记，从起始点开始按照指定方向
 * 生成一条连续的路径，直到遇到无效方向或边界为止
 */
class Route
{
public:
	// 定义索引列表类型，用于存储路径上的点坐标
	typedef std::vector<SDL_Point> IndexList;

public:
	Route() = default;

	/**
	 * @brief 构造函数，根据瓦片地图和起始点生成路径
	 * @param tile_map 瓦片地图
	 * @param index_origin 起始点坐标
	 *
	 * 从起始点开始，根据每个瓦片的方向标记，生成一条连续的路径。
	 * 遇到以下情况时停止：
	 * 1. 超出地图边界
	 * 2. 遇到重复的索引点
	 * 3. 遇到特殊标记为0的瓦片
	 * 4. 遇到无效的方向标记
	 */
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

	/**
	 * @brief 获取路径索引列表
	 * @return 返回存储路径点的索引列表的常量引用
	 */
	const IndexList& getIndexList() const { return m_index_list; }

private:
	IndexList m_index_list;		// 存储路径上所有点的索引列表

private:
	/**
	 * @brief 检查给定索引是否在当前路径中重复
	 * @param target_index 待检查的目标索引
	 * @return 如果索引重复返回true，否则返回false
	 */
	bool checkDuplicateIndex(const SDL_Point& target_index) const
	{
		for (const auto& index : m_index_list) {
			if (index.x == target_index.x && index.y == target_index.y) 
				return true;
		}
		return false;
	}
};