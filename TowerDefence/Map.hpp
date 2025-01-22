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

class Map
{
public:
	typedef std::unordered_map<int, Route> SpawnerRoutePool;	// 生成路由池

public:
	Map() = default;
	~Map() = default;

	// 加载地图
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

		m_tile_map = std::move(tile_map_temp);

		generateMapCache();

		return true;
	}

	// 放置塔
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
	const SDL_Point& getHomeIndex() const { return m_index_home; }							//获取房屋索引
	const SpawnerRoutePool& getSpawnerRoutePool() const { return m_spawner_route_pool; }	//获取生成路由池

private:
	TileMap m_tile_map;
	SDL_Point m_index_home = { 0, 0 };
	SpawnerRoutePool m_spawner_route_pool;

private:
	// 截取空白字符
	std::string trimString(const std::string& str) const
	{
		size_t begin_index = str.find_first_not_of(" \t");
		if (begin_index == std::string::npos) return "";

		size_t end_index = str.find_last_not_of(" \t");
		size_t index_range = end_index - begin_index + 1;

		return str.substr(begin_index, index_range);
	}

	// 从字符串中加载Tile
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

	// 分割字符串并将值转换为整数
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

	// 解析一行数据
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

	// 生成地图缓存
	void generateMapCache()
	{
		for (int y = 0; y < getHeight(); y++) {
			for (int x = 0; x < getWidth(); x++) {
				const Tile& tile = m_tile_map[y][x];

				if(tile.special_flag < 0) continue;

				if (tile.special_flag == 0)	{
					m_index_home = { x, y };
				}
				else {
					m_spawner_route_pool[tile.special_flag] = Route(m_tile_map, { x, y });
				}
			}
		}
	}
};