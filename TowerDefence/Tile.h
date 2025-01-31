#pragma once

#include <vector>

constexpr auto TILE_SIZE = 48;                  // 瓦片大小(像素)

struct Tile
{
    enum class Direction                        // 地图瓦片的方向枚举
    {
        NONE = 0,                               // 无方向
        UP,                                     // 向上
        DOWN,                                   // 向下
        LEFT,                                   // 向左
        RIGHT                                   // 向右
    };

    int terrian = 0;                            // 地形类型ID(0表示默认地形)
    int decoration = -1;                        // 装饰物ID(-1表示无装饰)
    int special_flag = -1;                      // 特殊标记ID(-1表示无特殊标记)


    Direction direction = Direction::NONE;      // 瓦片朝向(默认无方向)
};

using TileMap = std::vector<std::vector<Tile>>; // 二维瓦片地图类型别名
