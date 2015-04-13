/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	BugginOutKeyEventHandler.cpp

	See BugginOutKeyEventHandler.h for a class description.
*/

#include "buggin_out_VS\stdafx.h"
#include "buggin_out\BugginOut.h"
#include "buggin_out\BugginOutKeyEventHandler.h"
#include "sssf\game\Game.h"
#include "sssf\game\WStringTable.h"
#include "sssf\graphics\GameGraphics.h"
#include "sssf\gsm\physics\Physics.h"
#include "sssf\gsm\physics\PhysicalProperties.h"
#include "sssf\gsm\sprite\AnimatedSprite.h"
#include "sssf\gsm\state\GameState.h"
#include "sssf\gsm\state\GameStateManager.h"
#include "sssf\gui\Cursor.h"
#include "sssf\gui\GameGUI.h"
#include "sssf\input\GameInput.h"
#include "sssf\timer\GameTimer.h"
#include "sssf\platforms\Windows\WindowsTimer.h"
#include "sssf\gsm\ai\bots\BasicBulletBot.h"

/*
	handleKeyEvent - this method handles all keyboard interactions. Note that every frame this method
	gets called and it can respond to key interactions in any custom way. Ask the GameInput class for
	key states since the last frame, which can allow us to respond to key presses, including when keys
	are held down for multiple frames.
*/
void BugginOutKeyEventHandler::handleKeyEvents(Game *game)
{
	// WE CAN QUERY INPUT TO SEE WHAT WAS PRESSED
	GameInput *input = game->getInput();

	// LET'S GET THE PLAYER'S PHYSICAL PROPERTIES, IN CASE WE WANT TO CHANGE THEM
	GameStateManager *gsm = game->getGSM();
	SpriteManager *sm = gsm->getSpriteManager();
	AnimatedSpriteType *playerbulletSpriteType = sm->getSpriteType(4);
	AnimatedSprite *player = gsm->getSpriteManager()->getPlayer();
	PhysicalProperties *pp = player->getPhysicalProperties();
	Viewport *viewport = game->getGUI()->getViewport();
	
	// IF THE GAME IS IN PROGRESS
	if (gsm->isGameInProgress())
	{
		// WASD KEY PRESSES WILL CONTROL THE PLAYER
		// SO WE'LL UPDATE THE PLAYER VELOCITY WHEN THESE KEYS ARE
		// PRESSED, THAT WAY PHYSICS CAN CORRECT AS NEEDED
		float vX = pp->getVelocityX();
		float vY = pp->getVelocityY();

		// YOU MIGHT WANT TO UNCOMMENT THIS FOR SOME TESTING,
		// BUT IN THIS ASSIGNMENT, THE USER MOVES VIA MOUSE BUTTON PRESSES
		if (input->isKeyDownForFirstTime(R_KEY))
		{
			player->getPhysicalProperties()->swapFireDir();
		}

		if (input->isKeyDown(A_KEY))
		{
			vX = -PLAYER_SPEED;
			player->setCurrentState(MOVE_LEFT);
		}
		if (input->isKeyDown(D_KEY))
		{
			vX = PLAYER_SPEED;
			player->setCurrentState(MOVE_RIGHT);
		}
		if (input->isKeyDown(S_KEY))
		{
			vY = PLAYER_SPEED;
			player->setCurrentState(MOVE_DOWN);
		}
		if (input->isKeyDown(W_KEY))
		{
			vY = -PLAYER_SPEED;
			player->setCurrentState(MOVE_UP);
		}
		if (input->isKeyDownForFirstTime(G_KEY))
		{
			viewport->toggleDebugView();
			game->getGraphics()->toggleDebugTextShouldBeRendered();
		}
		if (!input->isKeyDown(W_KEY) && !input->isKeyDown(S_KEY))
		{
			vY = 0.0f;
			player->setCurrentState(IDLE);
		}
		if (!input->isKeyDown(A_KEY) && !input->isKeyDown(D_KEY)){

			vX = 0.0f;
		}
		
		if (input->isKeyDownForFirstTime(P_KEY))
		{
			gsm->getPhysics()->togglePhysics();
		}
		if (input->isKeyDownForFirstTime(T_KEY))
		{
			gsm->getPhysics()->activateForSingleUpdate();
		}

		// NOW SET THE ACTUAL PLAYER VELOCITY
 		pp->setVelocity(vX, vY);


		//PLAYER SHOOTING
		int shotTime = player->getPhysicalProperties()->getShot();
		if (player->getPhysicalProperties()->getFireDir()){
			if (input->isKeyDownForFirstTime(I_KEY) && shotTime == 0)
			{
				AnimatedSprite *playerBullet = new AnimatedSprite();
				Physics *physics = gsm->getPhysics();
				Bot *bot = new BasicBulletBot();
				physics->addCollidableObject(bot);
				PhysicalProperties *bulletProps = bot->getPhysicalProperties();
				bulletProps->setPosition(pp->getX() + 46, pp->getY());
				bulletProps->setVelocity(0.0f, -PLAYER_BULLET_SPEED);
				bot->setSpriteType(playerbulletSpriteType);
				bot->setAlpha(255);
				bot->setCurrentState(IDLE);
				sm->addBot(bot);
				bot->affixTightAABBBoundingVolume();
				player->getPhysicalProperties()->setShot(30);

			}
			if (input->isKeyDownForFirstTime(J_KEY) && shotTime == 0)
			{
				AnimatedSprite *playerBullet = new AnimatedSprite();
				Physics *physics = gsm->getPhysics();
				Bot *bot = new BasicBulletBot();
				physics->addCollidableObject(bot);
				PhysicalProperties *bulletProps = bot->getPhysicalProperties();
				bulletProps->setPosition(pp->getX(), pp->getY() + 50);
				bulletProps->setVelocity(-PLAYER_BULLET_SPEED, 0.0f);
				bulletProps->setSpriteType(4);
				bot->setSpriteType(playerbulletSpriteType);
				bot->setAlpha(255);
				bot->setCurrentState(IDLE);
				sm->addBot(bot);
				bot->affixTightAABBBoundingVolume();
				player->getPhysicalProperties()->setShot(30);
			}
			if (input->isKeyDownForFirstTime(K_KEY) && shotTime == 0)
			{
				AnimatedSprite *playerBullet = new AnimatedSprite();
				Physics *physics = gsm->getPhysics();
				Bot *bot = new BasicBulletBot();
				physics->addCollidableObject(bot);
				PhysicalProperties *bulletProps = bot->getPhysicalProperties();
				bulletProps->setPosition(pp->getX() + 46, pp->getY() + 110);
				bulletProps->setVelocity(0.0f, PLAYER_BULLET_SPEED);
				bulletProps->setSpriteType(4);
				bot->setSpriteType(playerbulletSpriteType);
				bot->setAlpha(255);
				bot->setCurrentState(IDLE);
				sm->addBot(bot);
				bot->affixTightAABBBoundingVolume();
				player->getPhysicalProperties()->setShot(30);
			}
			if (input->isKeyDownForFirstTime(L_KEY) && shotTime == 0)
			{
				AnimatedSprite *playerBullet = new AnimatedSprite();
				Physics *physics = gsm->getPhysics();
				Bot *bot = new BasicBulletBot();
				physics->addCollidableObject(bot);
				PhysicalProperties *bulletProps = bot->getPhysicalProperties();
				bulletProps->setPosition(pp->getX()+110, pp->getY() + 50);
				bulletProps->setVelocity(PLAYER_BULLET_SPEED, 0);
				bulletProps->setSpriteType(4);
				bot->setSpriteType(playerbulletSpriteType);
				bot->setAlpha(255);
				bot->setCurrentState(IDLE);
				sm->addBot(bot);
				bot->affixTightAABBBoundingVolume();
				player->getPhysicalProperties()->setShot(30);
			}
		}
		else{
			if (input->isKeyDownForFirstTime(I_KEY) && shotTime == 0)
			{
				AnimatedSprite *playerBullet = new AnimatedSprite();
				Physics *physics = gsm->getPhysics();
				Bot *bot = new BasicBulletBot();
				physics->addCollidableObject(bot);
				PhysicalProperties *bulletProps = bot->getPhysicalProperties();
				bulletProps->setPosition(pp->getX() + 70, pp->getY()+20);
				bulletProps->setVelocity(PLAYER_BULLET_SPEED, -PLAYER_BULLET_SPEED);
				bulletProps->setSpriteType(4);
				bot->setSpriteType(playerbulletSpriteType);
				bot->setAlpha(255);
				bot->setCurrentState(IDLE);
				sm->addBot(bot);
				bot->affixTightAABBBoundingVolume();
				player->getPhysicalProperties()->setShot(30);
			}
			if (input->isKeyDownForFirstTime(J_KEY) && shotTime == 0)
			{
				AnimatedSprite *playerBullet = new AnimatedSprite();
				Physics *physics = gsm->getPhysics();
				Bot *bot = new BasicBulletBot();
				physics->addCollidableObject(bot);
				PhysicalProperties *bulletProps = bot->getPhysicalProperties();
				bulletProps->setPosition(pp->getX() + 10, pp->getY() + 20);
				bulletProps->setVelocity(-PLAYER_BULLET_SPEED, -PLAYER_BULLET_SPEED);
				bulletProps->setSpriteType(4);
				bot->setSpriteType(playerbulletSpriteType);
				bot->setAlpha(255);
				bot->setCurrentState(IDLE);
				sm->addBot(bot);
				bot->affixTightAABBBoundingVolume();
				player->getPhysicalProperties()->setShot(30);
			}
			if (input->isKeyDownForFirstTime(K_KEY) && shotTime == 0)
			{
				AnimatedSprite *playerBullet = new AnimatedSprite();
				Physics *physics = gsm->getPhysics();
				Bot *bot = new BasicBulletBot();
				physics->addCollidableObject(bot);
				PhysicalProperties *bulletProps = bot->getPhysicalProperties();
				bulletProps->setPosition(pp->getX() + 20, pp->getY() + 90);
				bulletProps->setVelocity(-PLAYER_BULLET_SPEED, PLAYER_BULLET_SPEED);
				bulletProps->setSpriteType(4);
				bot->setSpriteType(playerbulletSpriteType);
				bot->setAlpha(255);
				bot->setCurrentState(IDLE);
				sm->addBot(bot);
				bot->affixTightAABBBoundingVolume();
				player->getPhysicalProperties()->setShot(30);
			}
			if (input->isKeyDownForFirstTime(L_KEY) && shotTime == 0)
			{
				AnimatedSprite *playerBullet = new AnimatedSprite();
				Physics *physics = gsm->getPhysics();
				Bot *bot = new BasicBulletBot();
				physics->addCollidableObject(bot);
				PhysicalProperties *bulletProps = bot->getPhysicalProperties();
				bulletProps->setPosition(pp->getX() + 70, pp->getY() + 90);
				bulletProps->setVelocity(PLAYER_BULLET_SPEED, PLAYER_BULLET_SPEED);
				bulletProps->setSpriteType(4);
				bot->setSpriteType(playerbulletSpriteType);
				bot->setAlpha(255);
				bot->setCurrentState(IDLE);
				sm->addBot(bot);
				bot->affixTightAABBBoundingVolume();
				player->getPhysicalProperties()->setShot(30);
			}
		}
		if (shotTime > 0){
			player->getPhysicalProperties()->setShot(player->getPhysicalProperties()->getShot() - 1);
		}

		bool viewportMoved = false;
		float viewportVx = 0.0f;
		float viewportVy = 0.0f;
		float playerDistX = 0.0f;
		float playerDistY = 0.0f;
		if (input->isKeyDown(UP_KEY))
		{
			viewportVy -= MAX_VIEWPORT_AXIS_VELOCITY;
			viewportMoved = true;
		}
		if (input->isKeyDown(DOWN_KEY))
		{
			viewportVy += MAX_VIEWPORT_AXIS_VELOCITY;
			viewportMoved = true;
		}
		if (input->isKeyDown(LEFT_KEY))
		{
			viewportVx -= MAX_VIEWPORT_AXIS_VELOCITY;
			viewportMoved = true;
		}
		if (input->isKeyDown(RIGHT_KEY))
		{
			viewportVx += MAX_VIEWPORT_AXIS_VELOCITY;
			viewportMoved = true;
		}
		if (!input->isKeyDown(UP_KEY) && !input->isKeyDown(DOWN_KEY) && !input->isKeyDown(LEFT_KEY) && !input->isKeyDown(RIGHT_KEY)){
			playerDistX = (viewport->getViewportX() + viewport->getViewportWidth() / 2) - (player->getPhysicalProperties()->getX());
			playerDistY = (viewport->getViewportY() + viewport->getViewportHeight() / 2.2) - (player->getPhysicalProperties()->getY());
			if (playerDistX > 30){
				viewportVx -= MAX_VIEWPORT_AXIS_VELOCITY;
			}
			else if (playerDistX < -30){
				viewportVx += MAX_VIEWPORT_AXIS_VELOCITY;
			}
			else viewportVx = player->getPhysicalProperties()->getVelocityX();
			if (playerDistY > 30){
				viewportVy -= MAX_VIEWPORT_AXIS_VELOCITY;
			}
			else if (playerDistY < -30){
				viewportVy += MAX_VIEWPORT_AXIS_VELOCITY;
			}
			else{
				viewportVy = player->getPhysicalProperties()->getVelocityY();
				viewportVx = player->getPhysicalProperties()->getVelocityX();
			}
			viewportMoved = true;
		}
		Viewport *viewport = game->getGUI()->getViewport();
		if (viewportMoved)
			viewport->moveViewport((int)floor(viewportVx+0.5f), (int)floor(viewportVy+0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
		
	}

	// 0X43 is HEX FOR THE 'C' VIRTUAL KEY
	// THIS CHANGES THE CURSOR IMAGE
	if ((input->isKeyDownForFirstTime(C_KEY))
		&& input->isKeyDown(VK_SHIFT))
	{
		Cursor *cursor = game->getGUI()->getCursor();
		unsigned int id = cursor->getActiveCursorID();
		id++;
		if (id == cursor->getNumCursorIDs())
			id = 0;		
		cursor->setActiveCursorID(id);
	}

	// LET'S MESS WITH THE TARGET FRAME RATE IF THE USER PRESSES THE HOME OR END KEYS
	WindowsTimer *timer = (WindowsTimer*)game->getTimer();
	int fps = timer->getTargetFPS();

	// THIS SPEEDS UP OUR GAME LOOP AND THUS THE GAME, NOTE THAT WE COULD ALTERNATIVELY SCALE
	// DOWN THE GAME LOGIC (LIKE ALL VELOCITIES) AS WE SPEED UP THE GAME. THAT COULD PROVIDE
	// A BETTER PLAYER EXPERIENCE
	if (input->isKeyDown(VK_HOME) && (fps < MAX_FPS))
		timer->setTargetFPS(fps + FPS_INC);

	// THIS SLOWS DOWN OUR GAME LOOP, BUT WILL NOT GO BELOW 5 FRAMES PER SECOND
	else if (input->isKeyDown(VK_END) && (fps > MIN_FPS))
		timer->setTargetFPS(fps - FPS_INC);
}