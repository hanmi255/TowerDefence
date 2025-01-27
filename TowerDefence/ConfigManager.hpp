#pragma once

#include "Manager.h"
#include "Map.hpp"
#include "Wave.h"

#include <SDL.h>
#include <string>
#include <cJSON.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <array>

class ConfigManager : public Manager<ConfigManager> {
	friend class Manager<ConfigManager>;

public:
	struct BasicTemplate  // 基础配置
	{
		std::string window_title = u8"Tower Defence";
		int window_width = 1280;
		int window_height = 720;
	};

	struct PlayerTemplate // 玩家配置
	{
		double speed = 3.0;
		double normal_attack_interval = 0.5;
		double normal_attack_damage = 0;
		double skill_interval = 10;
		double skill_damage = 1;
	};

	struct EnemyTemplate  // 敌人配置
	{
		double hp = 100;
		double speed = 1.0;
		double damage = 1;
		double reward_ratio = 0.5;
		double recover_interval = 10;
		double recover_range = 0;
		double recover_intensity = 25;
	};

	struct TowerTemplate  // 防御塔配置
	{
		double interval[10] = { 1 };
		double damage[10] = { 25 };
		double view_range[10] = { 5 };
		double cost[10] = { 50 };
		double upgrade_cost[9] = { 75 };
	};

public:
	Map map;										 // 地图
	std::vector<Wave> wave_list;					 // 波次列表

	int level_archer = 0;							 // 弓箭手塔等级
	int level_axeman = 0;							 // 斧头兵塔等级
	int level_gunner = 0;							 // 枪手塔等级

	bool is_game_win = true;						 // 游戏是否胜利
	bool is_game_over = false;						 // 游戏是否结束

	SDL_Rect rect_tile_map = { 0 };					 // 瓦片地图的矩形

	BasicTemplate basic_template;					 // 基础配置

	PlayerTemplate player_template;					 // 玩家配置

	EnemyTemplate slim_template;					 // 史莱姆配置
	EnemyTemplate king_slim_template;				 // 皇家史莱姆配置
	EnemyTemplate skeleton_template;				 // 骷髅配置
	EnemyTemplate goblin_template;					 // 哥布林配置
	EnemyTemplate goblin_priest_template;			 // 哥布林祭司配置

	TowerTemplate archer_template;					 // 弓箭手塔配置
	TowerTemplate axeman_template;					 // 斧头兵塔配置
	TowerTemplate gunner_template;					 // 枪手塔配置

	static constexpr double num_initial_hp = 10;	 // 初始血量
	static constexpr double num_initial_coin = 100;	 // 初始金币
	static constexpr double num_coin_per_prop = 10;	 // 每个道具的金币数

public:
	bool loadLevelConfig(const std::string& path) 
	{
		// 读取文件内容
		std::ifstream file(path);
		if (!file.good()) 
			return false;

		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();

		// 解析JSON
		JsonPtr json_root = makeJsonPtr(cJSON_Parse(buffer.str().c_str()));
		if (!json_root || json_root->type != cJSON_Array)
			return false;

		// 清空现有数据
		wave_list.clear();

		// 解析每个波次
		cJSON* json_wave = nullptr;
		cJSON_ArrayForEach(json_wave, json_root.get()) {
			wave_list.emplace_back();
			if (!parseWave(json_wave, wave_list.back())) {
				wave_list.pop_back();
				continue;
			}
		}

		return !wave_list.empty();
	}

