#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "vector"
#include "Physics.h"
#include "Render.h"
#include "Animation.h"
#include "DialogTriggerEntity.h"
//New//
#include "Fishing.h"
struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	// L07 DONE 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	//L02: DONE 2: Declare player parameters
	float speed = 0.2f;
	
	pugi::xml_node config;
	uint texW, texH;

	//Audio fx
	int pickCoinFxId;
	PhysBody* pbody;
	b2Vec2 vel;



	Animation idle_U;
	Animation idle_D;
	Animation idle_L;
	Animation idle_R;
	Animation walk_U;
	Animation walk_D;
	Animation walk_L;
	Animation walk_R;
	Animation SPosition;
	SDL_Rect* spritePositions;


	Animation* currentAnimation = nullptr;
	SDL_Rect rect;
	int TSprite;
	int SpriteX;
	int SpriteY;
	int PhotoWeight;
	const char* Path;
	SDL_Texture* texture = NULL;

	//Fishing
	//New//
	bool playermove = false;
	Direction player_Direction;

	bool contactDialogTrigger = false;
	DialogTrigger* dialogTriggerContact = nullptr;
};

#endif // __PLAYER_H__