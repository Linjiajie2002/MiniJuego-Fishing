#pragma once
#ifndef __RODSYSTEM_H__
#define __RODSYSTEM_H__


#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "SDL/include/SDL.h"
#include "Timer.h"

#include "List.h"

struct SDL_Texture;

class RodSystem : public Entity
{
public:

	RodSystem();
	virtual ~RodSystem();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

private:
	void castingline(FISHINGTYPE type);
	void ani_castingline(Direction direction);
	void selectFishingtype();

public:

	pugi::xml_node config;

	Direction player_Direction;
	//Animacion
	Animation* currentAnimation = nullptr;
	SDL_Rect rect;
	int TSprite;
	int SpriteX;
	int SpriteY;
	int PhotoWeight;
	const char* Path;
	SDL_Texture* texture = NULL;

	Fishing fishing;

	//fishingfloat
	const char* fishingfloat_path;
	SDL_Texture* fishingfloat_texture = NULL;
	float fishingflota_position_x;
	float fishingflota_position_y;
	float fishingflota_CenterX;
	float fishingflota_CenterY;
	bool fishingfloat_lineReady = false;
	bool fishingfloat_getPlayerPosition = true;;
	bool isFishingta = false;
	bool crearfloatbody = false;
	PhysBody* floatbody;
	//fishingType
	FISHINGTYPE fishingtype;
	bool changefishingtype = false;

};
#endif // __ITEM_H__#pragma once
#pragma once
