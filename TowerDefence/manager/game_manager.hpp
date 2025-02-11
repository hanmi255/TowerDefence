#pragma once

#include "manager.hpp"
#include "resource_manager.hpp"
#include "config_manager.hpp"
#include "enemy_manager.hpp"
#include "wave_manager.hpp"
#include "tower_manager.hpp"
#include "bullet_manager.hpp"
#include "../ui/status_bar.hpp"
#include "../ui/panel/panel.hpp"
#include "../ui/panel/place_panel.hpp"
#include "../ui/panel/upgrade_panel.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <chrono>

// 自定义删除器，用于智能指针管理SDL资源
struct SDLDeleter 
{
    void operator()(SDL_Window* w) const { SDL_DestroyWindow(w); }
    void operator()(SDL_Renderer* r) const { SDL_DestroyRenderer(r); }
    void operator()(SDL_Texture* t) const { SDL_DestroyTexture(t); }
};

/**
 * @brief 游戏管理器类，负责游戏生命周期和资源管理
 */
class GameManager : public Manager<GameManager> 
{
    friend class Manager<GameManager>;

public:
    /** @brief 运行游戏主循环 */
    int run(int argc, char** argv) 
    {
        using clock = std::chrono::high_resolution_clock;
        constexpr double TARGET_FPS = 60.0;
        constexpr double FRAME_TIME = 1.0 / TARGET_FPS;

        auto last_time = clock::now();

        while (!m_quit) {
            // 事件处理
            processEvents();

            // 时间控制和帧率限制
            auto current_time = clock::now();
            double delta_time = std::chrono::duration<double>(current_time - last_time).count();

            if (delta_time < FRAME_TIME) {
                auto sleep_time = static_cast<Uint32>((FRAME_TIME - delta_time) * 1000);
                SDL_Delay(sleep_time);
                current_time = clock::now();
                delta_time = FRAME_TIME;
            }

            last_time = current_time;

            // 更新和渲染
            onUpdate(delta_time);

            SDL_SetRenderDrawColor(m_renderer.get(), 0, 0, 0, 255);
            SDL_RenderClear(m_renderer.get());

            onRender();

            SDL_RenderPresent(m_renderer.get());
        }

        return 0;
    }

protected:
    /** @brief 初始化游戏系统 */
    GameManager() 
    {
        initializeSDL();
        loadConfig();
        createWindowAndRenderer();

        initAssert(ResourceManager::instance()->loadFromFile(m_renderer.get()), u8"资源管理器初始化失败");
        initAssert(generateTileMapTexture(), u8"瓦片地图纹理生成失败");

        m_status_bar.setPosition(15, 15);
        m_place_panel = std::make_unique<PlacePanel>();
        m_upgrade_panel = std::make_unique<UpgradePanel>();
    }

    /** @brief 清理SDL资源 */
    ~GameManager() 
    {
        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
    }

private:
    SDL_Event m_event;                                       // SDL事件
    bool m_quit = false;                                     // 退出标志

    StatusBar m_status_bar;                                  // 状态栏

    std::unique_ptr<SDL_Window, SDLDeleter> m_window;        // 游戏窗口
    std::unique_ptr<SDL_Renderer, SDLDeleter> m_renderer;    // 渲染器
    std::unique_ptr<SDL_Texture, SDLDeleter> m_tex_tile_map; // 瓦片地图纹理

