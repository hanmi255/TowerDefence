#pragma once

#include "enemy.hpp"
#include "config_manager.hpp"
#include "resource_manager.hpp"

class KingSlimeEnemy : public Enemy
{
public:
    KingSlimeEnemy()
    {
        static const auto& texture_pool = ResourceManager::instance()->getTexturePool();
        static SDL_Texture* tex_king_slime = texture_pool.find(ResID::Tex_KingSlime)->second;
        static SDL_Texture* tex_king_slime_sketch = texture_pool.find(ResID::Tex_KingSlimeSketch)->second;
        static auto& king_slime_template = ConfigManager::instance()->king_slime_template;

        static const std::vector<int> index_list_down = { 0, 1, 2, 3, 4, 5 };
        static const std::vector<int> index_list_up = { 6, 7, 8, 9, 10, 11 };
        static const std::vector<int> index_list_right = { 12, 13, 14, 15, 16, 17 };
        static const std::vector<int> index_list_left = { 18, 19, 20, 21, 22, 23 };

        anim_up.setLoop(true);
        anim_up.setInterval(0.1);
        anim_up.setFrameData(tex_king_slime, 6, 4, index_list_up);

        anim_down.setLoop(true);
        anim_down.setInterval(0.1);
        anim_down.setFrameData(tex_king_slime, 6, 4, index_list_down);

        anim_left.setLoop(true);
        anim_left.setInterval(0.1);
        anim_left.setFrameData(tex_king_slime, 6, 4, index_list_left);

        anim_right.setLoop(true);
        anim_right.setInterval(0.1);
        anim_right.setFrameData(tex_king_slime, 6, 4, index_list_right);

        anim_up_sketch.setLoop(true);
        anim_up_sketch.setInterval(0.1);
        anim_up_sketch.setFrameData(tex_king_slime_sketch, 6, 4, index_list_up);

        anim_down_sketch.setLoop(true);
        anim_down_sketch.setInterval(0.1);
        anim_down_sketch.setFrameData(tex_king_slime_sketch, 6, 4, index_list_down);

        anim_left_sketch.setLoop(true);
        anim_left_sketch.setInterval(0.1);
        anim_left_sketch.setFrameData(tex_king_slime_sketch, 6, 4, index_list_left);

        anim_right_sketch.setLoop(true);
        anim_right_sketch.setInterval(0.1);
        anim_right_sketch.setFrameData(tex_king_slime_sketch, 6, 4, index_list_right);

        max_hp = king_slime_template.hp;
        max_speed = king_slime_template.speed;
        damage = king_slime_template.damage;
        reward_ratio = king_slime_template.reward_ratio;
        recover_interval = king_slime_template.recover_interval;
        recover_range = king_slime_template.recover_range;
        recover_intensity = king_slime_template.recover_intensity;

        size.x = 48, size.y = 48;
        hp = max_hp;
        speed = max_speed;
    }

    ~KingSlimeEnemy() = default;
};

