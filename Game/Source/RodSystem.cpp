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
#include <map>
#include <random>

std::map<Fishlevel, double> prizes = {
	{Fishlevel::NOTHING, 0.4},
	{Fishlevel::TRASH, 0.3},
	{Fishlevel::SMALL, 0.15},
	{Fishlevel::MEDIUM, 0.1},
	{Fishlevel::BIG, 0.05}
	/*{"small", 0.3},
	{"medium", 0.15},
	{"big", 0.05} */
};

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




	if (fishing.startFishing) {
		//printf("\n%f", timeFishing.ReadMSec());
		//printf("%d", lotteryrandomNum);
		if (timeFishing.ReadMSec() >= lotteryrandomNum * 1000) {
			if (thistimehooked) {
				hooked();
				thistimehooked = false;
			}
			if (timeFishing.ReadMSec() >= 5000) {
				fishing.startFishing = false;
				fishing.isFishing = false;
				if (!fishing.isFishing) {
					fishingfloat_getPlayerPosition = true;
				}
				castingline(fishingtype);
			}
		}
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
	else { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y + 100; }
	if (floatbody == nullptr && crearfloatbody) {
		floatbody = app->physics->CreateRectangleSensor(app->scene->GetPlayer()->position.x, app->scene->GetPlayer()->position.y, 20, 20, bodyType::DYNAMIC);
		floatbody->ctype = ColliderType::FLOAT;
		floatbody->body->SetFixedRotation(true);
		floatbody->listener = this;
		crearfloatbody = false;

	}

	float timeLerp = 0.1f;
	fishingflota_position_x = fishingflota_position_x * (1 - timeLerp) + fishingflota_CenterX * timeLerp;
	fishingflota_position_y = fishingflota_position_y * (1 - timeLerp) + fishingflota_CenterY * timeLerp;


	float cheke_x = (METERS_TO_PIXELS(floatbody->body->GetPosition().x) - texH / 2) - 23;
	float cheke_y = (METERS_TO_PIXELS(floatbody->body->GetPosition().y) - texH / 2) - 23;


	if (direction == Direction::UP) {
		if (cheke_y >= fishingflota_position_y) {
			b2Vec2 force(0.0f, -10.0f);
			floatbody->body->ApplyForceToCenter(force, true);
		}
		else {
			floatbody->body->SetLinearVelocity(b2Vec2(0, 0));
		}
		app->render->DrawTexture(fishingfloat_texture, fishingflota_position_x - 23, fishingflota_position_y, 3);
	}
	else if (direction == Direction::DOWN) {
		if (cheke_y <= fishingflota_position_y) {
			b2Vec2 force(0.0f, 10.0f);
			floatbody->body->ApplyForceToCenter(force, true);
		}
		else {
			floatbody->body->SetLinearVelocity(b2Vec2(0, 0));
		}
		app->render->DrawTexture(fishingfloat_texture, fishingflota_position_x - 23, fishingflota_position_y, 3);
	}
	else if (direction == Direction::LEFT) {
		if (cheke_x >= fishingflota_position_x) {
			b2Vec2 force(-10.0f, 0.0f);
			floatbody->body->ApplyForceToCenter(force, true);
		}
		else {
			floatbody->body->SetLinearVelocity(b2Vec2(0, 0));
		}
		app->render->DrawTexture(fishingfloat_texture, fishingflota_position_x, fishingflota_position_y - 23, 3);
	}
	else if (direction == Direction::RIGHT) {
		if (cheke_x <= fishingflota_position_x) {
			b2Vec2 force(10.0f, 0.0f);
			floatbody->body->ApplyForceToCenter(force, true);
		}
		else {
			floatbody->body->SetLinearVelocity(b2Vec2(0, 0));
		}
		app->render->DrawTexture(fishingfloat_texture, fishingflota_position_x, fishingflota_position_y - 23, 3);
	}
	else {
		if (cheke_y <= fishingflota_position_y) {
			b2Vec2 force(0.0f, 10.0f);
			floatbody->body->ApplyForceToCenter(force, true);
		}
		else {
			floatbody->body->SetLinearVelocity(b2Vec2(0, 0));
		}
		app->render->DrawTexture(fishingfloat_texture, fishingflota_position_x - 23, fishingflota_position_y, 3);
	}


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



void RodSystem::hooked()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dis(0.0, 1.0);
	double random_number = dis(gen);

	// 根据概率分配抽奖
	double cumulative_probability = 0.0;
	Fishlevel selected_prize;
	for (const auto& prize : prizes) {
		cumulative_probability += prize.second;
		if (random_number <= cumulative_probability) {
			selected_prize = prize.first;
			break;
		}
	}

	switch (selected_prize)
	{
	case Fishlevel::NOTHING: printf("\nNothing"); break;
	case Fishlevel::TRASH:  printf("\nTRASH"); break;
	case Fishlevel::SMALL:  printf("\nSMALL"); break;
	case Fishlevel::MEDIUM:  printf("\nMEDIUM"); break;
	case Fishlevel::BIG:printf("\nBIG"); break;
	case Fishlevel::UNKNOWN:LOG("Collision UNKNOWN"); break;
	}
}





void RodSystem::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	case ColliderType::LAGO:
		fishing.startFishing = true;
		timeFishing.Start();
		lotteryrandomNum = rand() % 3 + 2;
		thistimehooked = true;
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}


