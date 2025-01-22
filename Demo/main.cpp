#define SDL_MAIN_HANDLED

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>
#include <cJSON.h>

void testJson()
{
    std::ifstream ifs("test.json");
    if (!ifs.good()) {
        std::cout << "open file failed" << std::endl;
        return;
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    ifs.close();

    cJSON* root = cJSON_Parse(ss.str().c_str());
    if (root == nullptr) {
        std::cout << "parse json failed" << std::endl;
        return;
    }
    cJSON* name = cJSON_GetObjectItem(root, "name");
    cJSON* age = cJSON_GetObjectItem(root, "age");
    cJSON* pets = cJSON_GetObjectItem(root, "pets");

    std::cout << name->string << ": " << name->valuestring << std::endl;
    std::cout << age->string << ": " << age->valueint << std::endl;

    std::cout << pets->string << ": " << std::endl;
    cJSON* petItem = nullptr;
    cJSON_ArrayForEach(petItem, pets)
    {
        std::cout << "\t" << petItem->valuestring << std::endl;
    }
}

void testCSV()
{
    std::ifstream ifs("test.csv");
    if (!ifs.good()) {
        std::cout << "open file failed" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(ifs, line))
    {
        std::string grid;
        std::stringstream ss(line);
        while (std::getline(ss, grid, ','))
        {
            std::cout << grid << " ";
        }
        std::cout << std::endl;
    }

    ifs.close();
}

int main()
{
    testJson();
    testCSV();

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    Mix_Init(MIX_INIT_MP3);
    TTF_Init();

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window* window = SDL_CreateWindow(u8"Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface* surfaceImg = IMG_Load("avatar.jpg");
    SDL_Texture* textureImg = SDL_CreateTextureFromSurface(renderer, surfaceImg);

    TTF_Font* font = TTF_OpenFont("ipix.ttf", 24);
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* surfaceText = TTF_RenderUTF8_Blended(font, u8"你好，世界！", color);
    SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);

    Mix_Music* music = Mix_LoadMUS("music.mp3");
    Mix_FadeInMusic(music, -1, 1500);

    int fps = 60;

    bool isQuit = false;

    SDL_Event event;
    SDL_Point mousePos = { 0, 0 };
    SDL_Rect rectImg = { 0, 0, surfaceImg->w, surfaceImg->h };
    SDL_Rect rectText = { 0, 0, surfaceText->w, surfaceText->h };

    Uint64 lastCounter = SDL_GetPerformanceCounter();
    Uint64 conterFreq = SDL_GetPerformanceFrequency();

    while (!isQuit)
    {

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isQuit = true;
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                mousePos.x = event.motion.x;
                mousePos.y = event.motion.y;
            }
        }

        Uint64 currentCounter = SDL_GetPerformanceCounter();
        double deltaTime = (currentCounter - lastCounter) / (double)conterFreq;
        lastCounter = currentCounter;

        if (deltaTime > 1.0 / fps)
        {
            deltaTime = 1.0 / fps;
        } 
        SDL_Delay((Uint32)(deltaTime * 1000));

        // 处理数据
        rectImg.x = mousePos.x - rectImg.w / 2;
        rectImg.y = mousePos.y - rectImg.h / 2;

        rectText.x = mousePos.x - rectText.w / 2;
        rectText.y = mousePos.y - rectText.h / 2;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 渲染绘图
        SDL_RenderCopy(renderer, textureImg, NULL, &rectImg);
        filledCircleRGBA(renderer, mousePos.x, mousePos.y, 50, 255, 0, 0, 125);
        SDL_RenderCopy(renderer, textureText, NULL, &rectText);

        SDL_RenderPresent(renderer);
    }
    return 0;
}