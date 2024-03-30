#ifndef __FISHING_H__
#define __FISHING_H__

#include "Point.h"
#include "Input.h"
#include "Render.h"
#include "List.h"


enum class Fishlevel
{
	TRASH,
	SMALL,
	MEDIUM,
	BIG
};

enum class Rodlevel
{
	BASIC,
	INTERMEDIATE,
	ADVANCED
};

enum class FISHINGTYPE
{
	FISHING,
	LUREFISHING
};

class Fishing
{
public:
	Fishlevel fishlever;
	Rodlevel rodlevel;
	FISHINGTYPE fishingtype;

	bool rodReady = true;
	bool isFishing = false;
	bool startFishing = false;



};

#endif // __DIALOG_H__