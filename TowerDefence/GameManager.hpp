﻿#pragma once

#include "Manager.h"
#include "ConfigManager.hpp"
#include "ResourceManager.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <chrono>

// 自定义删除器，用于智能指针管理SDL资源
struct SDLDeleter {
    void operator()(SDL_Window* w) const { SDL_DestroyWindow(w); }
    void operator()(SDL_Renderer* r) const { SDL_DestroyRenderer(r); }
    void operator()(SDL_Texture* t) const { SDL_DestroyTexture(t); }
};

class GameManager : public Manager<GameManager> {
    friend class Manager<GameManager>;

public:
    // 运行游戏
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
            onRender();
        }

        return 0;
    }

protected:
    GameManager() 
    {
        initializeSDL();
        loadConfig();
        createWindowAndRenderer();

        initAssert(ResourceManager::instance()->loadFromFile(m_renderer.get()), "资源管理器初始化失败");
        initAssert(generateTileMapTexture(), "瓦片地图纹理生成失败");
    }

    ~GameManager() 
    {
        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
    }

private:
    SDL_Event m_event;
    bool m_quit = false;

    std::unique_ptr<SDL_Window, SDLDeleter> m_window;
    std::unique_ptr<SDL_Renderer, SDLDeleter> m_renderer;
    std::unique_ptr<SDL_Texture, SDLDeleter> m_tex_tile_map;

private:
    // 初始化断言
    void initAssert(bool flag, const char* errorMessage)
    {
        if (flag) return;

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"游戏启动失败", errorMessage, m_window.get());
        exit(-1);
    }

    // SDL初始化
    void initializeSDL() 
    {
        initAssert(!SDL_Init(SDL_INIT_EVERYTHING), u8"SDL初始化失败");
        initAssert(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG), u8"SDL_image初始化失败");
        initAssert(Mix_Init(MIX_INIT_MP3), u8"SDL_mixer 初始化失败！");
        initAssert(!TTF_Init(), u8"SDL_ttf初始化失败");
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    }

    // 加载配置
    void loadConfig() 
    {
        initAssert(ConfigManager::instance()->map.loadMap("map.csv"), u8"地图加载失败");
        initAssert(ConfigManager::instance()->loadLevelConfig("level.json"), u8"关卡配置加载失败");
        initAssert(ConfigManager::instance()->loadGameConfig("config.json"), u8"游戏配置加载失败");
    }

    // 创建窗口和渲染器
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

    // 事件处理
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

    // 处理输入
    void onInput()
    {
    }
    // 更新游戏状态
    void onUpdate(double delta_time)
    {
    }

    // 渲染游戏画面
    void onRender() 
    {
        static ConfigManager* config = ConfigManager::instance();
        static SDL_Rect& rect_dst = config->rect_tile_map;
        SDL_RenderCopy(m_renderer.get(), m_tex_tile_map.get(), nullptr, &rect_dst);
    }

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
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to find tile set texture");
                return false;
            }

            // 查询tile set尺寸
            int tile_set_width, tile_set_height;
            if (SDL_QueryTexture(tex_tile_set->second, nullptr, nullptr, &tile_set_width, &tile_set_height) < 0) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to query tile set texture: %s", SDL_GetError());
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
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to create tile map texture: %s", SDL_GetError());
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
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to set blend mode: %s", SDL_GetError());
                return false;
            }

            // 设置渲染目标
            if (SDL_SetRenderTarget(m_renderer.get(), m_tex_tile_map.get()) < 0) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to set render target: %s", SDL_GetError());
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
                        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to render terrain: %s", SDL_GetError());
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
                            SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to render decoration: %s", SDL_GetError());
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
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to find home texture");
                return false;
            }

            if (SDL_RenderCopy(m_renderer.get(), tex_home->second, nullptr, &rect_dst) < 0) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to render home: %s", SDL_GetError());
                return false;
            }

            // 恢复默认渲染目标
            if (SDL_SetRenderTarget(m_renderer.get(), nullptr) < 0) {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to reset render target: %s", SDL_GetError());
                return false;
            }

            return true;
        }
        catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Exception in generate_tile_map_texture: %s", e.what());
            return false;
        }
    }
};
