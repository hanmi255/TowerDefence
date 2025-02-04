#pragma once

#include "enemy.hpp"
#include "config_manager.hpp"
#include "resource_manager.hpp"

#include <vector>

class SkeletonEnemy : public Enemy
{
public:
    SkeletonEnemy()
    {
        static const auto& texture_pool = ResourceManager::instance()->getTexturePool();
        static SDL_Texture* tex_skeleton = texture_pool.find(ResID::Tex_Skeleton)->second;
        static SDL_Texture* tex_skeleton_sketch = texture_pool.find(ResID::Tex_SkeletonSketch)->second;
        static auto& skeleton_template = ConfigManager::instance()->skeleton_template;

        static const std::vector<int> index_list_down = { 0, 1, 2, 3, 4 };
        static const std::vector<int> index_list_up = { 5, 6, 7, 8, 9 };
        static const std::vector<int> index_list_right = { 10, 11, 12, 13, 14 };
        static const std::vector<int> index_list_left = { 15, 16, 17, 18, 19 };

        anim_up.setLoop(true);
        anim_up.setInterval(0.15);
        anim_up.setFrameData(tex_skeleton, 5, 4, index_list_up);

        anim_down.setLoop(true);
        anim_down.setInterval(0.15);
        anim_down.setFrameData(tex_skeleton, 5, 4, index_list_down);

        anim_left.setLoop(true);
        anim_left.setInterval(0.15);
        anim_left.setFrameData(tex_skeleton, 5, 4, index_list_left);

        anim_right.setLoop(true);
        anim_right.setInterval(0.15);
        anim_right.setFrameData(tex_skeleton, 5, 4, index_list_right);

        anim_up_sketch.setLoop(true);
        anim_up_sketch.setInterval(0.15);
        anim_up_sketch.setFrameData(tex_skeleton_sketch, 5, 4, index_list_up);

        anim_down_sketch.setLoop(true);
        anim_down_sketch.setInterval(0.15);
        anim_down_sketch.setFrameData(tex_skeleton_sketch, 5, 4, index_list_down);

        anim_left_sketch.setLoop(true);
        anim_left_sketch.setInterval(0.15);
        anim_left_sketch.setFrameData(tex_skeleton_sketch, 5, 4, index_list_left);

        anim_right_sketch.setLoop(true);
        anim_right_sketch.setInterval(0.15);
        anim_right_sketch.setFrameData(tex_skeleton_sketch, 5, 4, index_list_right);

        max_hp = skeleton_template.hp;
        max_speed = skeleton_template.speed;
        damage = skeleton_template.damage;
        reward_ratio = skeleton_template.reward_ratio;
        recover_interval = skeleton_template.recover_interval;
        recover_range = skeleton_template.recover_range;
        recover_intensity = skeleton_template.recover_intensity;

        size.x = 48, size.y = 48;
        hp = max_hp;
        speed = max_speed;
    }

    ~SkeletonEnemy() = default;
};
