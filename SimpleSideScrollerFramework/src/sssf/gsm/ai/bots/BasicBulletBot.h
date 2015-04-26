#pragma once

// PREPROCESSOR INCLUDE STATEMENTS
#include "sssf_VS\stdafx.h"
#include "sssf\gsm\ai\Bot.h"
#include "sssf\gsm\physics\Physics.h"
#include "sssf\game\Game.h"

class BasicBulletBot : public Bot
{
private:
	unsigned int initX;
	unsigned int initY;
	int counter;
	

public:
	BasicBulletBot();
	virtual ~BasicBulletBot(){}
	Bot*	clone();
	void initBot();
	void setinitX(int x){ initX = x; }
	void setinitY(int y){ initY = y; }
	void	setTexSize(int initx, int inity){ this->getSpriteType()->setTextureSize(8, 8); }
	void	think(Game *game);

};