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

//base of the probability of catching fish
double nothing_probability = 0.4;
double trash_probability = 0.3;
double small_probability = 0.15;
double medium_probability = 0.1;
double big_probability = 0.05;

//probability machine for each type of fish
std::map<Fishlevel, double> fish = {
	{Fishlevel::NOTHING, nothing_probability},
	{Fishlevel::TRASH, trash_probability},
	{Fishlevel::SMALL, small_probability},
	{Fishlevel::MEDIUM, medium_probability},
	{Fishlevel::BIG, big_probability}
};

//probability machine for lure fishing
std::map<bool, double> isFishCaught = {
	{true, 0.9},
	{false, 0.1},
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
	fishing.fishingtype = FISHINGTYPE::FISHING;
	floatChangeDistance = 100;

	return true;
}

bool RodSystem::Update(float dt)
{

	//Automatic dialog box closure mechanism
	if (dialogoautoclose) {
		app->dialogManager->AutoNextDiagolo(dialogoTimeCount);
	}

	if (fishing.playerGetRod && !fishing.isFishing) {
		if (app->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {
			fishing.rodReady = !fishing.rodReady;
		}
	}//end_if, equip or stow the fishing rod

	//Change Rod
	if (!fishing.isFishing && fishing.rodReady) {
		if (app->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {
			if (fishing.fishingtype == FISHINGTYPE::LUREFISHING) {
				printf("\nRod:FISHING");
				fishing.fishingtype = FISHINGTYPE::FISHING;
				lureDistanceGetRandom = false;//Disable irregular distance
			}
			else
			{
				printf("\nRod:LUREFISHING");
				fishing.fishingtype = FISHINGTYPE::LUREFISHING;
				lureDistanceGetRandom = true;// enable irregular distance
			}//end_if for cheke is "LUREFISHING" o "FISHING"
		}//end_if for press "C" key
	}//end_if for check if fishing


	//Cast the rod and StartFishing
	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		fishing.isFishing = !fishing.isFishing;//Start fishing o Stop fishing

		/*if (fishing.isFishing == true && fishing.startFishing == false) {
			fishing.isFishing = true;
		}
		else {
			fishing.isFishing = !fishing.isFishing;//Start fishing o Stop fishing
		}*///can fix the bug where collision does not reach the lago area, but it's not perfect
		
		isFishingta = fishing.isFishing;//Save data to "isFishingta"
		if (lureDistanceGetRandom) {
			lureDistance = getRandomNumber(3, 6);
			floatChangeDistance = lureDistance * 100;
		}//end_if is lurefishing and get irregular distance
		else {
			floatChangeDistance = 100;
		}//normal fishing distance


		//Retrieve the rod
		if (!fishing.isFishing) {
			//Close dialogo
			dialogoClose(0);

			dialogoPlayerMoving = false;
			fishingfloat_getPlayerPosition = true;//Enable acquiring player's current location
			lureFinishLine = true;//In order to avoid repetition between the closing dialog for reeling in and the automatic dialog
			fishingOver();
		}//end_if for if not fishing

		castingline(fishing.fishingtype);//check if can fishing
	}//end_if for press "F" key



	//Animation float
	if (fishingfloat_lineReady) {
		ani_castingline(app->scene->GetPlayer()->player_Direction);//animation and collision of the float
	}//end_if can fishing or not


	//if player mover, end fishing
	if (app->scene->GetPlayer()->playermove) {
		if (dialogoPlayerMoving) {
			//Close dialogo
			dialogoClose(0);
			dialogoPlayerMoving = false;
		}//end_if, if player fishing moving, close dialogo
		fishingOver();
	}//end_if player moving in fishing


	//GamePlaye
	if (fishing.startFishing) {
		if (fishing.fishingtype == FISHINGTYPE::FISHING) {
			playNormalFishing();
		}
		else {
			playLureFishing();
		}//end_if, fishing type
	}//end_if, if start fishing

	if (isEnd) {
		dialogoClose(2);
		fishingEndCloseDialogo = false;
		isEnd = false;
	}//end_if, close last dialogo
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
		}//start
		else {
			fishingfloat_lineReady = false;
			if (floatbody != nullptr) {
				floatbody->body->GetWorld()->DestroyBody(floatbody->body);
				floatbody = nullptr;
			}//end_if, delete collision
		}//end_if, if fishing
	}//end_if, check if equipped with a fishing rod
}

