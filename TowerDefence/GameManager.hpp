#pragma once

#include "Manager.h"

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
            update(delta_time);
            render();
        }

        return 0;
    }

protected:
    GameManager() 
    {
        initializeSDL();
        createWindowAndRenderer();
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

    static constexpr int WINDOW_WIDTH = 1280;
    static constexpr int WINDOW_HEIGHT = 720;

    // SDL初始化
    void initializeSDL() 
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            throw std::runtime_error("SDL初始化失败: " + std::string(SDL_GetError()));
        }

        if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
            throw std::runtime_error("SDL_image初始化失败: " + std::string(IMG_GetError()));
        }

        if (!(Mix_Init(MIX_INIT_MP3) & MIX_INIT_MP3)) {
            throw std::runtime_error("SDL_mixer初始化失败: " + std::string(Mix_GetError()));
        }

        if (TTF_Init() < 0) {
            throw std::runtime_error("SDL_ttf初始化失败: " + std::string(TTF_GetError()));
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            throw std::runtime_error("音频设备初始化失败: " + std::string(Mix_GetError()));
        }

        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    }

    // 创建窗口和渲染器
    void createWindowAndRenderer()
    {
        m_window.reset(SDL_CreateWindow(
            u8"Tower Defence",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN
        ));

        if (!m_window) {
            throw std::runtime_error("创建窗口失败: " + std::string(SDL_GetError()));
        }

        m_renderer.reset(SDL_CreateRenderer(
            m_window.get(),
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE
        ));

        if (!m_renderer) {
            throw std::runtime_error("创建渲染器失败: " + std::string(SDL_GetError()));
        }
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

    // 更新游戏状态
    void update(double delta_time)
    {
        onUpdate(delta_time);
    }

    // 渲染游戏画面
    void render() 
    {
        SDL_SetRenderDrawColor(m_renderer.get(), 0, 0, 0, 255);
        SDL_RenderClear(m_renderer.get());

        onRender();

        SDL_RenderPresent(m_renderer.get());
    }

    // 输入处理（由派生类实现）
    virtual void onInput() {}

    // 更新数据（由派生类实现）
    virtual void onUpdate(double delta_time) {}

    // 渲染（由派生类实现）
    virtual void onRender() {}
};
