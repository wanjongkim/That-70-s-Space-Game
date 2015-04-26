#include "sssf_VS\stdafx.h"
#include "sssf\game\Game.h"
#include "HomingBot.h"
#include "BasicBulletBot.h"
#include "sssf\gsm\state\GameStateManager.h"
#include "sssf\gsm\physics\Physics.h"

HomingBot::HomingBot(){
	
}

Bot* HomingBot::clone()
{
	HomingBot *botClone = new HomingBot();
	return botClone;
}

void HomingBot::think(Game *game){
	static const wstring DEAD = (L"DEAD");
	float targetX = game->getGSM()->getSpriteManager()->getPlayer()->getPhysicalProperties()->getX();
	float targetY = game->getGSM()->getSpriteManager()->getPlayer()->getPhysicalProperties()->getY();
	AnimatedSprite* player = game->getGSM()->getSpriteManager()->getPlayer();
	Viewport* view = game->getGUI()->getViewport();
	float playerDist = sqrt((this->getPhysicalProperties()->getX() - player->getPhysicalProperties()->getX())*(this->getPhysicalProperties()->getX() - player->getPhysicalProperties()->getX()) + (this->getPhysicalProperties()->getY() - player->getPhysicalProperties()->getY()) * (this->getPhysicalProperties()->getY() - player->getPhysicalProperties()->getY()));
	if (playerDist < 1300 && player->getCurrentState() != DEAD && view->getViewportX() < this->getPhysicalProperties()->getX() &&
		this->getPhysicalProperties()->getX() < (view->getViewportX() + view->getViewportWidth()) && view->getViewportY() < this->getPhysicalProperties()->getY() &&
		this->getPhysicalProperties()->getY() < (view->getViewportY() + view->getViewportHeight())){

		if (this->getPhysicalProperties()->getX() > targetX){
			if (this->getPhysicalProperties()->getY() > targetY){
				float angle = 90 + ((180 / PI) * atan((this->getPhysicalProperties()->getX() - targetX) / (this->getPhysicalProperties()->getY() - targetY)));
				this->getPhysicalProperties()->setVelocity(6 * cos(angle * (PI / 180)), -6 * sin(angle * (PI / 180)));
				this->getBody()->SetLinearVelocity(b2Vec2(6 * cos(angle * (PI / 180)), 6 * sin(angle * (PI / 180))));
			}
			else{
				float angle = 180 + (90 - ((180 / PI) * atan((this->getPhysicalProperties()->getX() - targetX) / (targetY - this->getPhysicalProperties()->getY()))));
				this->getPhysicalProperties()->setVelocity(6 * cos(angle * (PI / 180)), -6 * sin(angle * (PI / 180)));
				this->getBody()->SetLinearVelocity(b2Vec2(6 * cos(angle * (PI / 180)), 6 * sin(angle * (PI / 180))));
			}
		}
		else{
			if (this->getPhysicalProperties()->getY() > targetY){
				float angle = 90 - ((180 / PI) * atan((targetX - this->getPhysicalProperties()->getX()) / (this->getPhysicalProperties()->getY() - targetY)));
				this->getPhysicalProperties()->setVelocity(6 * cos(angle * (PI / 180)), -6 * sin(angle * (PI / 180)));
				this->getBody()->SetLinearVelocity(b2Vec2(6 * cos(angle * (PI / 180)), 6 * sin(angle * (PI / 180))));
			}
			else{
				float angle = 270 + ((180 / PI) * atan((targetX - this->getPhysicalProperties()->getX()) / (targetY - this->getPhysicalProperties()->getY())));
				this->getPhysicalProperties()->setVelocity(6 * cos(angle * (PI / 180)), -6 * sin(angle * (PI / 180)));
				this->getBody()->SetLinearVelocity(b2Vec2(6 * cos(angle * (PI / 180)), 6 * sin(angle * (PI / 180))));
			}
		}
	}
}