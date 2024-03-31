#pragma once
#ifndef __FISHINGMANAGER_H__
#define __FISHINGMANAGER_H__

#include "Module.h"
#include "Dialog.h"
#include "Fishing.h"
#include "Timer.h"
#include "List.h"
#include <vector>
#include "Player.h"
#include "Physics.h"

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



public:


	
};

#endif // __DIALOGMANAGER_H__
