#pragma once

#include "sssf_VS\stdafx.h"
#include "sssf\game\Game.h"
#include "sssf\gsm\sprite\AnimatedSprite.h"
#include <iostream>

#include "LuaPlusFramework\LuaPlus.h"
using namespace LuaPlus;
#include "stdio.h"
using namespace std;

class Bot : public AnimatedSprite
{
public:
	int frameCounter;
	Bot()	{ frameCounter = 0; }
	virtual ~Bot()	{}

	// TO BE DEFINED BY BOT AI CLASSES
	virtual void think(Game *game) = 0;
	virtual void setTexSize(int initx, int inity) = 0;
	virtual Bot* clone() = 0;
	int getFrameCount(){ return frameCounter; }
};