    std::unique_ptr<PlacePanel> m_place_panel;               // 放置塔面板
    std::unique_ptr<UpgradePanel> m_upgrade_panel;           // 升级塔面板

private:
    /** @brief 初始化检查 */
    void initAssert(bool flag, const char* errorMessage)
    {
        if (flag) return;

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"游戏启动失败", errorMessage, m_window.get());
        exit(-1);
    }

    /** @brief 初始化SDL系统 */
    void initializeSDL() 
    {
        initAssert(!SDL_Init(SDL_INIT_EVERYTHING), u8"SDL初始化失败");
        initAssert(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG), u8"SDL_image初始化失败");
        initAssert(Mix_Init(MIX_INIT_MP3), u8"SDL_mixer 初始化失败！");
        initAssert(!TTF_Init(), u8"SDL_ttf初始化失败");
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    }

    /** @brief 加载配置文件 */
    void loadConfig() 
    {
        initAssert(ConfigManager::instance()->map.loadMap("file/map.csv"), u8"地图加载失败");
        initAssert(ConfigManager::instance()->loadLevelConfig("file/level.json"), u8"关卡配置加载失败");
        initAssert(ConfigManager::instance()->loadGameConfig("file/config.json"), u8"游戏配置加载失败");
    }

    /** @brief 创建窗口和渲染器 */
    void createWindowAndRenderer()
    {
        m_window.reset(SDL_CreateWindow(
            ConfigManager::instance()->basic_template.window_title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            ConfigManager::instance()->basic_template.window_width,
            ConfigManager::instance()->basic_template.window_height,
            SDL_WINDOW_SHOWN
        ));

        initAssert(m_window.get(), u8"游戏创建窗口失败");

        m_renderer.reset(SDL_CreateRenderer(
            m_window.get(),
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE
        ));

        initAssert(m_renderer.get(), u8"创建渲染器失败");
    }

    /** @brief 处理SDL事件 */
    void processEvents()
    {
        while (SDL_PollEvent(&m_event)) {
            switch (m_event.type) {
            case SDL_QUIT:
                m_quit = true;
                break;
            default:
                onInput();
                break;
            }
        }
    }

    /** @brief 处理输入 */
    void onInput()
    {
        static SDL_Point position_center;
        static SDL_Point index_tile_selected;
        static auto* config = ConfigManager::instance();

        switch (m_event.type) {
        case SDL_QUIT:
            m_quit = true;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(config->is_game_over)
                break;

            if (getCursorIndexTile(index_tile_selected, m_event.motion.x, m_event.motion.y)) {
                getSelectedTileCenter(position_center, index_tile_selected);

                if (checkHome(index_tile_selected)) {
                    m_upgrade_panel->setIndexTile(index_tile_selected);
                    m_upgrade_panel->setCenterPosition(position_center);
                    m_upgrade_panel->show();
                }
                else if (canPlaceTower(index_tile_selected)) {
                    m_place_panel->setIndexTile(index_tile_selected);
                    m_place_panel->setCenterPosition(position_center);
                    m_place_panel->show();
                }
            }
            break;
        default:
            break;
        }

        if (!config->is_game_over) {
            m_place_panel->onInput(m_event);
            m_upgrade_panel->onInput(m_event);
        }
    }

    /** @brief 更新游戏状态 */
    void onUpdate(double delta_time)
    {
        static auto* config = ConfigManager::instance();

        if (!config->is_game_over) {
            m_place_panel->onUpdate(m_renderer.get());
            m_upgrade_panel->onUpdate(m_renderer.get());
            m_status_bar.onUpdate(m_renderer.get());
            WaveManager::instance()->onUpdate(delta_time);
            EnemyManager::instance()->onUpdate(delta_time);
            BulletManager::instance()->onUpdate(delta_time);
            TowerManager::instance()->onUpdate(delta_time);
            CoinManager::instance()->onUpdate(delta_time);
        }
    }

    /** @brief 渲染游戏画面 */
    void onRender() 
    {
        static auto* config = ConfigManager::instance();
        static SDL_Rect& rect_dst = config->rect_tile_map;
        SDL_RenderCopy(m_renderer.get(), m_tex_tile_map.get(), nullptr, &rect_dst);

        EnemyManager::instance()->onRender(m_renderer.get());
        BulletManager::instance()->onRender(m_renderer.get());
        TowerManager::instance()->onRender(m_renderer.get());
        CoinManager::instance()->onRender(m_renderer.get());

        if (!config->is_game_over) {
            m_place_panel->onRender(m_renderer.get());
            m_upgrade_panel->onRender(m_renderer.get());
            m_status_bar.onRender(m_renderer.get());
        }
    }

    /** @brief 生成瓦片地图纹理 */
    bool generateTileMapTexture() {
        try {
            // 获取必要的资源和配置
            const auto& config = ConfigManager::instance();
            const auto& map = config->map;
            const auto& tile_map = map.getTileMap();
            auto& rect_tile_map = config->rect_tile_map;

            // 获取tile set纹理
            auto tex_tile_set = ResourceManager::instance()->getTexturePool().find(ResID::Tex_TileSet);
            if (tex_tile_set == ResourceManager::instance()->getTexturePool().end()) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "获取tile set纹理失败");
                return false;
            }

            // 查询tile set尺寸
            int tile_set_width, tile_set_height;
            if (SDL_QueryTexture(tex_tile_set->second, nullptr, nullptr, &tile_set_width, &tile_set_height) < 0) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "查询tile set尺寸失败: %s", SDL_GetError());
                return false;
            }

            // 计算尺寸
            const int num_tile_single_line = static_cast<int>(std::ceil(static_cast<double>(tile_set_width) / TILE_SIZE));
            const int tile_map_width = static_cast<int>(map.getWidth()) * TILE_SIZE;
            const int tile_map_height = static_cast<int>(map.getHeight()) * TILE_SIZE;

            // 创建tile map纹理
            m_tex_tile_map.reset(SDL_CreateTexture(
                m_renderer.get(),
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_TARGET,
                tile_map_width,
                tile_map_height
            ));

            if (!m_tex_tile_map) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "创建tile map纹理失败: %s", SDL_GetError());
                return false;
            }

            // 设置纹理位置
            const auto& basic_template = config->basic_template;
            rect_tile_map = {
                (basic_template.window_width - tile_map_width) / 2,
                (basic_template.window_height - tile_map_height) / 2,
                tile_map_width,
                tile_map_height
            };

            // 设置混合模式
            if (SDL_SetTextureBlendMode(m_tex_tile_map.get(), SDL_BLENDMODE_BLEND) < 0) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "设置混合模式失败: %s", SDL_GetError());
                return false;
            }

            // 设置渲染目标
            if (SDL_SetRenderTarget(m_renderer.get(), m_tex_tile_map.get()) < 0) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "设置渲染目标失败: %s", SDL_GetError());
                return false;
            }

            // 渲染tile map
            SDL_Rect rect_src, rect_dst;
            for (int y = 0; y < map.getHeight(); ++y) {
                for (int x = 0; x < map.getWidth(); ++x) {
                    const auto& tile = tile_map[y][x];

                    // 设置目标矩形(在循环外计算不变的部分)
                    rect_dst = {
                        x * TILE_SIZE,
                        y * TILE_SIZE,
                        TILE_SIZE,
                        TILE_SIZE
                    };

                    // 渲染地形
                    rect_src = {
                        (tile.terrian % num_tile_single_line) * TILE_SIZE,
                        (tile.terrian / num_tile_single_line) * TILE_SIZE,
                        TILE_SIZE,
                        TILE_SIZE
                    };

                    if (SDL_RenderCopy(m_renderer.get(), tex_tile_set->second, &rect_src, &rect_dst) < 0) {
                        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "渲染地形失败: %s", SDL_GetError());
                        return false;
                    }

                    // 渲染装饰
                    if (tile.decoration >= 0) {
                        rect_src = {
                            (tile.decoration % num_tile_single_line) * TILE_SIZE,
                            (tile.decoration / num_tile_single_line) * TILE_SIZE,
                            TILE_SIZE,
                            TILE_SIZE
                        };

                        if (SDL_RenderCopy(m_renderer.get(), tex_tile_set->second, &rect_src, &rect_dst) < 0) {
                            SDL_LogError(SDL_LOG_CATEGORY_RENDER, "渲染装饰失败: %s", SDL_GetError());
                            return false;
                        }
                    }
                }
            }

            // 渲染home标记
            const auto& index_home = map.getIndexHome();
            rect_dst = {
                index_home.x * TILE_SIZE,
                index_home.y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
            };

            auto tex_home = ResourceManager::instance()->getTexturePool().find(ResID::Tex_Home);
            if (tex_home == ResourceManager::instance()->getTexturePool().end()) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "获取home标记失败");
                return false;
            }

            if (SDL_RenderCopy(m_renderer.get(), tex_home->second, nullptr, &rect_dst) < 0) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "渲染home标记失败: %s", SDL_GetError());
                return false;
            }

            // 恢复默认渲染目标
            if (SDL_SetRenderTarget(m_renderer.get(), nullptr) < 0) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "恢复默认渲染目标失败: %s", SDL_GetError());
                return false;
            }

            return true;
        }
        catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "生成瓦片地图纹理失败: %s", e.what());
            return false;
        }
    }

    /** @brief 是否在home位置
      * @param index_tile_selected 瓦片坐标 
      */
    bool checkHome(const SDL_Point& index_tile_selected)
    {
        static const auto& map = ConfigManager::instance()->map;
        static const auto& index_home = map.getIndexHome();

        return (index_tile_selected.x == index_home.x && index_tile_selected.y == index_home.y);
    }

    /** @brief 获取鼠标选中的瓦片坐标 
      * @param index_tile_selected 瓦片坐标
      * @param screen_x 鼠标屏幕坐标x
      * @param screen_y 鼠标屏幕坐标y
      * @return 是否成功获取
      */
    bool getCursorIndexTile(SDL_Point& index_tile_selected, int screen_x, int screen_y) const
    {
        static const auto& map = ConfigManager::instance()->map;
        static const auto& rect_tile_map = ConfigManager::instance()->rect_tile_map;

        if(screen_x < rect_tile_map.x || screen_x > rect_tile_map.x + rect_tile_map.w ||
            screen_y < rect_tile_map.y || screen_y > rect_tile_map.y + rect_tile_map.h)
            return false;

        index_tile_selected.x = std::min((screen_x - rect_tile_map.x) / TILE_SIZE, (int)map.getWidth() - 1);
        index_tile_selected.y = std::min((screen_y - rect_tile_map.y) / TILE_SIZE, (int)map.getHeight() - 1);

        return true;
    }

    /** @brief 是否可以放置塔 
      * @param index_tile_selected 瓦片坐标
      * @return 是否可以放置
      */
    bool canPlaceTower(const SDL_Point& index_tile_selected) const
    {
        static const auto& map = ConfigManager::instance()->map;
        const auto& tile_map = map.getTileMap();
        const auto& tile = tile_map[index_tile_selected.y][index_tile_selected.x];

        return (tile.decoration < 0 && tile.direction == Tile::Direction::NONE && !tile.has_tower);
    }

    /** @brief 获取选中瓦片中心坐标 
      * @param position 瓦片中心坐标
      * @param index_tile_selected 瓦片坐标
      */
    void getSelectedTileCenter(SDL_Point& position, const SDL_Point& index_tile_selected) const
    {
        static const auto& rect_tile_map = ConfigManager::instance()->rect_tile_map;

        position.x = rect_tile_map.x + index_tile_selected.x * TILE_SIZE + TILE_SIZE / 2;
        position.y = rect_tile_map.y + index_tile_selected.y * TILE_SIZE + TILE_SIZE / 2;
    }
};