void RodSystem::ani_castingline(Direction direction)
{
	//obtener position de player
	if (fishingfloat_getPlayerPosition) {
		fishingflota_position_x = app->scene->GetPlayer()->position.x;
		fishingflota_position_y = app->scene->GetPlayer()->position.y;
		fishingfloat_getPlayerPosition = false;
	}//end_if, can get player position

	floatDistance = floatChangeDistance;

	//Calculate the final position of the float
	if (direction == Direction::UP) { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y - floatDistance; }
	else if (direction == Direction::DOWN) { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y + floatDistance; }
	else if (direction == Direction::LEFT) { fishingflota_CenterX = app->scene->GetPlayer()->position.x - floatDistance; fishingflota_CenterY = app->scene->GetPlayer()->position.y; }
	else if (direction == Direction::RIGHT) { fishingflota_CenterX = app->scene->GetPlayer()->position.x + floatDistance; fishingflota_CenterY = app->scene->GetPlayer()->position.y; }
	else { fishingflota_CenterX = app->scene->GetPlayer()->position.x; fishingflota_CenterY = app->scene->GetPlayer()->position.y + floatDistance; }

	//crear float collision
	if (floatbody == nullptr && crearfloatbody) {
		floatbody = app->physics->CreateRectangleSensor(app->scene->GetPlayer()->position.x, app->scene->GetPlayer()->position.y, 20, 20, bodyType::DYNAMIC);
		floatbody->ctype = ColliderType::FLOAT;
		floatbody->body->SetFixedRotation(true);
		floatbody->listener = this;
		crearfloatbody = false;

	}

	//lerp
	float timeLerp = 0.1f;
	fishingflota_position_x = fishingflota_position_x * (1 - timeLerp) + fishingflota_CenterX * timeLerp;
	fishingflota_position_y = fishingflota_position_y * (1 - timeLerp) + fishingflota_CenterY * timeLerp;

	//moving collision
	float cheke_x = (METERS_TO_PIXELS(floatbody->body->GetPosition().x) - texH / 2) - 23;
	float cheke_y = (METERS_TO_PIXELS(floatbody->body->GetPosition().y) - texH / 2) - 23;

	if (startFinishingLine) {
		fishing_line(direction, cheke_x, cheke_y);
	}
	else {
		floatCollision(direction, cheke_x, cheke_y);
	}


}

void RodSystem::playNormalFishing()
{
	if (timeFishing.ReadMSec() >= lotteryrandomNum * 1000) {
		if (thistimehooked) {
			thistimehooked = false;
			//Close dialogo
			dialogoClose(0);
			//Crear new dialogo
			app->dialogManager->CreateDialogSinEntity("Ostia puta a pescado", "fishing System");
			playerGoplay = true;//Start game play
			gamePlayTime = getRandomNumber(3, 6);// Get play time
			gamePlayTimeLimit.Start();// reset play time
		}


		
		if (app->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN) {
			player_click_count += 1;
		}//end_if, player count

		if (playerGoplay == true) {
			GamePlaye();
		}//end_if, gameplay

		
		if (playerGoplay_TimeOver && player_click_count_TimeOver == 0) {
			player_click_count_TimeOver = 0;//reset
			playerGoplay_TimeOver = false;
			//Close dialogo
			dialogoClose(0);
			//Crear new dialogo
			app->dialogManager->CreateDialogSinEntity("Joder, porque no pesca", "fishing System");
			fishingOver();
		}//end_if player no play, end fishing
	}//end_if, if fish is caught
}

