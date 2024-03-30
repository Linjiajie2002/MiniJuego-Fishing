#include "DialogManager.h"
#include "Player.h"
#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include "Window.h"
#include "Fishing.h"
#include "FishingManager.h"

#include "Defs.h"
#include "Log.h"
#include "SDL_ttf/include/SDL_ttf.h"

FishingManager::FishingManager() : Module()
{
	name.Create("fishingmanager");
}


FishingManager::~FishingManager()
{}


bool FishingManager::Awake(pugi::xml_node config)
{
	LOG("Loading Fishing Manager");
	bool ret = true;

	


	/*for (pugi::xml_node itemNode = config.child("fishlevel"); itemNode; itemNode = itemNode.next_sibling("trashes"))
	{
		
	}*/

	return ret;

}

bool FishingManager::Start() {

	bool ret = true;

	


	return ret;
}

bool FishingManager::CleanUp()
{
	bool ret = true;


	return ret;
}



bool FishingManager::Update(float dt) {

	bool ret = true;

	

	return ret;

}

