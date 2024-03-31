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
#include "DialogTriggerEntity.h"
#include "DialogManager.h"
#include "Dialog.h"
#include <map>
#include <random>

double nothing_probability = 0.4;
double trash_probability = 0.3;
double small_probability = 0.15;
double medium_probability = 0.1;
double big_probability = 0.05;

std::map<Fishlevel, double> prizes = {
	{Fishlevel::NOTHING, nothing_probability},
	{Fishlevel::TRASH, trash_probability},
	{Fishlevel::SMALL, small_probability},
	{Fishlevel::MEDIUM, medium_probability},
	{Fishlevel::BIG, big_probability}
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
	//tanca dialogo automatica
	if (dialogoautoclose) {
		app->dialogManager->AutoNextDiagolo(dialogoTimeCount);
	}

	if (!fishing.isFishing) {
		if (app->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {
			if (fishing.fishingtype == FISHINGTYPE::FISHING) {
				printf("LUREFISHING");
				fishing.fishingtype = FISHINGTYPE::LUREFISHING;
			}
			else
			{
				fishing.fishingtype = FISHINGTYPE::FISHING;
				printf("FISHING");
			}
		}
	}

	//StartFishing
	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		fishing.isFishing = !fishing.isFishing;
		isFishingta = fishing.isFishing;
		//Cancelar
		if (!fishing.isFishing) {
			app->dialogManager->autoNextTime_TimerDown.Start();
			dialogoTimeCount = 0;
			dialogoautoclose = true;
			dialogoPlayerMoving = false;
			fishingfloat_getPlayerPosition = true;
			fishingOver();
		}
		castingline(fishingtype);
	}

	//Animation float
	if (fishingfloat_lineReady) {
		if (fishing.fishingtype == FISHINGTYPE::FISHING) {
			ani_castingline(app->scene->GetPlayer()->player_Direction);
		}
		else {
			ani_castingline_lure(app->scene->GetPlayer()->player_Direction);
		}
		
	}


	//if player mover, end fishing
	if (app->scene->GetPlayer()->playermove) {

		fishingOver();

		if (dialogoPlayerMoving) {
			app->dialogManager->autoNextTime_TimerDown.Start();
			dialogoTimeCount = 0;
			dialogoautoclose = true;
			dialogoPlayerMoving = false;
		}
	}

	//printf("\nstartFishing%d", fishing.startFishing);
	//GamePlaye
	if (fishing.startFishing) {
		if (fishing.fishingtype == FISHINGTYPE::FISHING) {
			playNormalFishing();
		}
		else {
			//printf("aun no tiene play");
		}

		


	}

	if (isEnd) {
		dialogoTimeCount = 2;
		dialogoautoclose = true;
		app->dialogManager->autoNextTime_TimerDown.Start();
		fishingEndCloseDialogo = false;
		isEnd = false;
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
	//obtener position de player
	if (fishingfloat_getPlayerPosition) {
		fishingflota_position_x = app->scene->GetPlayer()->position.x;
		fishingflota_position_y = app->scene->GetPlayer()->position.y;
		fishingfloat_getPlayerPosition = false;
	}

	floatDistance = 100;

	if (direction == Direction::UP) { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y - floatDistance; }
	else if (direction == Direction::DOWN) { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y + floatDistance; }
	else if (direction == Direction::LEFT) { fishingflota_CenterX = app->scene->GetPlayer()->position.x - floatDistance; fishingflota_CenterY = app->scene->GetPlayer()->position.y; }
	else if (direction == Direction::RIGHT) { fishingflota_CenterX = app->scene->GetPlayer()->position.x + floatDistance; fishingflota_CenterY = app->scene->GetPlayer()->position.y; }
	else { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y + floatDistance; }

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


void RodSystem::ani_castingline_lure(Direction direction)
{
	//obtener position de player
	if (fishingfloat_getPlayerPosition) {
		fishingflota_position_x = app->scene->GetPlayer()->position.x;
		fishingflota_position_y = app->scene->GetPlayer()->position.y;
		fishingfloat_getPlayerPosition = false;
	}

	floatDistance = 300;

	if (direction == Direction::UP) { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y - floatDistance; }
	else if (direction == Direction::DOWN) { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y + floatDistance; }
	else if (direction == Direction::LEFT) { fishingflota_CenterX = app->scene->GetPlayer()->position.x - floatDistance; fishingflota_CenterY = app->scene->GetPlayer()->position.y; }
	else if (direction == Direction::RIGHT) { fishingflota_CenterX = app->scene->GetPlayer()->position.x + floatDistance; fishingflota_CenterY = app->scene->GetPlayer()->position.y; }
	else { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y + floatDistance; }

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

void RodSystem::playNormalFishing()
{
	if (timeFishing.ReadMSec() >= lotteryrandomNum * 1000) {
		if (thistimehooked) {
			thistimehooked = false;
			ishooked = true;
			dialogoTimeCount = 0;
			dialogoautoclose = true;
			app->dialogManager->CreateDialogSinEntity("Ostia puta a pescado", "jiajie");
			playerGoplay = true;
			gamePlayTime = getRandomNumber(3, 6);
			gamePlayTimeLimit.Start();
			app->dialogManager->autoNextTime_TimerDown.Start();
			//printf("hoook");
		}


		//player count
		if (app->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN) {
			player_click_count += 1;
		}
		if (playerGoplay == true) {
			GamePlaye(selected_prize);
		}

		//if player no play, end fishing
		if (playerGoplay_TimeOver && player_click_count_TimeOver == 0) {

			player_click_count_TimeOver = 0;
			playerGoplay_TimeOver = false;
			dialogoTimeCount = 0;
			dialogoautoclose = true;
			app->dialogManager->CreateDialogSinEntity("Joder, porque no pesca", "jiajie");
			app->dialogManager->autoNextTime_TimerDown.Start();
			fishingOver();

		}
	}
}


void RodSystem::hooked(int player_click_count)
{

	if (player_click_count < 10) {
		changeProbability(0.4, 0.3, 0.15, 0.1, 0.05);
	}
	else if (player_click_count >= 10 && player_click_count < 20) {
		changeProbability(0.3, 0.4, 0.15, 0.1, 0.05);
	}
	else if (player_click_count >= 20 && player_click_count < 30) {
		changeProbability(0.25, 0.35, 0.2, 0.15, 0.05);
	}
	else if (player_click_count >= 30 && player_click_count < 40) {

		changeProbability(0.2, 0.35, 0.2, 0.15, 0.1);
	}
	else if (player_click_count >= 40 && player_click_count < 50) {

		changeProbability(0.1, 0.25, 0.3, 0.2, 0.15);
	}
	else if (player_click_count > 50) {

		changeProbability(0.0, 0.0, 0.0, 0.0, 1.0);
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dis(0.0, 1.0);
	double random_number = dis(gen);

	double cumulative_probability = 0.0;
	selected_prize;
	for (const auto& prize : prizes) {
		cumulative_probability += prize.second;
		if (random_number <= cumulative_probability) {
			selected_prize = prize.first;
			break;
		}
	}

	for (const auto& prize : prizes) {
		printf("\n%f", prize.second);
	}



	switch (selected_prize)
	{
	case Fishlevel::NOTHING: fishName = "NOTHING"; break;
	case Fishlevel::TRASH: fishName = "TRASH"; break;
	case Fishlevel::SMALL:  fishName = "SMALL"; break;
	case Fishlevel::MEDIUM:  fishName = "MEDIUM"; break;
	case Fishlevel::BIG:fishName = "BIG"; break;
	case Fishlevel::UNKNOWN:LOG("Collision UNKNOWN"); break;
	}

	std::string strNumber = std::to_string(player_click_count);

	dialogoTimeCount = 0;
	dialogoautoclose = true;
	app->dialogManager->CreateDialogSinEntity("you click " + strNumber + " veces " + " tu obtenido " + fishName, "jiajie");
	app->dialogManager->autoNextTime_TimerDown.Start();
	fishingOver();
	resetProbability();
}

void RodSystem::GamePlaye(Fishlevel fishleve)
{
	gamePlayTimeLimit_show = gamePlayTimeLimit.CountDown(gamePlayTime);

	//printf("\n%d", (int)gamePlayTimeLimit_show);
	if ((float)gamePlayTimeLimit_show == 0) {
		printf("\nTimeStop, you get click %d veces", player_click_count);
		playerGoplay = false;
		playerGoplay_TimeOver = true;
		player_click_count_TimeOver = player_click_count;
		if (player_click_count_TimeOver != 0) {
			hooked(player_click_count);
		}
		player_click_count = 0;
	}
}

void RodSystem::resetProbability() {
	nothing_probability = 0.4;
	trash_probability = 0.3;
	small_probability = 0.15;
	medium_probability = 0.1;
	big_probability = 0.05;

	prizes[Fishlevel::NOTHING] = nothing_probability;
	prizes[Fishlevel::TRASH] = trash_probability;
	prizes[Fishlevel::SMALL] = small_probability;
	prizes[Fishlevel::MEDIUM] = medium_probability;
	prizes[Fishlevel::BIG] = big_probability;

}

void RodSystem::changeProbability(double nothing, double trash, double small, double medium, double big)
{
	nothing_probability = nothing;
	trash_probability = trash;
	small_probability = small;
	medium_probability = medium;
	big_probability = big;

	prizes[Fishlevel::NOTHING] = nothing_probability;
	prizes[Fishlevel::TRASH] = trash_probability;
	prizes[Fishlevel::SMALL] = small_probability;
	prizes[Fishlevel::MEDIUM] = medium_probability;
	prizes[Fishlevel::BIG] = big_probability;
}

int RodSystem::getRandomNumber(int min, int max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(min, max);
	return dis(gen);
}

void RodSystem::fishingOver()
{
	if (app->scene->GetPlayer()->playermove == false) {
		fishingEndCloseDialogo = true;
	}
	fishing.isFishing = false;
	fishing.startFishing = false;
	if (!fishing.isFishing) {
		fishingfloat_getPlayerPosition = true;
	}
	castingline(fishingtype);
}

void RodSystem::OnCollision(PhysBody* physA, PhysBody* physB) {




	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	case ColliderType::LAGO:
		fishing.startFishing = true;
		dialogoPlayerMoving = true;
		timeFishing.Start();
		lotteryrandomNum = getRandomNumber(2, 4);
		thistimehooked = true;

		app->dialogManager->CreateDialogSinEntity("fishing", "jiajie");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}