	//加载游戏配置
	bool loadGameConfig(const std::string& path) 
	{
		// 读取文件内容
		std::ifstream file(path);
		if (!file) return false;

		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();

		// 解析JSON
		JsonPtr json_root = makeJsonPtr(cJSON_Parse(buffer.str().c_str()));
		if (!json_root || json_root->type != cJSON_Object) {
			return false;
		}

		// 获取各个配置节点
		const cJSON* json_basic = getJsonObject(json_root.get(), "basic");
		const cJSON* json_player = getJsonObject(json_root.get(), "player");
		const cJSON* json_tower = getJsonObject(json_root.get(), "tower");
		const cJSON* json_enemy = getJsonObject(json_root.get(), "enemy");

		if (!json_basic || !json_player || !json_tower || !json_enemy)
			return false;

		// 解析基本配置和玩家配置
		if (!parseBasicTemplate(basic_template, json_basic) ||
			!parsePlayerTemplate(player_template, json_player)) 
			return false;

		// 解析敌人配置
		struct {
			const char* key;
			EnemyTemplate& tmpl;
		} enemies[] = {
			{"slim", slim_template},
			{"king_slim", king_slim_template},
			{"skeleton", skeleton_template},
			{"goblin", goblin_template},
			{"goblin_priest", goblin_priest_template}
		};

		for (const auto& enemy : enemies) {
			if (!parseEnemyTemplate(enemy.tmpl, getJsonObject(json_enemy, enemy.key))) {
				return false;
			}
		}

		// 解析防御塔配置
		struct {
			const char* key;
			TowerTemplate& tmpl;
		} towers[] = {
			{"archer", archer_template},
			{"axeman", axeman_template},
			{"gunner", gunner_template}
		};

		for (const auto& tower : towers) {
			if (!parseTowerTemplate(tower.tmpl, getJsonObject(json_tower, tower.key))) {
				return false;
			}
		}

		return true;
	}

private:
	// 使用智能指针包装cJSON以自动管理内存
	using JsonPtr = std::unique_ptr<cJSON, decltype(&cJSON_Delete)>;

	// 创建自动删除的JSON指针
	JsonPtr makeJsonPtr(cJSON* json)
	{
		return JsonPtr(json, cJSON_Delete);
	}

	// 安全获取JSON对象
	const cJSON* getJsonObject(const cJSON* root, const char* key) 
	{
		auto* obj = cJSON_GetObjectItem(root, key);
		return (obj && obj->type == cJSON_Object) ? obj : nullptr;
	}

	// 安全获取JSON数值
	template<typename T>
	bool getJsonNumber(const cJSON* json, const char* key, T& value)
	{
		if (auto item = cJSON_GetObjectItem(json, key)) {
			if (item->type == cJSON_Number) {
				value = static_cast<T>(item->valuedouble);
				return true;
			}
		}
		return false;
	}

	// 安全获取JSON字符串
	bool getJsonString(const cJSON* json, const char* key, std::string& value) 
	{
		if (auto item = cJSON_GetObjectItem(json, key)) {
			if (item->type == cJSON_String) {
				value = item->valuestring;
				return true;
			}
		}
		return false;
	}

	// 解析数值数组
	template<typename T>
	void parseNumberArray(T* array, size_t max_len, const cJSON* json_array) 
	{
		if (!json_array || json_array->type != cJSON_Array) return;

		size_t index = 0;
		cJSON* item = nullptr;
		cJSON_ArrayForEach(item, json_array) {
			if (index >= max_len || item->type != cJSON_Number) {
				continue;
			}
			array[index++] = static_cast<T>(item->valuedouble);
		}
	}

	// 解析敌人类型
	bool parseEnemyType(const char* str, EnemyType& type)
	{
		static const std::unordered_map<std::string, EnemyType> enemyTypes = {
			{"Slim", EnemyType::Slim},
			{"KingSlim", EnemyType::KingSlim},
			{"Skeleton", EnemyType::Skeleton},
			{"Goblin", EnemyType::Goblin},
			{"GoblinPriest", EnemyType::GoblinPriest}
		};

		auto it = enemyTypes.find(str);
		if (it != enemyTypes.end()) {
			type = it->second;
			return true;
		}
		return false;
	}

