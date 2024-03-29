#pragma once
#ifndef __FISHINGMANAGER_H__
#define __FISHINGMANAGER_H__

#include "Module.h"
#include "Dialog.h"
#include "Fishing.h"
#include "Timer.h"
#include "List.h"
#include <vector>

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


public:
	Animation trash;
	Animation small;
	Animation medium;
	Animation big;
	Animation fishingfloat;

	Animation SPosition;
	SDL_Rect* spritePositions;

	Direction player_Direction;
	Animation* currentAnimation = nullptr;
	SDL_Rect rect;
	int TSprite;
	int SpriteX;
	int SpriteY;
	int PhotoWeight;
	const char* Path;
	SDL_Texture* texture = NULL;

	Fishing fishing;
	
};

#endif // __DIALOGMANAGER_H__
