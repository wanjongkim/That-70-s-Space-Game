#include "sssf_VS\stdafx.h"
#include "sssf\game\Game.h"
#include "BasicBulletBot.h"
#include "sssf\gsm\state\GameStateManager.h"
#include "sssf\gsm\physics\Physics.h"



/*
This is the public constructor used by other classes for
creating these types of bots.
*/
BasicBulletBot::BasicBulletBot()
{
	frameCounter = 0;
}

/*
clone - this method makes another BasicBulletBot object, but does
not completely initialize it with similar data to this. Most of the
object, like velocity and position, are left uninitialized.
*/
Bot* BasicBulletBot::clone()
{
	BasicBulletBot *botClone = new BasicBulletBot();
	return botClone;
}






/*
think - called once per frame, this is where the bot performs its
decision-making. Note that we might not actually do any thinking each
frame, depending on the value of cyclesRemainingBeforeThinking.
*/
void BasicBulletBot::think(Game *game)
{

	frameCounter++;
}