#pragma once

#include "manager.hpp"

#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <unordered_map>

/**
 * @enum ResID
 * @brief 资源ID枚举类，用于标识所有游戏资源
 */
enum class ResID
{
	Tex_TileSet,

	Tex_Player,
	Tex_Archer,
	Tex_Axeman,
	Tex_Gunner,

	Tex_Slime,
	Tex_KingSlime,
	Tex_Skeleton,
	Tex_Goblin,
	Tex_GoblinPriest,
	Tex_SlimeSketch,
	Tex_KingSlimeSketch,
	Tex_SkeletonSketch,
	Tex_GoblinSketch,
	Tex_GoblinPriestSketch,

	Tex_BulletArrow,
	Tex_BulletAxe,
	Tex_BulletShell,

	Tex_Coin,
	Tex_Home,

	Tex_EffectFlash_Up,
	Tex_EffectFlash_Down,
	Tex_EffectFlash_Left,
	Tex_EffectFlash_Right,
	Tex_EffectImpact_Up,
	Tex_EffectImpact_Down,
	Tex_EffectImpact_Left,
	Tex_EffectImpact_Right,
	Tex_EffectExplode,

	Tex_UISelectCursor,
	Tex_UIPlaceIdle,
	Tex_UIPlaceHoveredTop,
	Tex_UIPlaceHoveredLeft,
	Tex_UIPlaceHoveredRight,
	Tex_UIUpgradeIdle,
	Tex_UIUpgradeHoveredTop,
	Tex_UIUpgradeHoveredLeft,
	Tex_UIUpgradeHoveredRight,
	Tex_UIHomeAvatar,
	Tex_UIPlayerAvatar,
	Tex_UIHeart,
	Tex_UICoin,
	Tex_UIGameOverBar,
	Tex_UIWinText,
	Tex_UILossText,

	Sound_ArrowFire_1,
	Sound_ArrowFire_2,
	Sound_AxeFire,
	Sound_ShellFire,
	Sound_ArrowHit_1,
	Sound_ArrowHit_2,
	Sound_ArrowHit_3,
	Sound_AxeHit_1,
	Sound_AxeHit_2,
	Sound_AxeHit_3,
	Sound_ShellHit,

	Sound_Flash,
	Sound_Impact,

	Sound_Coin,
	Sound_HomeHurt,
	Sound_PlaceTower,
	Sound_TowerLevelUp,

	Sound_Win,
	Sound_Loss,

	Music_BGM,

	Font_Main
};

/**
 * @brief 游戏资源管理器，负责加载和管理所有游戏资源
 *
 * 使用单例模式实现，管理游戏中的纹理、音效、音乐和字体资源
 */
class ResourceManager : public Manager<ResourceManager>
{
	friend class Manager<ResourceManager>;

	template <typename ResourceType>
	using ResourcePool = std::unordered_map<ResID, ResourceType *>;

public:
	using TexturePool = ResourcePool<SDL_Texture>;
	using SoundPool = ResourcePool<Mix_Chunk>;
	using MusicPool = ResourcePool<Mix_Music>;
	using FontPool = ResourcePool<TTF_Font>;

public:
	const TexturePool &getTexturePool() const { return m_texturePool; }
	const SoundPool &getSoundPool() const { return m_soundPool; }
	const MusicPool &getMusicPool() const { return m_musicPool; }
	const FontPool &getFontPool() const { return m_fontPool; }

