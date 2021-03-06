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
#include "sssf\audio\AudioManager.h"
#include "Box2D\Box2D.h"
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
	AudioManager* am = gsm->getAudioManager();
	
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
		if (player->getCurrentState() != DEAD){
			if (input->isKeyDown(CONTROL_KEY)){
				if (input->isKeyDownForFirstTime(V_KEY)){
					bool in = player->getPhysicalProperties()->getInvincible();
					player->getPhysicalProperties()->setInvincible(!in);
				}
			}
			
			if (input->isKeyDownForFirstTime(R_KEY))
			{
				player->getPhysicalProperties()->swapFireDir();
			}

			if (input->isKeyDown(A_KEY))
			{
				if (pp->getX() >= viewport->getViewportX() + 265){
					vX = -PLAYER_SPEED;
					player->setCurrentState(MOVE_LEFT);
				}
				else vX = 0;
			}
			if (input->isKeyDown(D_KEY))
			{
				if (pp->getX() <= viewport->getViewportX() + 1024){
					vX = PLAYER_SPEED;
					player->setCurrentState(MOVE_RIGHT);
				}
				else vX = 0;
			}
			if (input->isKeyDown(S_KEY))
			{
				if (pp->getY() <= viewport->getViewportY() + 520){
					vY = -PLAYER_SPEED;
					player->setCurrentState(MOVE_DOWN);
				}
				else vY = 0;
			}
			if (input->isKeyDown(W_KEY))
			{
				if (pp->getY() >= viewport->getViewportY() + 50){
					vY = PLAYER_SPEED;
					player->setCurrentState(MOVE_UP);
				}
				else vY = 0;
			}
			if (input->isKeyDownForFirstTime(G_KEY))
			{
				viewport->toggleDebugView();
				game->getGraphics()->toggleDebugTextShouldBeRendered();
			}
			if (!input->isKeyDown(W_KEY) && !input->isKeyDown(S_KEY))
			{
				vY = 0.0f;
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
			player->getBody()->SetLinearVelocity(b2Vec2(vX + viewport->getVx(), vY - viewport->getVy()));


			//PLAYER SHOOTING
			int shotTime = player->getPhysicalProperties()->getShot();
			if (player->getPhysicalProperties()->getFireDir()){
				if (input->isKeyDownForFirstTime(UP_KEY) && shotTime == 0)
				{
					AnimatedSprite *playerBullet = new AnimatedSprite();
					Physics *physics = gsm->getPhysics();
					Bot *bot = new BasicBulletBot();
					physics->addCollidableObject(bot);
					PhysicalProperties *bulletProps = bot->getPhysicalProperties();
					bulletProps->setPosition(pp->getX() + 46, pp->getY());
					bulletProps->setVelocity(0.0f, -PLAYER_BULLET_SPEED);
					bulletProps->setSpriteType(4);
					bot->setSpriteType(playerbulletSpriteType);
					bot->setAlpha(255);
					bot->setCurrentState(IDLE);
					sm->addBot(bot);
					bot->affixTightAABBBoundingVolume();
					b2World* bWorld = game->getGSM()->getb2World();
					b2BodyDef botBodyDef;
					botBodyDef.type = b2_dynamicBody;
					botBodyDef.position.Set((pp->getX() + 46) / 64, (3200 - pp->getY()+7) / 64);
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
					dynamicBody->SetLinearVelocity(b2Vec2(0, PLAYER_BULLET_SPEED));
					player->getPhysicalProperties()->setShot(15);
					am->playSound(CUE_PLAYER_FIRE);
				}
				if (input->isKeyDownForFirstTime(LEFT_KEY) && shotTime == 0)
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
					b2World* bWorld = game->getGSM()->getb2World();
					b2BodyDef botBodyDef;
					botBodyDef.type = b2_dynamicBody;
					botBodyDef.position.Set((pp->getX()-20) / 64, (3200 - (pp->getY() + 50)) / 64);
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
					dynamicBody->SetLinearVelocity(b2Vec2(-PLAYER_BULLET_SPEED, 0));
					player->getPhysicalProperties()->setShot(15);
					am->playSound(CUE_PLAYER_FIRE);
				}
				if (input->isKeyDownForFirstTime(DOWN_KEY) && shotTime == 0)
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
					b2World* bWorld = game->getGSM()->getb2World();
					b2BodyDef botBodyDef;
					botBodyDef.type = b2_dynamicBody;
					botBodyDef.position.Set((pp->getX() + 46) / 64, (3200 - (pp->getY() + 110 + 35)) / 64);
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
					dynamicBody->SetLinearVelocity(b2Vec2(0, -PLAYER_BULLET_SPEED));
					player->getPhysicalProperties()->setShot(15);
					am->playSound(CUE_PLAYER_FIRE);
				}
				if (input->isKeyDownForFirstTime(RIGHT_KEY) && shotTime == 0)
				{
					AnimatedSprite *playerBullet = new AnimatedSprite();
					Physics *physics = gsm->getPhysics();
					Bot *bot = new BasicBulletBot();
					physics->addCollidableObject(bot);
					PhysicalProperties *bulletProps = bot->getPhysicalProperties();
					bulletProps->setPosition(pp->getX() + 110, pp->getY() + 50);
					bulletProps->setVelocity(PLAYER_BULLET_SPEED, 0);
					bulletProps->setSpriteType(4);
					bot->setSpriteType(playerbulletSpriteType);
					bot->setAlpha(255);
					bot->setCurrentState(IDLE);
					sm->addBot(bot);
					bot->affixTightAABBBoundingVolume();
					b2World* bWorld = game->getGSM()->getb2World();
					b2BodyDef botBodyDef;
					botBodyDef.type = b2_dynamicBody;
					botBodyDef.position.Set((pp->getX() + 110 +35) / 64, (3200 - (pp->getY() + 50)) / 64);
					botBodyDef.angle = 0;
					b2Body* dynamicBody = bWorld->CreateBody(&botBodyDef);
					b2PolygonShape boxShape;
					boxShape.SetAsBox(.125, .125);
					b2FixtureDef boxFixtureDef;
					boxFixtureDef.shape = &boxShape;
					boxFixtureDef.density = 1;
					dynamicBody->CreateFixture(&boxFixtureDef);
					bot->setBody(dynamicBody);
					dynamicBody->SetUserData(bot);
					dynamicBody->SetLinearDamping(0);
					dynamicBody->SetLinearVelocity(b2Vec2(PLAYER_BULLET_SPEED, 0));
					player->getPhysicalProperties()->setShot(15);
					am->playSound(CUE_PLAYER_FIRE);
				}
			}
			else{
				if (input->isKeyDownForFirstTime(UP_KEY) && shotTime == 0)
				{
					AnimatedSprite *playerBullet = new AnimatedSprite();
					Physics *physics = gsm->getPhysics();
					Bot *bot = new BasicBulletBot();
					physics->addCollidableObject(bot);
					PhysicalProperties *bulletProps = bot->getPhysicalProperties();
					bulletProps->setPosition(pp->getX() + 70, pp->getY() + 20);
					bulletProps->setVelocity(PLAYER_BULLET_SPEED, -PLAYER_BULLET_SPEED);
					bulletProps->setSpriteType(4);
					bot->setSpriteType(playerbulletSpriteType);
					bot->setAlpha(255);
					bot->setCurrentState(IDLE);
					sm->addBot(bot);
					bot->affixTightAABBBoundingVolume();
					b2World* bWorld = game->getGSM()->getb2World();
					b2BodyDef botBodyDef;
					botBodyDef.type = b2_dynamicBody;
					botBodyDef.position.Set((pp->getX() + 70) / 64, (3200 - (pp->getY() + 20)) / 64);
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
					dynamicBody->SetLinearVelocity(b2Vec2(PLAYER_BULLET_SPEED, PLAYER_BULLET_SPEED));
					player->getPhysicalProperties()->setShot(15);
					am->playSound(CUE_PLAYER_FIRE);
				}
				if (input->isKeyDownForFirstTime(LEFT_KEY) && shotTime == 0)
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
					b2World* bWorld = game->getGSM()->getb2World();
					b2BodyDef botBodyDef;
					botBodyDef.type = b2_dynamicBody;
					botBodyDef.position.Set((pp->getX() + 10) / 64, (3200 - (pp->getY() + 20)) / 64);
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
					dynamicBody->SetLinearVelocity(b2Vec2(-PLAYER_BULLET_SPEED, PLAYER_BULLET_SPEED));
					player->getPhysicalProperties()->setShot(15);
					am->playSound(CUE_PLAYER_FIRE);
				}
				if (input->isKeyDownForFirstTime(DOWN_KEY) && shotTime == 0)
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
					b2World* bWorld = game->getGSM()->getb2World();
					b2BodyDef botBodyDef;
					botBodyDef.type = b2_dynamicBody;
					botBodyDef.position.Set((pp->getX() + 20) / 64, (3200 - (pp->getY() + 90)) / 64);
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
					dynamicBody->SetLinearVelocity(b2Vec2(-PLAYER_BULLET_SPEED, -PLAYER_BULLET_SPEED));
					player->getPhysicalProperties()->setShot(15);
					am->playSound(CUE_PLAYER_FIRE);
				}
				if (input->isKeyDownForFirstTime(RIGHT_KEY) && shotTime == 0)
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
					b2World* bWorld = game->getGSM()->getb2World();
					b2BodyDef botBodyDef;
					botBodyDef.type = b2_dynamicBody;
					botBodyDef.position.Set((pp->getX() + 70) / 64, (3200 - (pp->getY() + 90)) / 64);
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
					dynamicBody->SetLinearVelocity(b2Vec2(PLAYER_BULLET_SPEED, -PLAYER_BULLET_SPEED));
					player->getPhysicalProperties()->setShot(15);
					am->playSound(CUE_PLAYER_FIRE);
				}
			}
			if (shotTime > 0){
				player->getPhysicalProperties()->setShot(player->getPhysicalProperties()->getShot() - 1);
			}
		}
		bool viewportMoved = false;
		float viewportVx = 0.0f;
		float viewportVy = 0.0f;
		float playerDistX = 0.0f;
		float playerDistY = 0.0f;
		if (input->isKeyDown(I_KEY))
		{
			viewportVy -= MAX_VIEWPORT_AXIS_VELOCITY;
			viewportMoved = true;
		}
		if (input->isKeyDown(K_KEY))
		{
			viewportVy += MAX_VIEWPORT_AXIS_VELOCITY;
			viewportMoved = true;
		}
		if (input->isKeyDown(J_KEY))
		{
			viewportVx -= MAX_VIEWPORT_AXIS_VELOCITY;
			viewportMoved = true;
		}
		if (input->isKeyDown(L_KEY))
		{
			viewportVx += MAX_VIEWPORT_AXIS_VELOCITY;
			viewportMoved = true;
		}
		/*if (!input->isKeyDown(I_KEY) && !input->isKeyDown(K_KEY) && !input->isKeyDown(J_KEY) && !input->isKeyDown(L_KEY)){
			playerDistX = (viewport->getViewportX() + viewport->getViewportWidth() / 2) - (player->getPhysicalProperties()->getX());
			playerDistY = (viewport->getViewportY() + viewport->getViewportHeight() / 2.2) - (player->getPhysicalProperties()->getY());
			if (playerDistX > 30){
				viewportVx -= MAX_VIEWPORT_AXIS_VELOCITY;
			}
			else if (playerDistX < -30){
				viewportVx += MAX_VIEWPORT_AXIS_VELOCITY;
			}
			else {
				viewportVy = player->getPhysicalProperties()->getVelocityY();
				viewportVx = player->getPhysicalProperties()->getVelocityX();
			}
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
		}*/
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

	handleSpawnEvents(game);
	}
	
		// USES THE CURRENT LEVEL AND THE VIEWPORT POSITION TO SPAWN ENEMIES IF NECESSARY
		void BugginOutKeyEventHandler::handleSpawnEvents(Game *game){
		if (game->getGSM()->getWorld()->getLevel() == 1){
			if (spawn){
				
					
			}
			if (!spawn){
				
					
			}
			
		}
	}