	// 解析生成事件
	bool parseSpawnEvent(const cJSON* json_spawn_event, Wave::SpawnEvent& spawn_event)
	{
		if (!json_spawn_event || json_spawn_event->type != cJSON_Object) {
			return false;
		}

		// 获取基本属性
		if (!getJsonNumber(json_spawn_event, "interval", spawn_event.interval) ||
			!getJsonNumber(json_spawn_event, "spawn_point", spawn_event.spawn_point)) {
			return false;
		}

		// 解析敌人类型
		auto json_enemy_type = cJSON_GetObjectItem(json_spawn_event, "enemy_type");
		if (!json_enemy_type || json_enemy_type->type != cJSON_String ||
			!parseEnemyType(json_enemy_type->valuestring, spawn_event.enemy_type)) {
			return false;
		}

		return true;
	}

	// 解析波次数据
	bool parseWave(const cJSON* json_wave, Wave& wave)
	{
		if (!json_wave || json_wave->type != cJSON_Object) {
			return false;
		}

		// 获取基本属性
		if (!getJsonNumber(json_wave, "rewards", wave.rewards) ||
			!getJsonNumber(json_wave, "interval", wave.interval)) {
			return false;
		}

		// 解析生成事件列表
		auto json_spawn_list = cJSON_GetObjectItem(json_wave, "spawn_list");
		if (!json_spawn_list || json_spawn_list->type != cJSON_Array) 
			return false;

		cJSON* json_spawn_event = nullptr;
		cJSON_ArrayForEach(json_spawn_event, json_spawn_list) {
			wave.spawn_event_list.emplace_back();
			if (!parseSpawnEvent(json_spawn_event, wave.spawn_event_list.back())) {
				wave.spawn_event_list.pop_back();
				continue;
			}
		}

		return !wave.spawn_event_list.empty();
	}

	// 解析基础配置
	bool parseBasicTemplate(BasicTemplate& basic_template, const cJSON* json_root) 
	{
		if (!json_root) return false;

		return getJsonString(json_root, "window_title", basic_template.window_title) &&
			getJsonNumber(json_root, "window_width", basic_template.window_width) &&
			getJsonNumber(json_root, "window_height", basic_template.window_height);
	}

	// 解析玩家配置
	bool parsePlayerTemplate(PlayerTemplate& player_template, const cJSON* json_root) 
	{
		if (!json_root) return false;

		return getJsonNumber(json_root, "speed", player_template.speed) &&
			getJsonNumber(json_root, "normal_attack_interval", player_template.normal_attack_interval) &&
			getJsonNumber(json_root, "normal_attack_damage", player_template.normal_attack_damage) &&
			getJsonNumber(json_root, "skill_interval", player_template.skill_interval) &&
			getJsonNumber(json_root, "skill_damage", player_template.skill_damage);
	}

	// 解析敌人配置
	bool parseEnemyTemplate(EnemyTemplate& enemy_template, const cJSON* json_root) 
	{
		if (!json_root) return false;

		return getJsonNumber(json_root, "hp", enemy_template.hp) &&
			getJsonNumber(json_root, "speed", enemy_template.speed) &&
			getJsonNumber(json_root, "damage", enemy_template.damage) &&
			getJsonNumber(json_root, "reward_ratio", enemy_template.reward_ratio) &&
			getJsonNumber(json_root, "recover_interval", enemy_template.recover_interval) &&
			getJsonNumber(json_root, "recover_range", enemy_template.recover_range) &&
			getJsonNumber(json_root, "recover_intensity", enemy_template.recover_intensity);
	}

	// 解析防御塔配置
	bool parseTowerTemplate(TowerTemplate& tower_template, const cJSON* json_root) 
	{
		if (!json_root) return false;

		parseNumberArray(tower_template.interval, 10, cJSON_GetObjectItem(json_root, "interval"));
		parseNumberArray(tower_template.damage, 10, cJSON_GetObjectItem(json_root, "damage"));
		parseNumberArray(tower_template.view_range, 10, cJSON_GetObjectItem(json_root, "view_range"));
		parseNumberArray(tower_template.cost, 10, cJSON_GetObjectItem(json_root, "cost"));
		parseNumberArray(tower_template.upgrade_cost, 9, cJSON_GetObjectItem(json_root, "upgrade_cost"));

		return true;
	}

protected:
	ConfigManager() = default;
	~ConfigManager() = default;
};

