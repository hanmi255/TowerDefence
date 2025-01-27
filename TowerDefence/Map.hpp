#pragma once

#include "Tile.h"
#include "Route.hpp"

#include <SDL.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <stdexcept>

/**
 * @brief 游戏地图类，负责地图数据的加载、管理和查询
 *
 * 该类提供了地图文件的解析、地形管理、路径生成等功能
 */
class Map
{
public:
	/** @brief 生成路由池类型定义，键为生成点ID，值为对应路径 */
	using SpawnerRoutePool = std::unordered_map<int, Route>;

public:
	Map() = default;
	~Map() = default;

	/**
	 * @brief 从文件加载地图数据
	 * @param file_path 地图文件路径
	 * @return 加载是否成功
	 * @throw std::runtime_error 当文件打开失败或地图数据无效时抛出异常
	 */
	bool loadMap(const std::string& file_path)
	{
		std::ifstream file(file_path);
		if (!file.is_open())
			throw std::runtime_error("Failed to open map file: " + file_path);

		TileMap tile_map_temp;

		int index_x = -1, index_y = -1;

		std::string str_line;
		while (std::getline(file, str_line)) {
			str_line = trimString(str_line);
			if (str_line.empty()) continue;

			index_x = -1; index_y++;
			tile_map_temp.emplace_back();

			parseLine(str_line, index_x, index_y, tile_map_temp);
		}

		file.close();

		if (tile_map_temp.empty() || tile_map_temp[0].empty()) 
			throw std::runtime_error("Invalid map data in file: " + file_path);

		this->m_tile_map = tile_map_temp;

		generateMapCache();

		return true;
	}

	/**
	 * @brief 在指定位置放置防御塔
	 * @param tile_index 要放置防御塔的格子坐标
	 */
	void placeTower(const SDL_Point& tile_index)
	{
		m_tile_map[tile_index.y][tile_index.x].has_tower = true;
	}

	// 获取地图尺寸
	size_t getWidth() const { 
		if (m_tile_map.empty()) return 0;
		return m_tile_map[0].size();
	}
	size_t getHeight() const { return m_tile_map.size(); }

	const TileMap& getTileMap() const { return m_tile_map; }								// 获取地图数据
	const SDL_Point& getIndexHome() const { return m_index_home; }							// 获取房屋索引
	const SpawnerRoutePool& getSpawnerRoutePool() const { return m_spawner_route_pool; }	// 获取生成路由池

private:
	TileMap m_tile_map;						// 地图数据
	SDL_Point m_index_home = { 0 };			// 房屋位置索引
	SpawnerRoutePool m_spawner_route_pool;  // 怪物生成点路由池

private:
	/**
	 * @brief 移除字符串首尾的空白字符
	 * @param str 待处理的字符串
	 * @return 处理后的字符串
	 */
	std::string trimString(const std::string& str) const
	{
		size_t begin_index = str.find_first_not_of(" \t");
		if (begin_index == std::string::npos) return "";

		size_t end_index = str.find_last_not_of(" \t");
		size_t index_range = end_index - begin_index + 1;

		return str.substr(begin_index, index_range);
	}

	/**
	 * @brief 从字符串解析Tile数据
	 * @param tile 待填充的Tile对象
	 * @param str 包含Tile数据的字符串
	 */
	void loadTileFromStr(Tile& tile, const std::string& str)
	{
		std::string str_tidy = trimString(str);

		std::vector<int> vec_values;
		splitAndConvertToInts(str_tidy, vec_values);

		tile.terrian = (vec_values.size() < 1 || vec_values[0] < 0) ? 0 : vec_values[0];						// 地形
		tile.decoration = (vec_values.size() < 2) ? -1 : vec_values[1];											// 装饰
		tile.direction = (Tile::Direction)((vec_values.size() < 3 || vec_values[2] < 0) ? 0 : vec_values[2]);	// 方向
		tile.special_flag = (vec_values.size() <= 3) ? -1 : vec_values[3];										// 特殊标记
	}

	/**
	 * @brief 将字符串分割并转换为整数数组
	 * @param str 待分割的字符串
	 * @param values 存储转换结果的向量
	 */
	void splitAndConvertToInts(const std::string& str, std::vector<int>& values)
	{
		std::stringstream str_stream(str);
		std::string str_value;

		while (std::getline(str_stream, str_value, '\\')) {
			try {
				values.push_back(std::stoi(str_value));
			}
			catch (const std::invalid_argument&) {
				values.push_back(-1); // 若转换失败则使用默认值
			}
		}
	}

	/**
	 * @brief 解析地图文件中的一行数据
	 * @param line 行数据字符串
	 * @param index_x 当前X坐标索引
	 * @param index_y 当前Y坐标索引
	 * @param tile_map_temp 临时地图数据存储
	 */
	void parseLine(const std::string& line, int& index_x, int index_y, TileMap& tile_map_temp)
	{
		std::stringstream str_stream(line);
		std::string str_tile;

		while (std::getline(str_stream, str_tile, ',')) {
			index_x++;
			tile_map_temp[index_y].emplace_back();
			Tile& tile = tile_map_temp[index_y].back();
			loadTileFromStr(tile, str_tile);
		}
	}

	/**
	 * @brief 生成地图缓存数据
	 *
	 * 遍历地图，识别特殊点（如家园位置、怪物生成点），
	 * 并为每个生成点计算到达家园的路径
	 */
	void generateMapCache()
	{
		for (int y = 0; y < getHeight(); y++) {
			for (int x = 0; x < getWidth(); x++) {
				const Tile& tile = m_tile_map[y][x];

				if(tile.special_flag < 0) continue;

				if (tile.special_flag == 0)	{
					m_index_home.x = x;
					m_index_home.y = y;
				}
				else {
					m_spawner_route_pool[tile.special_flag] = Route(m_tile_map, { x, y });
				}
			}
		}
	}
};