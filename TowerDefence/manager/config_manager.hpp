#pragma once

#include "manager.hpp"
#include "../basic/map.hpp"
#include "../basic/wave.hpp"

#include <SDL.h>
#include <string>
#include <cJSON.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <array>

/**
 * @brief 游戏配置管理类，负责加载和管理所有游戏相关的配置数据
 * 采用单例模式设计，继承自Manager基类
 */
class ConfigManager : public Manager<ConfigManager> 
{
	friend class Manager<ConfigManager>;

public:
	/**
	 * @brief 基础游戏配置结构体
	 */
	struct BasicTemplate 
	{
        std::string window_title = "Tower Defence";  // 游戏窗口标题
		int window_width = 1280;                       // 窗口宽度
		int window_height = 720;                       // 窗口高度
	};

	/**
	 * @brief 玩家角色配置结构体
	 */
	struct PlayerTemplate 
	{
		double speed = 3.0;						// 移动速度
		double normal_attack_interval = 0.5;    // 普通攻击间隔时间
		double normal_attack_damage = 0;        // 普通攻击伤害
		double skill_interval = 10;             // 技能冷却时间
		double skill_damage = 1;                // 技能伤害
	};

	/**
	 * @brief 敌人单位配置结构体
	 */
	struct EnemyTemplate 
	{
		double hp = 100;                 // 生命值
		double speed = 1.0;              // 移动速度
		double damage = 1;               // 攻击伤害
		double reward_ratio = 0.5;		 // 击杀奖励系数
		double recover_interval = 10;    // 治疗间隔时间
		double recover_range = 0;        // 治疗范围
		double recover_intensity = 25;   // 治疗强度
	};

	/**
	 * @brief 防御塔配置结构体
	 */
	struct TowerTemplate 
	{
		double interval[10] = { 1 };        // 攻击间隔时间（各等级）
		double damage[10] = { 25 };         // 攻击伤害（各等级）
		double view_range[10] = { 5 };      // 攻击范围（各等级）
		double cost[10] = { 50 };           // 建造费用（各等级）
		double upgrade_cost[9] = { 75 };    // 升级费用（各等级）
	};

public:
	// 游戏状态相关成员
	Map map;                                  // 游戏地图实例
	std::vector<Wave> wave_list;              // 敌人波次列表

	// 防御塔等级状态
	int level_archer = 0;                     // 弓箭手塔当前等级
	int level_axeman = 0;                     // 斧头兵塔当前等级
	int level_gunner = 0;                     // 枪手塔当前等级

	// 游戏状态标志
	bool is_game_win = true;                  // 游戏胜利标志
	bool is_game_over = false;                // 游戏结束标志

	SDL_Rect rect_tile_map = { 0 };           // 地图瓦片区域

	// 各类型配置模板实例
	BasicTemplate basic_template;              // 基础配置
	PlayerTemplate player_template;            // 玩家配置

	// 敌人类型配置
	EnemyTemplate slime_template;              // 史莱姆配置
	EnemyTemplate king_slime_template;         // 皇家史莱姆配置
	EnemyTemplate skeleton_template;           // 骷髅配置
	EnemyTemplate goblin_template;             // 哥布林配置
	EnemyTemplate goblin_priest_template;      // 哥布林祭司配置

	// 防御塔类型配置
	TowerTemplate archer_template;            // 弓箭手塔配置
	TowerTemplate axeman_template;            // 斧头兵塔配置
	TowerTemplate gunner_template;            // 枪手塔配置

	// 游戏常量配置
	static constexpr double num_initial_hp = 10;      // 初始生命值
	static constexpr double num_initial_coin = 100;   // 初始金币数量
	static constexpr double num_coin_per_prop = 10;   // 每个道具价值

public:
	/**
	 * @brief 加载关卡配置文件
	 * @param path 关卡配置文件路径
	 * @return 加载成功返回true，失败返回false
	 * @details 从JSON格式的配置文件中读取并解析关卡波次信息
	 */
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

