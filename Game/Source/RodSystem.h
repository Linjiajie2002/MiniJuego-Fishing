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

public:
	void castingline(FISHINGTYPE type);
	void ani_castingline(Direction direction);
	void selectFishingtype();
	void hooked(int player_click_count);
	void GamePlaye(Fishlevel fishleve);
	void fishingOver();
	void resetProbability();
	void changeProbability(double nothing, double trash, double small, double medium, double big);
	int getRandomNumber(int min, int max);

public:

	pugi::xml_node config;
	uint texW, texH;

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
	PhysBody* floatbody = nullptr;
	//fishingType
	FISHINGTYPE fishingtype;
	bool changefishingtype = false;

	//StartFishing
	Timer timeFishing;
	int lotteryrandomNum;
	bool thistimehooked = true;
	bool ishooked = false;
	bool playerGoplay = false;
	bool playerGoplay_TimeOver = false;
	Fishlevel selected_prize;

	//GamePlay
	Timer gamePlayTimeLimit;
	uint32 gamePlayTimeLimit_show = 0;
	bool gamePlayStart;
	int player_click_count;
	int player_click_count_TimeOver;
	bool dialogoautoclose = false;
	bool dialogoPlayerMoving = false;
	int dialogoTimeCount;
	bool fishingEndCloseDialogo = false;
	bool isEnd = false;
	int gamePlayTime;

	//gailv
	
};
#endif // __ITEM_H__#pragma once
#pragma once
