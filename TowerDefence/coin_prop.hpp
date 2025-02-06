#pragma once

#include "vector2.hpp"
#include "timer.hpp"
#include "tile.hpp"
#include "resource_manager.hpp"

#include <SDL.h>

class CoinProp
{
public:
	CoinProp()
	{
		timer_jump.setOneShot(true);
		timer_jump.setWaitTime(interval_jump);
		timer_jump.setOnTimeOut([&]() { is_jumping = false; });

		timer_disappear.setOneShot(true);
		timer_disappear.setWaitTime(interval_disappear);
		timer_disappear.setOnTimeOut([&]() { is_valid = false; });

		velocity.x = (rand() % 2 ? 1 : -1) * 2 * TILE_SIZE;
		velocity.y = -3 * TILE_SIZE;
	}

	~CoinProp() = default;

	void setPosition(const Vector2& position)
	{ 
		this->position = position;
	}

	const Vector2& getPosition() const 
	{ 
		return position; 
	}

	const Vector2& getSize() const 
	{ 
		return size;
	}

	void makeInvalid()
	{ 
		is_valid = false; 
	}

	bool canRemove() const 
	{ 
		return !is_valid; 
	}

	void onUpdate(double delta_time)
	{
		timer_jump.onUpdate(delta_time);
		timer_disappear.onUpdate(delta_time);

		if (is_jumping) 
			velocity.y += gravity * delta_time;
		else {
			velocity.x = 0;
			velocity.y = sin(SDL_GetTicks64() / 1000.0 * 4) * 30;
		}

		position += velocity * delta_time;
	}

	void onRender(SDL_Renderer* renderer) const
	{
		static SDL_Rect rect = { 0, 0, (int)size.x, (int)size.y };
		static auto* tex_coin = ResourceManager::instance()->getTexturePool().find(ResID::Tex_Coin)->second;

		rect.x = (int)(position.x - size.x / 2);
		rect.y = (int)(position.y - size.y / 2);

		SDL_RenderCopy(renderer, tex_coin, nullptr, &rect);
	}
private:
	Vector2 position;
	Vector2 velocity;
	Vector2 size = { 16, 16 };

	Timer timer_jump;
	Timer timer_disappear;

	bool is_valid = true;
	bool is_jumping = true;

	double gravity = 490;
	double interval_jump = 0.75;
	double interval_disappear = 5.0;
};

