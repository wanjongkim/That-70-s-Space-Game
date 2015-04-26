#include "sssf_VS\stdafx.h"
#include "sssf\game\Game.h"
#include "ShootingBot.h"
#include "BasicBulletBot.h"
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
clone - this method makes another shootingBot object, but does
not completely initialize it with similar data to this. Most of the
object, like velocity and position, are left uninitialized.
*/
Bot* ShootingBot::clone()
{
	ShootingBot *botClone = new ShootingBot(this->cyclesBeforeShoot, this->cyclesBeforeSwapMove, this->moveDirection);
	return botClone;
}



/*
shootAtPoint - determines the angle to the player's position, and uses that to determine
x and y components of a vector with a magnitude that equals this bot's projectile's speed
*/

void ShootingBot::shootAtPoint(float x, float y, Game *game){
	static const wstring IDLE = (L"IDLE");
	AnimatedSpriteType *enemyBulletType = game->getGSM()->getSpriteManager()->getSpriteType(8);
	AnimatedSprite *enemyBullet = new AnimatedSprite();
	Physics *physics = game->getGSM()->getPhysics();
	Bot *bot = new BasicBulletBot();
	physics->addCollidableObject(bot);
	PhysicalProperties *bulletProps = bot->getPhysicalProperties();
	bulletProps->setPosition(this->getPhysicalProperties()->getX(), this->getPhysicalProperties()->getY());
	bulletProps->setSpriteType(8);
	bot->setSpriteType(enemyBulletType);
	bot->setAlpha(255);
	bot->setCurrentState(IDLE);
	if (this->getPhysicalProperties()->getX() > x){
		if (this->getPhysicalProperties()->getY() > y){
			float angle = 90 + ((180 / PI) * atan((this->getPhysicalProperties()->getX() - x) / (this->getPhysicalProperties()->getY() - y)));
			bulletProps->setVelocity(20 * cos(angle * (PI / 180)), -20 * sin(angle * (PI / 180)));
			game->getGSM()->getSpriteManager()->addBot(bot);
			bot->affixTightAABBBoundingVolume();
		}
		else{
			float angle = 180 + (90 - ((180 / PI) * atan((this->getPhysicalProperties()->getX() - x) / (y - this->getPhysicalProperties()->getY()))));
			bulletProps->setVelocity(20 * cos(angle * (PI / 180)), -20 * sin(angle * (PI / 180)));
			game->getGSM()->getSpriteManager()->addBot(bot);
			bot->affixTightAABBBoundingVolume();
		}
	}
	else{
		if (this->getPhysicalProperties()->getY() > y){
			float angle = 90 - ((180 / PI) * atan((x - this->getPhysicalProperties()->getX()) / (this->getPhysicalProperties()->getY() - y)));
			bulletProps->setVelocity(20 * cos(angle * (PI / 180)), -20 * sin(angle * (PI / 180)));
			game->getGSM()->getSpriteManager()->addBot(bot);
			bot->affixTightAABBBoundingVolume();
		}
		else{
			float angle = 270 + ((180 / PI) * atan((x - this->getPhysicalProperties()->getX()) / (y - this->getPhysicalProperties()->getY())));
			bulletProps->setVelocity(20 * cos(angle * (PI / 180)),- 20 * sin(angle * (PI / 180)));
			game->getGSM()->getSpriteManager()->addBot(bot);
			bot->affixTightAABBBoundingVolume();
		}
	}
}

/*
think - called once per frame, this is where the bot performs its
decision-making. 
*/
void ShootingBot::think(Game *game)
{
	static const wstring DEAD = (L"DEAD");
	if (actualCyclesBeforeShoot == 0){
		AnimatedSprite *player = game->getGSM()->getSpriteManager()->getPlayer();
		float playerDist = sqrt((this->getPhysicalProperties()->getX() - player->getPhysicalProperties()->getX())*(this->getPhysicalProperties()->getX() - player->getPhysicalProperties()->getX()) + (this->getPhysicalProperties()->getY() - player->getPhysicalProperties()->getY()) * (this->getPhysicalProperties()->getY() - player->getPhysicalProperties()->getY()));
		if (playerDist < 1300 && player->getCurrentState() != DEAD){
			this->shootAtPoint(player->getPhysicalProperties()->getX()+60, player->getPhysicalProperties()->getY()+60, game);
			game->getGSM()->getAudioManager()->playSound("enemyFire");
			actualCyclesBeforeShoot = cyclesBeforeShoot;
		}
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