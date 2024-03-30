#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Fishing.h"
#include "FishingManager.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: DONE 2: Initialize Player parameters
	position = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());

	Path = config.attribute("texturepath").as_string();
	TSprite = config.attribute("Tsprite").as_int();
	SpriteX = config.attribute("ani_x").as_int();
	SpriteY = config.attribute("ani_y").as_int();
	PhotoWeight = config.attribute("Pweight").as_int();
	spritePositions = SPosition.SpritesPos(TSprite, SpriteX, SpriteY, PhotoWeight);

	idle_D.LoadAnim("player", "idle_D", spritePositions);
	idle_U.LoadAnim("player", "idle_U", spritePositions);
	idle_L.LoadAnim("player", "idle_L", spritePositions);
	idle_R.LoadAnim("player", "idle_R", spritePositions);
	walk_D.LoadAnim("player", "walk_D", spritePositions);
	walk_U.LoadAnim("player", "walk_U", spritePositions);
	walk_L.LoadAnim("player", "walk_L", spritePositions);
	walk_R.LoadAnim("player", "walk_R", spritePositions);


	return true;
}

bool Player::Start() {

	texture = app->tex->Load(Path);

	pbody = app->physics->CreateCircle(position.x, position.y, 30, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	currentAnimation = &idle_D;
	player_Direction = Direction::UNKNOWN;
	printf("Path:%s\nTSprite:%d\nSpriteX:%d\nSpriteY:%d\nPhotoWeight:%d\n", Path, TSprite, SpriteX, SpriteY, PhotoWeight);


	return true;
}

bool Player::Update(float dt)
{

	if (player_Direction == Direction::UP) { currentAnimation = &idle_U; }
	else if (player_Direction == Direction::DOWN) { currentAnimation = &idle_D; }
	else if (player_Direction == Direction::LEFT) { currentAnimation = &idle_L; }
	else if (player_Direction == Direction::RIGHT) { currentAnimation = &idle_R; }
	else {
		currentAnimation = &idle_D;
	}

	
	b2Vec2 velocity = b2Vec2(0, 0);
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		velocity.y += -0.2 * dt;
		currentAnimation = &walk_U;
		player_Direction = Direction::UP;

		
		if (app->fishingManager->fishing.isFishing) { playermove = true; }
		else { playermove = false; }
	}

	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		velocity.y += 0.2 * dt;
		currentAnimation = &walk_D;
		player_Direction = Direction::DOWN;
		if (app->fishingManager->fishing.isFishing) { playermove = true; }
		else { playermove = false; }
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -0.2 * dt;
		currentAnimation = &walk_L;
		player_Direction = Direction::LEFT;
		if (app->fishingManager->fishing.isFishing) { playermove = true; }
		else { playermove = false; }
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = 0.2 * dt;
		currentAnimation = &walk_R;
		player_Direction = Direction::RIGHT;
		if (app->fishingManager->fishing.isFishing) { playermove = true; }
		else { playermove = false; }
	}


	pbody->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.x = METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2;
	position.y = METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2;


	rect = currentAnimation->GetCurrentFrame();
	currentAnimation->Update();
	app->render->DrawTexture(texture, position.x - 70, position.y - 70, 3, SDL_FLIP_NONE, &rect);
	

	return true;
}

bool Player::CleanUp()
{
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}