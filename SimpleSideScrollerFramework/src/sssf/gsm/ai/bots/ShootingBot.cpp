#include "sssf_VS\stdafx.h"
#include "sssf\game\Game.h"
#include "ShootingBot.h"
#include "sssf\gsm\state\GameStateManager.h"
#include "sssf\gsm\physics\Physics.h"



/*
This is the public constructor used by other classes for
creating these types of bots.
*/
ShootingBot::ShootingBot(unsigned int shotCyc, unsigned int moveCyc, unsigned int moveDir)
{
	cyclesBeforeShoot = shotCyc;
	cyclesBeforeSwapMove = moveCyc;
	moveDirection = moveDir; 
	actualCyclesBeforeShoot = shotCyc;
	actualCyclesBeforeSwapMove = moveCyc;
	actualMoveDirection = moveDir;
}

/*
clone - this method makes another BasicBulletBot object, but does
not completely initialize it with similar data to this. Most of the
object, like velocity and position, are left uninitialized.
*/
Bot* ShootingBot::clone()
{
	ShootingBot *botClone = new ShootingBot(this->cyclesBeforeShoot, this->cyclesBeforeSwapMove, this->moveDirection);
	return botClone;
}






/*
think - called once per frame, this is where the bot performs its
decision-making. Note that we might not actually do any thinking each
frame, depending on the value of cyclesRemainingBeforeThinking.
*/
void ShootingBot::think(Game *game)
{
	if (actualCyclesBeforeShoot == 0){
		AnimatedSprite *player = game->getGSM()->getSpriteManager()->getPlayer();
		actualCyclesBeforeShoot = cyclesBeforeShoot;
	}
	else{
		actualCyclesBeforeShoot -= 1;
	}
	if (actualCyclesBeforeSwapMove == 0){
		actualMoveDirection = !actualMoveDirection;
		actualCyclesBeforeSwapMove = cyclesBeforeSwapMove;
	}
	else{
		actualCyclesBeforeSwapMove -= 1;
	}
	switch (moveDirection){
	case 1:if (actualMoveDirection){
		this->getPhysicalProperties()->setVelocity(0.0f, -12.0f);
		}
		   else this->getPhysicalProperties()->setVelocity(0.0f, 12.0f);
		   break;
	case 2:if (actualMoveDirection){
		this->getPhysicalProperties()->setVelocity(12.0f, 0.0f);
	}
		   else this->getPhysicalProperties()->setVelocity(-12.0f, 0.0f);
		   break;
	case 3:if (actualMoveDirection){
		this->getPhysicalProperties()->setVelocity(0.0f, 12.0f);
	}
		   else this->getPhysicalProperties()->setVelocity(0.0f, -12.0f);
		   break;
	case 4:if (actualMoveDirection){
		this->getPhysicalProperties()->setVelocity(-12.0f, 0.0f);
	}
		   else this->getPhysicalProperties()->setVelocity(12.0f, 0.0f);
		   break;
	}


}