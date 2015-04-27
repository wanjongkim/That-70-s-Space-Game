#include "sssf_VS\stdafx.h"
#include "sssf\game\Game.h"
#include "NoAimBot.h"
#include "BasicBulletBot.h"
#include "sssf\gsm\state\GameStateManager.h"
#include "sssf\gsm\physics\Physics.h"

NoAimBot::NoAimBot(unsigned int shotCyc, unsigned int dir){
	cyclesBeforeShoot = shotCyc;
	actualCyclesBeforeShoot = shotCyc;
	shotDirection = dir;
}

Bot* NoAimBot::clone()
{
	NoAimBot *botClone = new NoAimBot(this->cyclesBeforeShoot, this->shotDirection);
	return botClone;
}

void NoAimBot::think(Game *game){
	static const wstring DEAD = (L"DEAD");
	Viewport *view = game->getGUI()->getViewport();
	if (actualCyclesBeforeShoot == 0){
		AnimatedSprite *player = game->getGSM()->getSpriteManager()->getPlayer();
		float playerDist = sqrt((this->getPhysicalProperties()->getX() - player->getPhysicalProperties()->getX())*(this->getPhysicalProperties()->getX() - player->getPhysicalProperties()->getX()) + (this->getPhysicalProperties()->getY() - player->getPhysicalProperties()->getY()) * (this->getPhysicalProperties()->getY() - player->getPhysicalProperties()->getY()));
		if (playerDist < 1300 && player->getCurrentState() != DEAD && view->getViewportX() < this->getPhysicalProperties()->getX() &&
			this->getPhysicalProperties()->getX() < (view->getViewportX() + view->getViewportWidth()) && view->getViewportY() < this->getPhysicalProperties()->getY() &&
			this->getPhysicalProperties()->getY() < (view->getViewportY() + view->getViewportHeight())){
			static const wstring IDLE = (L"IDLE");
			AnimatedSpriteType *enemyBulletType = game->getGSM()->getSpriteManager()->getSpriteType(8);
			AnimatedSprite *enemyBullet = new AnimatedSprite();
			Physics *physics = game->getGSM()->getPhysics();
			Bot *bot = new BasicBulletBot();
			physics->addCollidableObject(bot);
			PhysicalProperties *bulletProps = bot->getPhysicalProperties();
			bulletProps->setSpriteType(8);
			bot->setSpriteType(enemyBulletType);
			bot->setAlpha(255);
			b2World* bWorld = game->getGSM()->getb2World();
			b2BodyDef botBodyDef;
			botBodyDef.type = b2_dynamicBody;
			if (shotDirection == 1){
				bulletProps->setPosition(this->getPhysicalProperties()->getX() + 32, this->getPhysicalProperties()->getY() - 10);
				botBodyDef.position.Set((this->getPhysicalProperties()->getX() + 32) / 64, (3200 - (this->getPhysicalProperties()->getY() - 10)) / 64);
			}
			else if (shotDirection == 2){
				bulletProps->setPosition(this->getPhysicalProperties()->getX() + 70, this->getPhysicalProperties()->getY() +32);
				botBodyDef.position.Set((this->getPhysicalProperties()->getX() + 70) / 64, (3200 - (this->getPhysicalProperties()->getY() +32)) / 64);
			}
			else if (shotDirection == 3){
				bulletProps->setPosition(this->getPhysicalProperties()->getX() + 32, this->getPhysicalProperties()->getY() +70);
				botBodyDef.position.Set((this->getPhysicalProperties()->getX() + 32) / 64, (3200 - (this->getPhysicalProperties()->getY() +70)) / 64);
			}
			else{
				bulletProps->setPosition(this->getPhysicalProperties()->getX() -10, this->getPhysicalProperties()->getY() +32);
				botBodyDef.position.Set((this->getPhysicalProperties()->getX() -10) / 64, (3200 - (this->getPhysicalProperties()->getY() +32)) / 64);
			}
			botBodyDef.angle = 0;
			b2Body* dynamicBody = bWorld->CreateBody(&botBodyDef);
			b2PolygonShape boxShape;
			boxShape.SetAsBox(.125, .125);
			b2FixtureDef boxFixtureDef;
			boxFixtureDef.shape = &boxShape;
			boxFixtureDef.density = 1;
			dynamicBody->CreateFixture(&boxFixtureDef);
			dynamicBody->SetUserData(bot);
			bot->setBody(dynamicBody);
			dynamicBody->SetLinearDamping(0);
			bot->setCurrentState(IDLE);
			if (shotDirection == 1){
				bulletProps->setVelocity(0, 15);
				dynamicBody->SetLinearVelocity(b2Vec2(0, 15));
			}
			else if (shotDirection == 2){
				bulletProps->setVelocity(15, 0);
				dynamicBody->SetLinearVelocity(b2Vec2(15, 0));
			}
			else if (shotDirection == 3){
				bulletProps->setVelocity(0, -15);
				dynamicBody->SetLinearVelocity(b2Vec2(0, -15));
			}
			else{
				bulletProps->setVelocity(-15, 0);
				dynamicBody->SetLinearVelocity(b2Vec2(-15, 0));
			}
			game->getGSM()->getSpriteManager()->addBot(bot);
			bot->affixTightAABBBoundingVolume();

			
			game->getGSM()->getAudioManager()->playSound("enemyFire");
			actualCyclesBeforeShoot = cyclesBeforeShoot;
		}
	}
	else{
		actualCyclesBeforeShoot -= 1;
	}
}