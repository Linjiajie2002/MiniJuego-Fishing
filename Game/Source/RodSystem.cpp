#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Timer.h"
#include "Map.h"
#include "RodSystem.h"

#include "Player.h"
#include "App.h"
#include "Fishing.h"
#include "FishingManager.h"


RodSystem::RodSystem() : Entity(EntityType::ROD)
{
	name.Create("rodsystem");

}

RodSystem::~RodSystem() {}

bool RodSystem::Awake() {


	fishingfloat_path = parameters.child("fishingfloat").attribute("texturepath").as_string();

	return true;
}

bool RodSystem::Start() {

	fishingfloat_texture = app->tex->Load(fishingfloat_path);
	return true;
}

bool RodSystem::Update(float dt)
{


	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		fishing.isFishing = !fishing.isFishing;
		isFishingta = fishing.isFishing;

		if (!fishing.isFishing) {

			fishingfloat_getPlayerPosition = true;
		}
		castingline(fishingtype);
	}
	if (fishingfloat_lineReady) {

		ani_castingline(app->scene->GetPlayer()->player_Direction);
	}



	if (app->scene->GetPlayer()->playermove) {
		fishing.isFishing = false;
		if (!fishing.isFishing) {
			fishingfloat_getPlayerPosition = true;
		}
		castingline(fishingtype);
	}


	return true;
}


bool RodSystem::CleanUp()
{


	return true;
}


void RodSystem::castingline(FISHINGTYPE type)
{
	if (fishing.rodReady) {
		if (fishing.isFishing) {
			fishingfloat_lineReady = true;
			crearfloatbody = true;
		}
		else {
			fishingfloat_lineReady = false;
			if (floatbody != nullptr) {
				floatbody->body->GetWorld()->DestroyBody(floatbody->body);
				floatbody = nullptr;
			}
		}
	}
}

void RodSystem::ani_castingline(Direction direction)
{
	if (fishingfloat_getPlayerPosition) {
		fishingflota_position_x = app->scene->GetPlayer()->position.x;
		fishingflota_position_y = app->scene->GetPlayer()->position.y;
		fishingfloat_getPlayerPosition = false;
	}

	if (direction == Direction::UP) { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y - 100; }
	else if (direction == Direction::DOWN) { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y + 100; }
	else if (direction == Direction::LEFT) { fishingflota_CenterX = app->scene->GetPlayer()->position.x - 100; fishingflota_CenterY = app->scene->GetPlayer()->position.y; }
	else if (direction == Direction::RIGHT) { fishingflota_CenterX = app->scene->GetPlayer()->position.x + 100; fishingflota_CenterY = app->scene->GetPlayer()->position.y; }
	else {
		fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y + 100;
	}
	float timeLerp = 0.1f;
	fishingflota_position_x = fishingflota_position_x * (1 - timeLerp) + fishingflota_CenterX * timeLerp;
	fishingflota_position_y = fishingflota_position_y * (1 - timeLerp) + fishingflota_CenterY * timeLerp;

	if (floatbody == nullptr && crearfloatbody) {
		floatbody = app->physics->CreateRectangleSensor(fishingflota_CenterX + 25, fishingflota_CenterY + 23, 20, 20, bodyType::STATIC);
		floatbody->ctype = ColliderType::FLOAT;
		floatbody->body->SetFixedRotation(true);
		crearfloatbody = false;
	}


	app->render->DrawTexture(fishingfloat_texture, fishingflota_position_x, fishingflota_position_y, 3);
}

void RodSystem::selectFishingtype()
{
	if (changefishingtype) {
		fishingtype = FISHINGTYPE::LUREFISHING;
	}
	else
	{
		fishingtype = FISHINGTYPE::FISHING;
	}
}


void RodSystem::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}