void RodSystem::playLureFishing()
{
	if (timeFishing.ReadMSec() >= lure_lotteryrandomNum * 1000 && lureRandomTime == true) {
		isFishCaught_result = check_isFishCaught();
		printf("\nResultado: %d ", isFishCaught_result);
		lureRandomTime = false;
	}//end_if, if fish is caught

	if (isFishCaught_result) {
		//Close dialogo
		dialogoClose(0);
		//Crear new dialogo
		app->dialogManager->CreateDialogSinEntity("Ostia puta a pescado", "jiajie");
		isFishCaught_result = false;
		playerGoplay = true;
		gamePlayTime = getRandomNumber(3, 6);
		gamePlayTimeLimit.Start();
	}//end_if, fish caught

	if (app->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN && playerGoplay == true) {
		player_click_count += 1;
	}//end_if, press "K" key

	if (playerGoplay == true) {
		GamePlaye();
	}//end_if, gameplay

	//if player no play, end fishing
	if (playerGoplay_TimeOver && player_click_count_TimeOver == 0) {
		player_click_count_TimeOver = 0;
		playerGoplay_TimeOver = false;
		//Close dialogo
		dialogoClose(0);
		//Crear new dialogo
		app->dialogManager->CreateDialogSinEntity("Joder, porque no pesca", "jiajie");
		fishingOver();
	}//end_if, player no play, end fishing

	if (playerGoplay == false) {
		if (app->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN) {
			printf("\nsorteo");
			startFinishingLine = true;
			lureRandomTime = true;
			lure_lotteryrandomNum = getRandomNumber(3, 7);
			timeFishing.Start();
			if (floatChangeDistance == 100) {
				fishing.isFishing = false;
				printf("finishi");
				if (!fishing.isFishing) {
					//Close dialogo
					dialogoClose(0);
					dialogoPlayerMoving = false;
					fishingfloat_getPlayerPosition = true;
					lureFinishLine = true;
					fishingOver();
				}//end_if, end fishing
				castingline(fishing.fishingtype);
			}
			else {
				floatChangeDistance -= 100;
			}//end_if, end fishing or continue fishing based on distance
		}//end_if, press "V" key
	}//end_if, if the player catches a fish, they cannot draw another prize
}

void RodSystem::fishing_line(Direction direction, float cheke_x, float cheke_y)
{
	//Lure fishing, confirm direction, reel in
	if (direction == Direction::UP) {
		if (cheke_y <= fishingflota_position_y) {
			b2Vec2 force(0.0f, 10.0f);
			floatbody->body->ApplyForceToCenter(force, true);
		}
		else {
			floatbody->body->SetLinearVelocity(b2Vec2(0, 0));
		}
		app->render->DrawTexture(fishingfloat_texture, fishingflota_position_x - 23, fishingflota_position_y, 3);
	}
	else if (direction == Direction::DOWN) {
		if (cheke_y >= fishingflota_position_y) {
			b2Vec2 force(0.0f, -10.0f);
			floatbody->body->ApplyForceToCenter(force, true);
		}
		else {
			floatbody->body->SetLinearVelocity(b2Vec2(0, 0));
		}
		app->render->DrawTexture(fishingfloat_texture, fishingflota_position_x - 23, fishingflota_position_y, 3);
	}
	else if (direction == Direction::LEFT) {
		if (cheke_x <= fishingflota_position_x) {
			b2Vec2 force(10.0f, 0.0f);
			floatbody->body->ApplyForceToCenter(force, true);
		}
		else {
			floatbody->body->SetLinearVelocity(b2Vec2(0, 0));
		}
		app->render->DrawTexture(fishingfloat_texture, fishingflota_position_x, fishingflota_position_y - 23, 3);
	}
	else if (direction == Direction::RIGHT) {
		if (cheke_x >= fishingflota_position_x) {
			b2Vec2 force(-10.0f, 0.0f);
			floatbody->body->ApplyForceToCenter(force, true);
		}
		else {
			floatbody->body->SetLinearVelocity(b2Vec2(0, 0));
		}
		app->render->DrawTexture(fishingfloat_texture, fishingflota_position_x, fishingflota_position_y - 23, 3);
	}
	else {
		if (cheke_y >= fishingflota_position_y) {
			b2Vec2 force(0.0f, -10.0f);
			floatbody->body->ApplyForceToCenter(force, true);
		}
		else {
			floatbody->body->SetLinearVelocity(b2Vec2(0, 0));
		}
		app->render->DrawTexture(fishingfloat_texture, fishingflota_position_x - 23, fishingflota_position_y, 3);
	}



}

