#pragma once

// PREPROCESSOR INCLUDE STATEMENTS
#include "sssf_VS\stdafx.h"
#include "sssf\gsm\ai\Bot.h"
#include "sssf\gsm\physics\Physics.h"
#include "sssf\game\Game.h"

class ShootingBot : public Bot
{
private:
	unsigned int initX;
	unsigned int initY;
	unsigned int cyclesBeforeShoot;
	unsigned int cyclesBeforeSwapMove;
	unsigned int moveDirection;
	unsigned int actualCyclesBeforeShoot;
	unsigned int actualCyclesBeforeSwapMove;
	bool		 actualMoveDirection;



public:
	ShootingBot(unsigned int shotCyc, unsigned int moveCyc, unsigned int moveDir);
	virtual ~ShootingBot(){}
	Bot*	clone();
	void initBot();
	void setinitX(int x){ initX = x; }
	void setinitY(int y){ initY = y; }
	void	setTexSize(int initx, int inity){ this->getSpriteType()->setTextureSize(8, 8); }
	void shootAtPoint(float x, float y, Game *game);
	void	think(Game *game);

};