	/**
	 * @brief 加载游戏全局配置
	 * @param path 游戏配置文件路径
	 * @return 加载成功返回true，失败返回false
	 * @details 加载并解析包含基础配置、玩家配置、防御塔配置和敌人配置的JSON文件
	 */
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
		struct 
		{
			const char* key;
			EnemyTemplate& tmpl;
		} enemies[] = {
			{"slime", slime_template},
			{"king_slime", king_slime_template},
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
		struct 
		{
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
	/**
	 * @brief 智能指针类型定义，用于自动管理cJSON对象的内存
	 */
	using JsonPtr = std::unique_ptr<cJSON, decltype(&cJSON_Delete)>;

	/**
	 * @brief 创建管理cJSON对象的智能指针
	 * @param json 原始cJSON指针
	 * @return 包装后的智能指针
	 */
	JsonPtr makeJsonPtr(cJSON* json)
	{
		return JsonPtr(json, cJSON_Delete);
	}

	/**
	 * @brief 安全获取JSON对象中的子对象
	 * @param root JSON根节点
	 * @param key 要获取的键名
	 * @return 成功返回对应的cJSON对象指针，失败返回nullptr
	 */
	const cJSON* getJsonObject(const cJSON* root, const char* key) 
	{
		auto* obj = cJSON_GetObjectItem(root, key);
		return (obj && obj->type == cJSON_Object) ? obj : nullptr;
	}

	/**
	 * @brief 安全获取JSON中的数值
	 * @tparam T 目标数值类型
	 * @param json JSON对象
	 * @param key 键名
	 * @param value 输出参数，存储获取的值
	 * @return 获取成功返回true，失败返回false
	 */
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

	/**
	 * @brief 安全获取JSON中的字符串
	 * @param json JSON对象
	 * @param key 键名
	 * @param value 输出参数，存储获取的字符串
	 * @return 获取成功返回true，失败返回false
	 */
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

	/**
	 * @brief 解析JSON数组中的数值序列
	 * @tparam T 目标数组元素类型
	 * @param array 目标数组
	 * @param max_len 数组最大长度
	 * @param json_array JSON数组对象
	 */
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

	/**
	 * @brief 解析敌人类型字符串
	 * @param str 类型字符串
	 * @param type 输出参数，解析后的敌人类型枚举值
	 * @return 解析成功返回true，失败返回false
	 */
	bool parseEnemyType(const char* str, EnemyType& type)
	{
		static const std::unordered_map<std::string, EnemyType> enemyTypes = {
			{"Slime", EnemyType::Slime},
			{"KingSlime", EnemyType::KingSlime},
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

	/**
	 * @brief 解析敌人生成事件配置
	 * @param json_spawn_event 生成事件的JSON对象
	 * @param spawn_event 输出参数，解析后的生成事件结构
	 * @return 解析成功返回true，失败返回false
	 */
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

	/**
	 * @brief 解析波次配置
	 * @param json_wave 波次的JSON对象
	 * @param wave 输出参数，解析后的波次结构
	 * @return 解析成功返回true，失败返回false
	 */
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

	/**
	 * @brief 解析基础配置模板
	 * @param basic_template 输出参数，解析后的基础配置
	 * @param json_root JSON根节点
	 * @return 解析成功返回true，失败返回false
	 */
	bool parseBasicTemplate(BasicTemplate& basic_template, const cJSON* json_root) 
	{
		if (!json_root) return false;

		return getJsonString(json_root, "window_title", basic_template.window_title) &&
			getJsonNumber(json_root, "window_width", basic_template.window_width) &&
			getJsonNumber(json_root, "window_height", basic_template.window_height);
	}

	/**
	 * @brief 解析玩家配置模板
	 * @param player_template 输出参数，解析后的玩家配置
	 * @param json_root JSON根节点
	 * @return 解析成功返回true，失败返回false
	 */
	bool parsePlayerTemplate(PlayerTemplate& player_template, const cJSON* json_root) 
	{
		if (!json_root) return false;

		return getJsonNumber(json_root, "speed", player_template.speed) &&
			getJsonNumber(json_root, "normal_attack_interval", player_template.normal_attack_interval) &&
			getJsonNumber(json_root, "normal_attack_damage", player_template.normal_attack_damage) &&
			getJsonNumber(json_root, "skill_interval", player_template.skill_interval) &&
			getJsonNumber(json_root, "skill_damage", player_template.skill_damage);
	}

	/**
	 * @brief 解析敌人配置模板
	 * @param enemy_template 输出参数，解析后的敌人配置
	 * @param json_root JSON根节点
	 * @return 解析成功返回true，失败返回false
	 */
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

	/**
	 * @brief 解析防御塔配置模板
	 * @param tower_template 输出参数，解析后的防御塔配置
	 * @param json_root JSON根节点
	 * @return 解析成功返回true，失败返回false
	 */
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

