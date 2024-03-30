#pragma once
#ifndef __FISHINGMANAGER_H__
#define __FISHINGMANAGER_H__

#include "Module.h"
#include "Dialog.h"
#include "Fishing.h"
#include "Timer.h"
#include "List.h"
#include <vector>
#include "Player.h"

class FishingManager : public Module
{
public:

	FishingManager();

	// Destructor
	virtual ~FishingManager();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();


private:
	void castingline(FISHINGTYPE type);
	void ani_castingline(Direction direction);
	void selectFishingtype();


public:

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

	//fishingType
	FISHINGTYPE fishingtype;
	bool changefishingtype = false;
	

	
};

#endif // __DIALOGMANAGER_H__