	/**
	 * @brief 从文件加载所有游戏资源
	 * @param renderer SDL渲染器指针
	 * @return 加载是否成功
	 */
	bool loadFromFile(SDL_Renderer *renderer)
	{
		m_texturePool[ResID::Tex_TileSet] = IMG_LoadTexture(renderer, "res/image/tileset.png");

		m_texturePool[ResID::Tex_Player] = IMG_LoadTexture(renderer, "res/image/player.png");
		m_texturePool[ResID::Tex_Archer] = IMG_LoadTexture(renderer, "res/image/tower_archer.png");
		m_texturePool[ResID::Tex_Axeman] = IMG_LoadTexture(renderer, "res/image/tower_axeman.png");
		m_texturePool[ResID::Tex_Gunner] = IMG_LoadTexture(renderer, "res/image/tower_gunner.png");

		m_texturePool[ResID::Tex_Slime] = IMG_LoadTexture(renderer, "res/image/enemy_slime.png");
		m_texturePool[ResID::Tex_KingSlime] = IMG_LoadTexture(renderer, "res/image/enemy_king_slime.png");
		m_texturePool[ResID::Tex_Skeleton] = IMG_LoadTexture(renderer, "res/image/enemy_skeleton.png");
		m_texturePool[ResID::Tex_Goblin] = IMG_LoadTexture(renderer, "res/image/enemy_goblin.png");
		m_texturePool[ResID::Tex_GoblinPriest] = IMG_LoadTexture(renderer, "res/image/enemy_goblin_priest.png");
		m_texturePool[ResID::Tex_SlimeSketch] = IMG_LoadTexture(renderer, "res/image/enemy_slime_sketch.png");
		m_texturePool[ResID::Tex_KingSlimeSketch] = IMG_LoadTexture(renderer, "res/image/enemy_king_slime_sketch.png");
		m_texturePool[ResID::Tex_SkeletonSketch] = IMG_LoadTexture(renderer, "res/image/enemy_skeleton_sketch.png");
		m_texturePool[ResID::Tex_GoblinSketch] = IMG_LoadTexture(renderer, "res/image/enemy_goblin_sketch.png");
		m_texturePool[ResID::Tex_GoblinPriestSketch] = IMG_LoadTexture(renderer, "res/image/enemy_goblin_priest_sketch.png");

		m_texturePool[ResID::Tex_BulletArrow] = IMG_LoadTexture(renderer, "res/image/bullet_arrow.png");
		m_texturePool[ResID::Tex_BulletAxe] = IMG_LoadTexture(renderer, "res/image/bullet_axe.png");
		m_texturePool[ResID::Tex_BulletShell] = IMG_LoadTexture(renderer, "res/image/bullet_shell.png");

		m_texturePool[ResID::Tex_Coin] = IMG_LoadTexture(renderer, "res/image/coin.png");
		m_texturePool[ResID::Tex_Home] = IMG_LoadTexture(renderer, "res/image/home.png");

		m_texturePool[ResID::Tex_EffectFlash_Up] = IMG_LoadTexture(renderer, "res/image/effect_flash_up.png");
		m_texturePool[ResID::Tex_EffectFlash_Down] = IMG_LoadTexture(renderer, "res/image/effect_flash_down.png");
		m_texturePool[ResID::Tex_EffectFlash_Left] = IMG_LoadTexture(renderer, "res/image/effect_flash_left.png");
		m_texturePool[ResID::Tex_EffectFlash_Right] = IMG_LoadTexture(renderer, "res/image/effect_flash_right.png");
		m_texturePool[ResID::Tex_EffectImpact_Up] = IMG_LoadTexture(renderer, "res/image/effect_impact_up.png");
		m_texturePool[ResID::Tex_EffectImpact_Down] = IMG_LoadTexture(renderer, "res/image/effect_impact_down.png");
		m_texturePool[ResID::Tex_EffectImpact_Left] = IMG_LoadTexture(renderer, "res/image/effect_impact_left.png");
		m_texturePool[ResID::Tex_EffectImpact_Right] = IMG_LoadTexture(renderer, "res/image/effect_impact_right.png");
		m_texturePool[ResID::Tex_EffectExplode] = IMG_LoadTexture(renderer, "res/image/effect_explode.png");

		m_texturePool[ResID::Tex_UISelectCursor] = IMG_LoadTexture(renderer, "res/image/ui_select_cursor.png");
		m_texturePool[ResID::Tex_UIPlaceIdle] = IMG_LoadTexture(renderer, "res/image/ui_place_idle.png");
		m_texturePool[ResID::Tex_UIPlaceHoveredTop] = IMG_LoadTexture(renderer, "res/image/ui_place_hovered_top.png");
		m_texturePool[ResID::Tex_UIPlaceHoveredLeft] = IMG_LoadTexture(renderer, "res/image/ui_place_hovered_left.png");
		m_texturePool[ResID::Tex_UIPlaceHoveredRight] = IMG_LoadTexture(renderer, "res/image/ui_place_hovered_right.png");
		m_texturePool[ResID::Tex_UIUpgradeIdle] = IMG_LoadTexture(renderer, "res/image/ui_upgrade_idle.png");
		m_texturePool[ResID::Tex_UIUpgradeHoveredTop] = IMG_LoadTexture(renderer, "res/image/ui_upgrade_hovered_top.png");
		m_texturePool[ResID::Tex_UIUpgradeHoveredLeft] = IMG_LoadTexture(renderer, "res/image/ui_upgrade_hovered_left.png");
		m_texturePool[ResID::Tex_UIUpgradeHoveredRight] = IMG_LoadTexture(renderer, "res/image/ui_upgrade_hovered_right.png");
		m_texturePool[ResID::Tex_UIHomeAvatar] = IMG_LoadTexture(renderer, "res/image/ui_home_avatar.png");
		m_texturePool[ResID::Tex_UIPlayerAvatar] = IMG_LoadTexture(renderer, "res/image/ui_player_avatar.png");
		m_texturePool[ResID::Tex_UIHeart] = IMG_LoadTexture(renderer, "res/image/ui_heart.png");
		m_texturePool[ResID::Tex_UICoin] = IMG_LoadTexture(renderer, "res/image/ui_coin.png");
		m_texturePool[ResID::Tex_UIGameOverBar] = IMG_LoadTexture(renderer, "res/image/ui_game_over_bar.png");
		m_texturePool[ResID::Tex_UIWinText] = IMG_LoadTexture(renderer, "res/image/ui_win_text.png");
		m_texturePool[ResID::Tex_UILossText] = IMG_LoadTexture(renderer, "res/image/ui_loss_text.png");

		for (const auto &pair : m_texturePool)
			if (!pair.second)
				return false;

		m_soundPool[ResID::Sound_ArrowFire_1] = Mix_LoadWAV("res/music/sound_arrow_fire_1.mp3");
		m_soundPool[ResID::Sound_ArrowFire_2] = Mix_LoadWAV("res/music/sound_arrow_fire_2.mp3");
		m_soundPool[ResID::Sound_AxeFire] = Mix_LoadWAV("res/music/sound_axe_fire.wav");
		m_soundPool[ResID::Sound_ShellFire] = Mix_LoadWAV("res/music/sound_shell_fire.wav");
		m_soundPool[ResID::Sound_ArrowHit_1] = Mix_LoadWAV("res/music/sound_arrow_hit_1.mp3");
		m_soundPool[ResID::Sound_ArrowHit_2] = Mix_LoadWAV("res/music/sound_arrow_hit_2.mp3");
		m_soundPool[ResID::Sound_ArrowHit_3] = Mix_LoadWAV("res/music/sound_arrow_hit_3.mp3");
		m_soundPool[ResID::Sound_AxeHit_1] = Mix_LoadWAV("res/music/sound_axe_hit_1.mp3");
		m_soundPool[ResID::Sound_AxeHit_2] = Mix_LoadWAV("res/music/sound_axe_hit_2.mp3");
		m_soundPool[ResID::Sound_AxeHit_3] = Mix_LoadWAV("res/music/sound_axe_hit_3.mp3");
		m_soundPool[ResID::Sound_ShellHit] = Mix_LoadWAV("res/music/sound_shell_hit.mp3");

		m_soundPool[ResID::Sound_Flash] = Mix_LoadWAV("res/music/sound_flash.wav");
		m_soundPool[ResID::Sound_Impact] = Mix_LoadWAV("res/music/sound_impact.wav");

		m_soundPool[ResID::Sound_Coin] = Mix_LoadWAV("res/music/sound_coin.mp3");
		m_soundPool[ResID::Sound_HomeHurt] = Mix_LoadWAV("res/music/sound_home_hurt.wav");
		m_soundPool[ResID::Sound_PlaceTower] = Mix_LoadWAV("res/music/sound_place_tower.mp3");
		m_soundPool[ResID::Sound_TowerLevelUp] = Mix_LoadWAV("res/music/sound_tower_level_up.mp3");

		m_soundPool[ResID::Sound_Win] = Mix_LoadWAV("res/music/sound_win.wav");
		m_soundPool[ResID::Sound_Loss] = Mix_LoadWAV("res/music/sound_loss.mp3");

		for (const auto &pair : m_soundPool)
			if (!pair.second)
				return false;

		m_musicPool[ResID::Music_BGM] = Mix_LoadMUS("res/music/music_bgm.mp3");

		for (const auto &pair : m_musicPool)
			if (!pair.second)
				return false;

		m_fontPool[ResID::Font_Main] = TTF_OpenFont("res/font/ipix.ttf", 25);

		for (const auto &pair : m_fontPool)
			if (!pair.second)
				return false;

		return true;
	}

protected:
	ResourceManager() = default;
	~ResourceManager() = default;

private:
	TexturePool m_texturePool; // 纹理资源池
	SoundPool m_soundPool;	   // 音效资源池
	MusicPool m_musicPool;	   // 音乐资源池
	FontPool m_fontPool;	   // 字体资源池
};
