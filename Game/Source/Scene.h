#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "GuiControl.h"
#include "GuiControlButton.h"
#include "DialogTriggerEntity.h"

//NEW//
#include "MiniGameFishing.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Return the player position
	iPoint GetPLayerPosition();

	// Handles multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);

	Player* GetPlayer();
	DialogTrigger* GetDialogTrigger();

	//NEW//
	MiniGameFishing* GetRod();
	
private:
	SDL_Texture* img;
	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;
	SDL_Texture* mouseTileTex = nullptr;
	

	//L03: DONE 3b: Declare a Player attribute


	// L15: TODO 2: Declare a GUI Control Button 
	GuiControlButton* gcButtom;

public:
	Player* player = new Player;
	DialogTrigger* dialogtrigger = new DialogTrigger;

	//NEW//
	MiniGameFishing* fishing = new MiniGameFishing;
	
};

#endif // __SCENE_H__