void RodSystem::floatCollision(Direction direction, float cheke_x, float cheke_y)
{
	//Confirm direction, cast the float
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

void RodSystem::dialogoClose(int time)
{
	//Close dialogo
	dialogoTimeCount = time;
	dialogoautoclose = true;
	app->dialogManager->autoNextTime_TimerDown.Start();
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
	}//end_if, use different probabilities based on the player

	//lottery machine
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dis(0.0, 1.0);
	double random_number = dis(gen);

	double cumulative_probability = 0.0;
	selected_fish;
	for (const auto& check : fish) {
		cumulative_probability += check.second;
		if (random_number <= cumulative_probability) {
			selected_fish = check.first;
			break;
		}
	}

	for (const auto& prize : fish) {
		printf("\n%f", prize.second);
	}// print probabilities



	switch (selected_fish)
	{
	case Fishlevel::NOTHING: fishName = "NOTHING"; break;
	case Fishlevel::TRASH: fishName = "TRASH"; break;
	case Fishlevel::SMALL:  fishName = "SMALL"; break;
	case Fishlevel::MEDIUM:  fishName = "MEDIUM"; break;
	case Fishlevel::BIG:fishName = "BIG"; break;
	case Fishlevel::UNKNOWN:LOG("Collision UNKNOWN"); break;
	}//Reaction upon knowing what is obtained

	std::string strNumber = std::to_string(player_click_count);

	dialogoClose(0);
	app->dialogManager->CreateDialogSinEntity("you click " + strNumber + " veces " + " tu obtenido " + fishName, "jiajie");
	fishingOver();
	resetProbability();
}

void RodSystem::GamePlaye()
{
	gamePlayTimeLimit_show = gamePlayTimeLimit.CountDown(gamePlayTime);

	if ((float)gamePlayTimeLimit_show == 0) {
		printf("\nTimeStop, you get click %d times", player_click_count);
		playerGoplay = false;
		playerGoplay_TimeOver = true;
		player_click_count_TimeOver = player_click_count;
		if (player_click_count_TimeOver != 0) {
			hooked(player_click_count);
		}//end_if, if player click not 0
		player_click_count = 0;
	}//end_if, if gameplaytime over
}

void RodSystem::resetProbability() {
	//if the probability is changed, it will reset here.
	nothing_probability = 0.4;
	trash_probability = 0.3;
	small_probability = 0.15;
	medium_probability = 0.1;
	big_probability = 0.05;

	fish[Fishlevel::NOTHING] = nothing_probability;
	fish[Fishlevel::TRASH] = trash_probability;
	fish[Fishlevel::SMALL] = small_probability;
	fish[Fishlevel::MEDIUM] = medium_probability;
	fish[Fishlevel::BIG] = big_probability;

}

void RodSystem::changeProbability(double nothing, double trash, double small, double medium, double big)
{
	//The probability is changed here
	nothing_probability = nothing;
	trash_probability = trash;
	small_probability = small;
	medium_probability = medium;
	big_probability = big;

	fish[Fishlevel::NOTHING] = nothing_probability;
	fish[Fishlevel::TRASH] = trash_probability;
	fish[Fishlevel::SMALL] = small_probability;
	fish[Fishlevel::MEDIUM] = medium_probability;
	fish[Fishlevel::BIG] = big_probability;
}

int RodSystem::getRandomNumber(int min, int max) {
	//random number generator
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(min, max);
	return dis(gen);
}

bool RodSystem::check_isFishCaught()
{
	//Lure Fishing, whether the lottery results in catching a fish
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dis(0.0, 1.0);
	double random_number = dis(gen);

	double cumulative_probability = 0.0;

	for (const auto& cheke : isFishCaught) {
		cumulative_probability += cheke.second;
		if (random_number <= cumulative_probability) {
			return cheke.first;
			break;
		}
	}
}

void RodSystem::fishingOver()
{
	//if fishing ends
	if (app->scene->GetPlayer()->playermove == false && lureFinishLine == false) {
		fishingEndCloseDialogo = true;
	}//end_if, close dialogo if is last

	lureFinishLine = false;
	fishing.isFishing = false;
	fishing.startFishing = false;
	if (!fishing.isFishing) {
		fishingfloat_getPlayerPosition = true;
	}//end if, if not fishing
	startFinishingLine = false;
	castingline(fishingtype);
}

void RodSystem::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	case ColliderType::LAGO:
	
		if (fishing.startFishing == false) {
			app->dialogManager->CreateDialogSinEntity("fishing", "jiajie");
		}//end_if, if the float collides with the " LAGO"collider, display the fishing dialog
		fishing.startFishing = true;
		dialogoPlayerMoving = true;
		timeFishing.Start();

		if (fishing.fishingtype == FISHINGTYPE::FISHING) {
			lotteryrandomNum = getRandomNumber(2, 4);
		}
		else {
			lure_lotteryrandomNum = getRandomNumber(3, 7);
			lureRandomTime = true;
		}//end_if, React differently based on the different fishing rods
		thistimehooked = true;
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}


