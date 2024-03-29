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

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: DONE 2: Initialize Player parameters
	position = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());

	return true;
}

bool Player::Start() {

	texture = app->tex->Load(config.attribute("texturePath").as_string());

	//initialize audio effect
	pickCoinFxId = app->audio->LoadFx(config.attribute("coinfxpath").as_string());

	pbody = app->physics->CreateCircle(position.x, position.y, 20, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, 0);
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		velocity.y += -0.2 * dt;
	}

	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		velocity.y += 0.2 * dt;
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -0.2 * dt;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = 0.2 * dt;
	}
	pbody->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.x = METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2;
	position.y = METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2;

	

	app->render->DrawTexture(texture, position.x-15, position.y-15);

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