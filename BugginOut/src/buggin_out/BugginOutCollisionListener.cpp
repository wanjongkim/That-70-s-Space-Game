#include "sssf\game\Game.h"
#include "sssf\gsm\physics\Collision.h"
#include "sssf\gsm\sprite\SpriteManager.h"
#include "buggin_out\BugginOutCollisionListener.h"
#include "buggin_out\BugginOut.h"

void BugginOutCollisionListener::respondToCollision(Game *game, Collision *collision)
{
	// NOTE FROM THE COLLIDABLE OBJECTS, WHICH ARE IN THE COLLISION,
	// WE CAN CHECK AND SEE ON WHICH SIDE THE COLLISION HAPPENED AND
	// CHANGE SOME APPROPRIATE STATE ACCORDINGLY


	CollidableObject *col1 = collision->getCO1();
	CollidableObject *col2 = collision->getCO2();
	AnimatedSprite* an1 = dynamic_cast<AnimatedSprite*>(col1);
	AnimatedSprite* an2 = dynamic_cast<AnimatedSprite*>(col2);
	if (col1->getPhysicalProperties()->isCollidable() && col2->getPhysicalProperties()->isCollidable()){
		if (collision->isCollisionWithTile())
		{
			if (col1->getPhysicalProperties()->getSpriteType() == 4 || col1->getPhysicalProperties()->getSpriteType() == 8){
				game->getGSM()->getAudioManager()->playSound(CUE_BULLET_EXPLOSION);
				an1->setCurrentState(DEAD);
			}
			else if (col2->getPhysicalProperties()->getSpriteType() == 4 || col2->getPhysicalProperties()->getSpriteType() == 8){
				game->getGSM()->getAudioManager()->playSound(CUE_BULLET_EXPLOSION);
				an2->setCurrentState(DEAD);
			}


			if (col1->getPhysicalProperties()->getSpriteType() == 0 || col1->getPhysicalProperties()->getSpriteType() == 1){
				game->getGSM()->getAudioManager()->playSound(CUE_PLAYER_EXPLOSION);
				an1->setCurrentState(DEAD);
			}
			else if (col2->getPhysicalProperties()->getSpriteType() == 0 || col2->getPhysicalProperties()->getSpriteType() == 1){
				game->getGSM()->getAudioManager()->playSound(CUE_PLAYER_EXPLOSION);
				an2->setCurrentState(DEAD);
			}

		}
		else
		{
			if ((col1->getPhysicalProperties()->getSpriteType() == 8 && col2->getPhysicalProperties()->getSpriteType() == 0) || (col1->getPhysicalProperties()->getSpriteType() == 0 && col2->getPhysicalProperties()->getSpriteType() == 8)){
				game->getGSM()->getAudioManager()->playSound(CUE_PLAYER_EXPLOSION);
				an1->setCurrentState(DEAD);
				an2->setCurrentState(DEAD);
			}
			/*if ((col1->getPhysicalProperties()->getSpriteType() == 4 && col2->getPhysicalProperties()->getSpriteType() == 5) || (col1->getPhysicalProperties()->getSpriteType() == 5 && col2->getPhysicalProperties()->getSpriteType() == 4)
				|| (col1->getPhysicalProperties()->getSpriteType() == 4 && col2->getPhysicalProperties()->getSpriteType() == 3) || (col1->getPhysicalProperties()->getSpriteType() == 3 && col2->getPhysicalProperties()->getSpriteType() == 4)
				|| (col1->getPhysicalProperties()->getSpriteType() == 4 && col2->getPhysicalProperties()->getSpriteType() == 2) || (col1->getPhysicalProperties()->getSpriteType() == 2 && col2->getPhysicalProperties()->getSpriteType() == 4)
				|| (col1->getPhysicalProperties()->getSpriteType() == 4 && col2->getPhysicalProperties()->getSpriteType() == 1) || (col1->getPhysicalProperties()->getSpriteType() == 1 && col2->getPhysicalProperties()->getSpriteType() == 4)){
				an1->setCurrentState(DEAD);
				an2->setCurrentState(DEAD);
			}*/
		}
	}
}