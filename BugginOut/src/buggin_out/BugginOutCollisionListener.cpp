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
	if (collision->isCollisionWithTile())
	{
		if (col1->getPhysicalProperties()->getSpriteType() == 4){
			an1->setCurrentState(DEAD);
		}
		else if (col2->getPhysicalProperties()->getSpriteType() == 4){
			an2->setCurrentState(DEAD);
		}

		if (col1->getPhysicalProperties()->getSpriteType() == 0){
			an1->setCurrentState(DEAD);
		}
		else if (col2->getPhysicalProperties()->getSpriteType() == 0){
			an2->setCurrentState(DEAD);
		}

	}
	else
	{
		CollidableObject *sprite = collision->getCO1();
		if (sprite->getCollisionEdge() == BOTTOM_EDGE)
		{
			// ENEMY IS DEAD - WE SHOULD PLAY A DEATH ANIMATION
			// AND MARK IT FOR REMOVAL
		}
		else
		{
			// PLAYER IS DEAD - WE SHOULD PLAY A DEATH ANIMATION
			// AND MARK IT FOR REMOVAL/RESPAWN/RESTART GAME, WHATEVER
			// THE DEMANDS OF THE GAME ARE
		}
	}
}