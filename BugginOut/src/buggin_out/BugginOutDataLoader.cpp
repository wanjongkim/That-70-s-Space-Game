#include "buggin_out_VS\stdafx.h"

// BugginOut GAME INCLUDES
#include "buggin_out\BugginOutButtonEventHandler.h"
#include "buggin_out\BugginOutDataLoader.h"
#include "buggin_out\BugginOut.h"
#include "buggin_out\BugginOutKeyEventHandler.h"
#include "buggin_out\BugginOutTextGenerator.h"

// GAME OBJECT INCLUDES
#include "sssf\game\Game.h"
#include "sssf\graphics\GameGraphics.h"
#include "sssf\gsm\ai\bots\RandomJumpingBot.h"
#include "sssf\gsm\ai\bots\BasicBulletBot.h"
#include "sssf\gsm\ai\bots\ShootingBot.h"
#include "sssf\gsm\ai\bots\NoAimBot.h"
#include "sssf\gsm\ai\bots\HomingBot.h"
#include "sssf\gsm\state\GameState.h"
#include "sssf\gsm\world\TiledLayer.h"
#include "sssf\gui\Cursor.h"
#include "sssf\gui\GameGUI.h"
#include "sssf\gui\ScreenGUI.h"
#include "sssf\input\GameInput.h"
#include "sssf\os\GameOS.h"
#include "sssf\text\GameText.h"

// WINDOWS PLATFORM INCLUDES
#include "sssf\platforms\Windows\WindowsOS.h"
#include "sssf\platforms\Windows\WindowsInput.h"
#include "sssf\platforms\Windows\WindowsTimer.h"

// DIRECTX INCLUDES
#include "sssf\platforms\DirectX\DirectXGraphics.h"
#include "sssf\platforms\DirectX\DirectXTextureManager.h"

// TMX MAP LOADING
#include "tmxmi\TMXMapImporter.h"

// ANIMATED SPRITE TYPE LOADING
#include "psti\PoseurSpriteTypesImporter.h"
#include "Box2D\Box2D.h"

// FOR LUA SCRIPTING(LEVEL LOADING)
#include <iostream>

#include "LuaPlusFramework\LuaPlus.h"
using namespace LuaPlus;
#include "stdio.h"
using namespace std;

/*
	loadGame - This method loads the setup game data into the game and
	constructs all the needed objects for the game to work.
*/
void BugginOutDataLoader::loadGame(Game *game, wstring gameInitFile)
{
	// AND LET'S READ IN THE GAME SETUP INFO
	// FIRST LOAD ALL THE PROPERTIES
	map<wstring,wstring> *properties = new map<wstring,wstring>();
	loadGameProperties(game, properties, gameInitFile);

	// WE NEED THE TITLE AND USE_FULLSCREEN_MODE TO INITIALIZE OUR WINDOW
	wstring titleProp = (*properties)[W_TITLE];
	wstring useFullscreenProp = (*properties)[W_USE_FULLSCREEN_MODE];
	bool useFullscreen = false;
	if (useFullscreenProp.compare(L"true") == 0)
		useFullscreen = true;

	// GET THE SCREEN WIDTH AND HEIGHT
	int screenWidth, screenHeight;
	wstring screenWidthProp = (*properties)[W_SCREEN_WIDTH];
	wstring screenHeightProp = (*properties)[W_SCREEN_HEIGHT];
	wstringstream(screenWidthProp) >> screenWidth;
	wstringstream(screenHeightProp) >> screenHeight;

	// MAKE A CUSTOM GameOS OBJECT (WindowsOS) FOR SOME WINDOWS
	// PLATFORM STUFF, INCLUDING A Window OF COURSE
	WindowsOS *bugginOutOS = new WindowsOS(	hInstance, 
										nCmdShow,
										useFullscreen,
										titleProp,
										screenWidth, screenHeight,
										game);
	
	int textFontSize;
	wstring textFontSizeProp = (*properties)[W_TEXT_FONT_SIZE];
	wstringstream(textFontSizeProp) >> textFontSize;

	// RENDERING WILL BE DONE USING DirectX
	DirectXGraphics *bugginOutGraphics = new DirectXGraphics(game);
	bugginOutGraphics->init(screenWidth, screenHeight);
	bugginOutGraphics->initGraphics(bugginOutOS, useFullscreen);
	bugginOutGraphics->initTextFont(textFontSize);

	// AND NOW LOAD THE COLORS THE GRAPHICS WILL USE
	// AS A COLOR KEY AND FOR RENDERING TEXT
	initColors(bugginOutGraphics, properties);

	// WE'LL USE WINDOWS PLATFORM METHODS FOR GETTING INPUT
	WindowsInput *bugginOutInput = new WindowsInput();

	// AND WINDOWS FOR THE TIMER AS WELL
	WindowsTimer *bugginOutTimer = new WindowsTimer();

	// NOW INITIALIZE THE Game WITH ALL THE
	// PLATFORM AND GAME SPECIFIC DATA WE JUST CREATED
	game->initPlatformPlugins(	(GameGraphics*)bugginOutGraphics,
								(GameInput*)bugginOutInput,
								(GameOS*)bugginOutOS,
								(GameTimer*)bugginOutTimer);

	// LOAD OUR CUSTOM TEXT GENERATOR, WHICH DRAWS
	// TEXT ON THE SCREEN EACH FRAME
	BugginOutTextGenerator *bugginOutTextGenerator = new BugginOutTextGenerator();
	bugginOutTextGenerator->initText(game);
	GameText *text = game->getText();
	text->initDebugFile(W_DEBUG_FILE);
	text->setTextGenerator((TextGenerator*)bugginOutTextGenerator);

	// INIT THE VIEWPORT TOO
	initViewport(game->getGUI(), properties);	
	game->getGUI()->getViewport()->setViewportX(3776);

	// WE DON'T NEED THE PROPERTIES MAP ANYMORE, THE GAME IS ALL LOADED
	delete properties;
}

/*
	initColors - this helper method loads the color key, used for loading
	images, and the font color, used for rendering text.
*/
void BugginOutDataLoader::initColors(	GameGraphics *graphics,
									map<wstring,wstring> *properties)
{
	int fontRed, fontGreen, fontBlue;
	wstring fontRedProp = (*properties)[W_FONT_COLOR_RED];
	wstring fontGreenProp = (*properties)[W_FONT_COLOR_GREEN];
	wstring fontBlueProp = (*properties)[W_FONT_COLOR_BLUE];
	wstringstream(fontRedProp) >> fontRed;
	wstringstream(fontGreenProp) >> fontGreen;
	wstringstream(fontBlueProp) >> fontBlue;

	// COLOR USED FOR RENDERING TEXT
	graphics->setFontColor(fontRed, fontGreen, fontBlue);

	int keyRed, keyGreen, keyBlue;
	wstring keyRedProp = (*properties)[W_COLOR_KEY_RED];
	wstring keyGreenProp = (*properties)[W_COLOR_KEY_GREEN];
	wstring keyBlueProp = (*properties)[W_COLOR_KEY_BLUE];
	wstringstream(keyRedProp) >> keyRed;
	wstringstream(keyGreenProp) >> keyGreen;
	wstringstream(keyBlueProp) >> keyBlue;

	// COLOR KEY - COLOR TO BE IGNORED WHEN LOADING AN IMAGE
	// NOTE, IF YOU WISH TO USE PNG IMAGES THAT CONTAIN ALPHA
	// CHANNEL DATA, YOU DON'T NEED A COLOR KEY
	graphics->setColorKey(keyRed, keyGreen, keyBlue);
}

/*
	loadGUI - This method loads all the GUI assets described in the guiInitFile
	argument. Note that we are loading all GUI art for all GUIs when the application
	first starts. We'll learn a better technique later in the semester.
*/
void BugginOutDataLoader::loadGUI(Game *game, wstring guiInitFile)
{
	// WE'RE JUST GOING TO IGNORE THE GUI FILE FOR NOW.
	// FOR THE MOMENT WE ARE CALLING THIS HARD-CODED GUI LOADER
	hardCodedLoadGUIExample(game);
}

class ContactFilter : b2ContactFilter {
public:
	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
};

bool ContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) {
	const b2Filter& filterA = fixtureA->GetFilterData();
	const b2Filter& filterB = fixtureB->GetFilterData();

	if (filterA.groupIndex == filterB.groupIndex && filterA.groupIndex != 0)
	{
		return filterA.groupIndex > 0;
	}
	bool collide = (filterA.maskBits & filterB.categoryBits) != 0 && (filterA.categoryBits & filterB.maskBits) != 0;
	return collide;
}

class contactListener : public b2ContactListener {
	SpriteManager* spriteManager;

	b2World* bWorld;

public:
	void setWorld(b2World *world) { bWorld = world; };
	void setSpriteManager(SpriteManager *spriteManage) { spriteManager = spriteManage; };
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
	void setBot(AnimatedSprite *bot);
};



void contactListener::BeginContact(b2Contact* contact) {
	/*
	void* data = contact->GetFixtureA()->GetBody()->GetUserData();
	void* data2 = contact->GetFixtureB()->GetBody()->GetUserData();
	*/
	//Bot *bot = static_cast<Bot*>(data2);
	
	b2Body* body = contact->GetFixtureA()->GetBody();
	b2Body* body2 = contact->GetFixtureB()->GetBody();
	/*
	int a = *((int*)body->GetUserData());
	int b = *((int*)body2->GetUserData());
	*/
	
	AnimatedSprite* sprite1 = static_cast<AnimatedSprite*>(body->GetUserData());
	AnimatedSprite* sprite2 = static_cast<AnimatedSprite*>(body2->GetUserData());

	if (sprite1->getCurrentState() != L"DEAD" && sprite2->getCurrentState() != L"DEAD"){
		//Between player and enemies, and their bullets

		if (sprite1->getPhysicalProperties()->getSpriteType() == 0 && sprite2->getPhysicalProperties()->getSpriteType() == 1) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 0 && sprite2->getPhysicalProperties()->getSpriteType() == 2) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 0 && sprite2->getPhysicalProperties()->getSpriteType() == 3) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 0 && sprite2->getPhysicalProperties()->getSpriteType() == 5) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 0 && sprite2->getPhysicalProperties()->getSpriteType() == 6) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 0 && sprite2->getPhysicalProperties()->getSpriteType() == 7) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 0 && sprite2->getPhysicalProperties()->getSpriteType() == 8) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			sprite2->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}

		if (sprite1->getPhysicalProperties()->getSpriteType() == 1 && sprite2->getPhysicalProperties()->getSpriteType() == 0) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 2 && sprite2->getPhysicalProperties()->getSpriteType() == 0) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 3 && sprite2->getPhysicalProperties()->getSpriteType() == 0) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 5 && sprite2->getPhysicalProperties()->getSpriteType() == 0) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 6 && sprite2->getPhysicalProperties()->getSpriteType() == 0) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 7 && sprite2->getPhysicalProperties()->getSpriteType() == 0) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 8 && sprite2->getPhysicalProperties()->getSpriteType() == 0) {
			spriteManager->getPlayer()->setCurrentState(L"DEAD");
			sprite1->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}

		// Between good Bullets and enemies
		if (sprite1->getPhysicalProperties()->getSpriteType() == 4 && sprite2->getPhysicalProperties()->getSpriteType() == 1) {
			sprite2->setCurrentState(L"DEAD");
			sprite1->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body2);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 4 && sprite2->getPhysicalProperties()->getSpriteType() == 2) {
			sprite2->setCurrentState(L"DEAD");
			sprite1->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body2);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 4 && sprite2->getPhysicalProperties()->getSpriteType() == 3) {
			sprite2->setCurrentState(L"DEAD");
			sprite1->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body2);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 4 && sprite2->getPhysicalProperties()->getSpriteType() == 5) {
			sprite2->setCurrentState(L"DEAD");
			sprite1->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body2);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 1 && sprite2->getPhysicalProperties()->getSpriteType() == 4) {
			sprite1->setCurrentState(L"DEAD");
			sprite2->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 2 && sprite2->getPhysicalProperties()->getSpriteType() == 4) {
			sprite1->setCurrentState(L"DEAD");
			sprite2->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 3 && sprite2->getPhysicalProperties()->getSpriteType() == 4) {
			sprite1->setCurrentState(L"DEAD");
			sprite2->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 5 && sprite2->getPhysicalProperties()->getSpriteType() == 4) {
			sprite1->setCurrentState(L"DEAD");
			sprite2->setCurrentState(L"DEAD");
			//bWorld->DestroyBody(body);
		}

		// Between bad Bullets and enemies
		if (sprite1->getPhysicalProperties()->getSpriteType() == 8 && sprite2->getPhysicalProperties()->getSpriteType() == 1) {
			contact->SetEnabled(false);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 8 && sprite2->getPhysicalProperties()->getSpriteType() == 2) {
			contact->SetEnabled(false);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 8 && sprite2->getPhysicalProperties()->getSpriteType() == 3) {
			contact->SetEnabled(false);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 8 && sprite2->getPhysicalProperties()->getSpriteType() == 5) {
			contact->SetEnabled(false);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 1 && sprite2->getPhysicalProperties()->getSpriteType() == 8) {
			contact->SetEnabled(false);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 2 && sprite2->getPhysicalProperties()->getSpriteType() == 8) {
			contact->SetEnabled(false);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 3 && sprite2->getPhysicalProperties()->getSpriteType() == 8) {
			contact->SetEnabled(false);
		}
		if (sprite1->getPhysicalProperties()->getSpriteType() == 5 && sprite2->getPhysicalProperties()->getSpriteType() == 8) {
			contact->SetEnabled(false);
		}
	}

	//Bot *bots = static_cast<Bot*>(data);

	//	if (bots != NULL) {
			//enemy dies

			
			//bWorld->DestroyBody(contact->GetFixtureA()->GetBody());
		//}
		
}/*
	 if (contact->GetFixtureA()->GetBody()->GetUserData() == "enemy" && contact->GetFixtureB()->GetBody()->GetUserData() == "goodBullet") {
	 //enemy dies
	 spriteManager->
	 bWorld->DestroyBody(contact->GetFixtureA()->GetBody());
	 }
	 if (contact->GetFixtureA()->GetBody()->GetUserData() == "goodBullet" && contact->GetFixtureB()->GetBody()->GetUserData() == "enemy") {
	 //enemy dies
	 spriteManager->
	 bWorld->DestroyBody(contact->GetFixtureA()->GetBody());
	 }
	 if (contact->)
	 */


void contactListener::EndContact(b2Contact* contact) {

}

/*
	loadLevel - This method should load the data the level described by the
	levelInitFile argument in to the Game's game state manager.
*/
void BugginOutDataLoader::loadWorld(Game *game, wstring levelInitFile)	
{
	LuaState* luaPState = LuaState::Create();
	if (levelInitFile == L"level1") {
		int result = luaPState->DoFile("level1.lua");
	}
	else if (levelInitFile == L"level2") {
		int result = luaPState->DoFile("level2.lua");
	}
	else if (levelInitFile == L"level3") {
		int result = luaPState->DoFile("level3.lua");
	}
	// LOAD THE LEVEL'S BACKGROUND TILES
	TMXMapImporter tmxMapImporter;
	tmxMapImporter.loadWorld(game, W_LEVEL_1_DIR, W_LEVEL_1_NAME);

	// LOAD THE LEVEL'S SPRITE IMAGES
	PoseurSpriteTypesImporter psti;
	psti.loadSpriteTypes(game, SPRITE_TYPES_LIST);

	// LET'S MAKE A PLAYER SPRITE
	// @TODO - IT WOULD BE BETTER TO LOAD THIS STUFF FROM A FILE
	GameStateManager *gsm = game->getGSM();
	Physics *physics = gsm->getPhysics();
	physics->setGravity(W_GRAVITY);
	SpriteManager *spriteManager = gsm->getSpriteManager();
	AnimatedSprite *player = spriteManager->getPlayer();
	physics->addCollidableObject(player);

	// NOTE THAT RED BOX MAN IS SPRITE ID 2
	AnimatedSpriteType *playerSpriteType = spriteManager->getSpriteType(0);
	player->setSpriteType(playerSpriteType);
	player->setAlpha(255);
	player->setCurrentState(IDLE);
	PhysicalProperties *playerProps = player->getPhysicalProperties();
	playerProps->setX(PLAYER_INIT_X);
	playerProps->setY(PLAYER_INIT_Y);
	playerProps->setVelocity(0.0f, 0.0f);
	playerProps->setAccelerationX(0);
	playerProps->setAccelerationY(0);
	playerProps->setSpriteType(0);
	player->setOnTileThisFrame(false);
	player->setOnTileLastFrame(false);
	player->affixPlayerAABBBoundingVolume();

	b2World* bWorld = game->getGSM()->getb2World();
	b2BodyDef playerBodyDef;
	playerBodyDef.type = b2_dynamicBody; 
	LuaObject xObj = luaPState->GetGlobal("playerX");
	LuaObject yObj = luaPState->GetGlobal("playerY");
	float playerX = xObj.GetFloat();
	float playerY = yObj.GetFloat();
	playerBodyDef.position.Set(playerX, playerY);
	playerBodyDef.angle = 0;
	b2Body* dynamicBody = bWorld->CreateBody(&playerBodyDef);
	//b2PolygonShape boxShape;
	b2CircleShape circleShape;
	//boxShape.SetAsBox(1, 1);
	circleShape.m_radius = .3125;
	//int playerNum = 1;

	//b2FixtureDef boxFixtureDef;
	b2FixtureDef circleFixtureDef;
	circleFixtureDef.shape = &circleShape;
	circleFixtureDef.density = 1;
	dynamicBody->CreateFixture(&circleFixtureDef);
	dynamicBody->SetUserData(player);
	//dynamicBody->SetUserData(&playerNum);
	player->setBody(dynamicBody);

	AnimatedSpriteType *shootingLeftSpriteType = spriteManager->getSpriteType(1);
	AnimatedSpriteType *shootingRightSpriteType = spriteManager->getSpriteType(2);
	AnimatedSpriteType *shootingUpSpriteType = spriteManager->getSpriteType(3);
	AnimatedSpriteType *shootingDownSpriteType = spriteManager->getSpriteType(5);
	LuaObject shootObj = luaPState->GetGlobal("shooterSize");
	int shoot = shootObj.GetInteger();
	LuaObject noAimObj = luaPState->GetGlobal("noAimSize");
	int noAim = noAimObj.GetInteger();
	LuaObject homingObj = luaPState->GetGlobal("homingSize");
	int homing = homingObj.GetInteger();

	//SHOOTING BOTS
	for (int i = 1; i <= shoot; i++){
		char integer_string[4];
		sprintf_s(integer_string, "%d", i);
		char other_string[10] = "sprite"; 
		strcat_s(other_string, integer_string);
		LuaObject spriteObj = luaPState->GetGlobal(other_string);
		char nother_string[10] = "xPos";
		strcat_s(nother_string, integer_string);
		LuaObject xObj = luaPState->GetGlobal(nother_string);
		char mother_string[10] = "yPos";
		strcat_s(mother_string, integer_string);
		LuaObject yObj = luaPState->GetGlobal(mother_string);
		char lother_string[10] = "dir";
		strcat_s(lother_string, integer_string);
		LuaObject dirObj = luaPState->GetGlobal(lother_string);
		char kother_string[10] = "dur";
		strcat_s(kother_string, integer_string);
		LuaObject durObj = luaPState->GetGlobal(kother_string);
		int sprType = spriteObj.GetInteger();
		int xPos = xObj.GetInteger();
		int yPos = yObj.GetInteger();
		int moveDir = dirObj.GetInteger();
		int moveDur = durObj.GetInteger();
		AnimatedSpriteType* shootSpriteType = spriteManager->getSpriteType(sprType);
		makeShooterBot(game, shootSpriteType, sprType, xPos, yPos, 30, moveDur, moveDir);
	}

	//NO AIM BOTS
	for (int i = 1; i <= noAim; i++){
		char integer_string[4];
		sprintf_s(integer_string, "%d", i);
		char other_string[10] = "nsprite";
		strcat_s(other_string, integer_string);
		LuaObject spriteObj = luaPState->GetGlobal(other_string);
		char nother_string[10] = "nxPos";
		strcat_s(nother_string, integer_string);
		LuaObject xObj = luaPState->GetGlobal(nother_string);
		char mother_string[10] = "nyPos";
		strcat_s(mother_string, integer_string);
		LuaObject yObj = luaPState->GetGlobal(mother_string);
		char kother_string[10] = "ndir";
		strcat_s(kother_string, integer_string);
		LuaObject dirObj = luaPState->GetGlobal(kother_string);
		int sprType = spriteObj.GetInteger();
		int xPos = xObj.GetInteger();
		int yPos = yObj.GetInteger();
		int shotDir = dirObj.GetInteger();
		AnimatedSpriteType* noAimSpriteType = spriteManager->getSpriteType(sprType);
		makeNoAimBot(game, noAimSpriteType, sprType, xPos, yPos, 30, shotDir);
	}

	//HOMING BOTS
	for (int i = 1; i <= homing; i++){
		char integer_string[4];
		sprintf_s(integer_string, "%d", i);
		char other_string[10] = "hsprite";
		strcat_s(other_string, integer_string);
		LuaObject spriteObj = luaPState->GetGlobal(other_string);
		char nother_string[10] = "hxPos";
		strcat_s(nother_string, integer_string);
		LuaObject xObj = luaPState->GetGlobal(nother_string);
		char mother_string[10] = "hyPos";
		strcat_s(mother_string, integer_string);
		LuaObject yObj = luaPState->GetGlobal(mother_string);
		int sprType = spriteObj.GetInteger();
		int xPos = xObj.GetInteger();
		int yPos = yObj.GetInteger();
		AnimatedSpriteType* shootSpriteType = spriteManager->getSpriteType(sprType);
		makeHomingBot(game, shootSpriteType, sprType, xPos, yPos);
	}

	ContactFilter *filter = new ContactFilter();
	contactListener *contact = new contactListener();
	b2ContactFilter *filters = new b2ContactFilter();
	contact->setSpriteManager(spriteManager);
	contact->setWorld(bWorld);
	bWorld->SetContactListener(contact);
	//bWorld->SetContactFilter(filter);
	
}


// MAKE A BOT THAT MOVES IN A CERTAIN DIRECTION, FOR A CERTAIN NUMBER OF FRAMES, WITH A SHOT EVERY GIVEN NUMBER OF FRAMES
//DIRECTIONS: 1 = UP, 2 = RIGHT. 3 = DOWN, 4 = LEFT
void BugginOutDataLoader::makeShooterBot(Game *game, AnimatedSpriteType *shootingBotType, int spriteType, float initX, float initY, int shotCyc, int moveCyc, int moveDir)
{

	SpriteManager *spriteManager = game->getGSM()->getSpriteManager();
	Physics *physics = game->getGSM()->getPhysics();
	AnimatedSprite *bots = spriteManager->getBot();
	ShootingBot *bot = new ShootingBot(shotCyc, moveCyc, moveDir);
	physics->addCollidableObject(bot);
	PhysicalProperties *pp = bot->getPhysicalProperties();
	pp->setPosition(initX, initY);
	pp->setSpriteType(spriteType);
	bot->setSpriteType(shootingBotType);
	bot->setCurrentState(IDLE);
	bot->setAlpha(255);
	spriteManager->addBot(bot);
	bot->affixTightAABBBoundingVolume();
	

	b2World* bWorld = game->getGSM()->getb2World();
	b2BodyDef playerBodyDef;
	playerBodyDef.type = b2_dynamicBody;
	playerBodyDef.position.Set(initX / 64, (3200 - initY) / 64);
	playerBodyDef.angle = 0;
	b2Body* dynamicBody = bWorld->CreateBody(&playerBodyDef);
	b2PolygonShape boxShape;
	boxShape.SetAsBox(.5, .5);
	
	b2FixtureDef boxFixtureDef;
	boxFixtureDef.shape = &boxShape;
	boxFixtureDef.density = 1;
	//dynamicBody->GetFixtureList()->SetUserData("enemy");
	dynamicBody->CreateFixture(&boxFixtureDef);
	dynamicBody->SetUserData(bot);
	bot->setBody(dynamicBody);

	//boxFixtureDef.filter.groupIndex = -1;
}

// MAKE A BOT THAT SHOOTS IN A CERTAIN DIRECTION, WITH A SHOT EVERY GIVEN NUMBER OF FRAMES
//DIRECTIONS: 1 = UP, 2 = RIGHT. 3 = DOWN, 4 = LEFT
void BugginOutDataLoader::makeNoAimBot(Game *game, AnimatedSpriteType *noAimBotType, int spriteType, float initX, float initY, int shotCyc, int shotDir)
{
	SpriteManager *spriteManager = game->getGSM()->getSpriteManager();
	Physics *physics = game->getGSM()->getPhysics();
	NoAimBot *bot = new NoAimBot(shotCyc, shotDir);
	physics->addCollidableObject(bot);
	PhysicalProperties *pp = bot->getPhysicalProperties();
	pp->setPosition(initX, initY);
	pp->setSpriteType(spriteType);
	bot->setSpriteType(noAimBotType);
	bot->setCurrentState(IDLE);
	bot->setAlpha(255);
	spriteManager->addBot(bot);
	bot->affixTightAABBBoundingVolume();
	b2World* bWorld = game->getGSM()->getb2World();
	b2BodyDef playerBodyDef;
	playerBodyDef.type = b2_dynamicBody;
	playerBodyDef.position.Set(initX / 64, (3200 - initY) / 64);
	playerBodyDef.angle = 0;
	b2Body* dynamicBody = bWorld->CreateBody(&playerBodyDef);
	b2PolygonShape boxShape;
	boxShape.SetAsBox(.5, .5);

	b2FixtureDef boxFixtureDef;
	boxFixtureDef.shape = &boxShape;
	boxFixtureDef.density = 1;
	dynamicBody->CreateFixture(&boxFixtureDef);
	dynamicBody->SetUserData(bot);
	bot->setBody(dynamicBody);

}

// MAKE A BOT THAT FOLLOWS THE PLAYER
void BugginOutDataLoader::makeHomingBot(Game *game, AnimatedSpriteType *homingBotType, int spriteType, float initX, float initY)
{
	SpriteManager *spriteManager = game->getGSM()->getSpriteManager();
	Physics *physics = game->getGSM()->getPhysics();
	HomingBot *bot = new HomingBot();
	physics->addCollidableObject(bot);
	PhysicalProperties *pp = bot->getPhysicalProperties();
	pp->setPosition(initX, initY);
	pp->setSpriteType(spriteType);
	bot->setSpriteType(homingBotType);
	bot->setCurrentState(IDLE);
	bot->setAlpha(255);
	spriteManager->addBot(bot);
	bot->affixTightAABBBoundingVolume();
	b2World* bWorld = game->getGSM()->getb2World();
	b2BodyDef playerBodyDef;
	playerBodyDef.type = b2_dynamicBody;
	playerBodyDef.position.Set(initX / 64, (3200 - initY) / 64);
	playerBodyDef.angle = 0;
	b2Body* dynamicBody = bWorld->CreateBody(&playerBodyDef);
	b2PolygonShape boxShape;
	boxShape.SetAsBox(.5, .5);

	b2FixtureDef boxFixtureDef;
	boxFixtureDef.shape = &boxShape;
	boxFixtureDef.density = 1;
	dynamicBody->CreateFixture(&boxFixtureDef);
	dynamicBody->SetUserData(bot);
	bot->setBody(dynamicBody);

}


void BugginOutDataLoader::makeRandomJumpingBot(Game *game, AnimatedSpriteType *randomJumpingBotType, float initX, float initY)
{
	SpriteManager *spriteManager = game->getGSM()->getSpriteManager();
	Physics *physics = game->getGSM()->getPhysics();
	RandomJumpingBot *bot = new RandomJumpingBot(physics, 30, 120, 40);
	physics->addCollidableObject(bot);
	PhysicalProperties *pp = bot->getPhysicalProperties();
	pp->setPosition(initX, initY);
	bot->setSpriteType(randomJumpingBotType);
	bot->setCurrentState(IDLE);
	bot->setAlpha(255);
	spriteManager->addBot(bot);
	bot->affixTightAABBBoundingVolume();
}

/*
	initBugginOutGUI - This method builds a GUI for the BugginOut Game application.
	Note that we load all the GUI components from this method, including
	the ScreenGUI with Buttons and Overlays and the Cursor.
*/
void BugginOutDataLoader::hardCodedLoadGUIExample(Game *game)
{
	GameGUI *gui = game->getGUI();
	GameGraphics *graphics = game->getGraphics();
	DirectXTextureManager *guiTextureManager = (DirectXTextureManager*)graphics->getGUITextureManager();

	// SETUP THE CURSOR VIA OUR HELPER METHOD
	initCursor(gui, guiTextureManager);
	initSplashScreen(game, gui, guiTextureManager);
	initMainMenu(gui, guiTextureManager);
	initInGameGUI(gui, guiTextureManager);
	initHelpScreen(game, gui, guiTextureManager);
	initAboutScreen(game, gui, guiTextureManager);
}

/*
initHelpScreen - initializes the game's help screen gui.
*/
void BugginOutDataLoader::initHelpScreen(Game* game, GameGUI* gui,
	TextureManager* guiTextureManager) {
	// NOW, FIRST LET'S ADD A SPLASH SCREEN GUI
	ScreenGUI* helpScreenGUI = new ScreenGUI();

	// WE'LL ONLY HAVE ONE IMAGE FOR OUR GIANT BUTTON
	size_t normalTextureID = guiTextureManager->loadTexture(W_HELP_SCREEN_PATH);
	size_t mouseOverTextureID = normalTextureID;

	// INIT THE QUIT BUTTON
	Button* buttonToAdd = new Button();
	buttonToAdd->initButton(
		normalTextureID,
		mouseOverTextureID,
		0,
		0,
		0,
		255,
		game->getGraphics()->getScreenWidth(),
		game->getGraphics()->getScreenHeight(),
		false,
		W_GO_TO_MM_COMMAND);
	helpScreenGUI->addButton(buttonToAdd);

	// AND REGISTER IT WITH THE GUI
	gui->addScreenGUI(GameState::GS_MENU_HELP_MENU, helpScreenGUI);
}

/*
initAboutScreen - initializes the game's help screen gui.
*/
void BugginOutDataLoader::initAboutScreen(Game* game, GameGUI* gui,
	TextureManager* guiTextureManager) {
	// NOW, FIRST LET'S ADD A SPLASH SCREEN GUI
	ScreenGUI* aboutScreenGUI = new ScreenGUI();

	// WE'LL ONLY HAVE ONE IMAGE FOR OUR GIANT BUTTON
	size_t normalTextureID = guiTextureManager->loadTexture(W_ABOUT_SCREEN_PATH);
	size_t mouseOverTextureID = normalTextureID;

	// INIT THE QUIT BUTTON
	Button* buttonToAdd = new Button();
	buttonToAdd->initButton(
		normalTextureID,
		mouseOverTextureID,
		0,
		0,
		0,
		255,
		game->getGraphics()->getScreenWidth(),
		game->getGraphics()->getScreenHeight(),
		false,
		W_GO_TO_MM_COMMAND);
	aboutScreenGUI->addButton(buttonToAdd);

	// AND REGISTER IT WITH THE GUI
	gui->addScreenGUI(GameState::GS_MENU_ABOUT_MENU, aboutScreenGUI);
}

/*
	initCursor - initializes a simple little cursor for the gui.
*/
void BugginOutDataLoader::initCursor(GameGUI *gui, DirectXTextureManager *guiTextureManager)
{
	// SETUP THE CURSOR
	vector<unsigned int> *imageIDs = new vector<unsigned int>();
	int imageID;

	// - FIRST LOAD THE GREEN CURSOR IMAGE
	imageID = guiTextureManager->loadTexture(W_GREEN_CURSOR_PATH);
	imageIDs->push_back(imageID);

	// - AND NOW THE RED ONE
	imageID = guiTextureManager->loadTexture(W_RED_CURSOR_PATH);
	imageIDs->push_back(imageID);

	// - NOW BUILD AND LOAD THE CURSOR
	Cursor *cursor = new Cursor();
	cursor->initCursor(	imageIDs,
						*(imageIDs->begin()),
						0,
						0,
						0,
						255,
						32,
						32);
	gui->setCursor(cursor);
}

/*
	initSplashScreen - initializes the game's splash screen gui.
*/
void BugginOutDataLoader::initSplashScreen(Game *game, GameGUI *gui,	DirectXTextureManager *guiTextureManager)
{
	// NOW, FIRST LET'S ADD A SPLASH SCREEN GUI
	ScreenGUI *splashScreenGUI = new ScreenGUI();

	// WE'LL ONLY HAVE ONE IMAGE FOR OUR GIANT BUTTON
	unsigned int normalTextureID = guiTextureManager->loadTexture(W_SPLASH_SCREEN_PATH);
	unsigned int mouseOverTextureID = normalTextureID;

	// INIT THE QUIT BUTTON
	Button *buttonToAdd = new Button();
	buttonToAdd->initButton(normalTextureID, 
							mouseOverTextureID,
							0,
							0,
							0,
							255,
							game->getGraphics()->getScreenWidth(),
							game->getGraphics()->getScreenHeight(),
							false,
							W_GO_TO_MM_COMMAND);
	splashScreenGUI->addButton(buttonToAdd);

	// AND REGISTER IT WITH THE GUI
	gui->addScreenGUI(GS_SPLASH_SCREEN, splashScreenGUI);
}

/*
initMainMenu - initializes the game's main menu gui.
*/
void BugginOutDataLoader::initMainMenu(GameGUI* gui,
	DirectXTextureManager* guiTextureManager) {
	// NOW LET'S LOAD A MAIN MENU GUI SCREEN
	ScreenGUI* mainMenuGUI = new ScreenGUI();
	size_t imageID = guiTextureManager->loadTexture(W_MAIN_MENU_PATH);
	OverlayImage *imageToAdd = new OverlayImage();
	imageToAdd->x = 0;
	imageToAdd->y = 0;
	imageToAdd->z = 0;
	imageToAdd->alpha = 200;
	imageToAdd->width = 1366;
	imageToAdd->height = 768;
	imageToAdd->imageID = imageID;
	mainMenuGUI->addOverlayImage(imageToAdd);

	// - GET THE BUTTON COMMAND AND IMAGE IDs
	size_t normalTextureID = guiTextureManager->loadTexture(W_EXIT_IMAGE_PATH);
	size_t mouseOverTextureID = guiTextureManager->loadTexture(W_EXIT_IMAGE_MO_PATH);

	// - INIT THE EXIT BUTTON
	Button* buttonToAdd = new Button();
	buttonToAdd->initButton(
		normalTextureID,
		mouseOverTextureID,
		60,
		400,
		0,
		255,
		217,
		92,
		false,
		W_EXIT_COMMAND
		);

	// AND NOW LOAD IT INTO A ScreenGUI
	mainMenuGUI->addButton(buttonToAdd);

	// - GET THE BUTTON COMMAND AND IMAGE IDs

	normalTextureID = guiTextureManager->loadTexture(W_START_IMAGE_PATH);
	mouseOverTextureID = guiTextureManager->loadTexture(W_START_IMAGE_MO_PATH);

	// - INIT THE START BUTTON
	buttonToAdd = new Button();
	buttonToAdd->initButton(
		normalTextureID,
		mouseOverTextureID,
		60,
		212,
		0,
		255,
		221,
		89,
		false,
		W_START_COMMAND
		);

	// AND NOW LOAD IT INTO A ScreenGUI
	mainMenuGUI->addButton(buttonToAdd);

	// - GET THE BUTTON COMMAND AND IMAGE IDs

	normalTextureID = guiTextureManager->loadTexture(W_HELP_IMAGE_PATH);
	mouseOverTextureID = guiTextureManager->loadTexture(W_HELP_IMAGE_MO_PATH);

	// - INIT THE HELP BUTTON
	buttonToAdd = new Button();
	buttonToAdd->initButton(
		normalTextureID,
		mouseOverTextureID,
		352,
		589,
		0,
		255,
		209,
		102,
		false,
		W_HELP_COMMAND
		);

	// AND NOW LOAD IT INTO A ScreenGUI
	mainMenuGUI->addButton(buttonToAdd);

	// - GET THE BUTTON COMMAND AND IMAGE IDs

	normalTextureID = guiTextureManager->loadTexture(W_ABOUT_IMAGE_PATH);
	mouseOverTextureID = guiTextureManager->loadTexture(W_ABOUT_IMAGE_MO_PATH);

	// - INIT THE ABOUT BUTTON
	buttonToAdd = new Button();
	buttonToAdd->initButton(
		normalTextureID,
		mouseOverTextureID,
		344,
		669,
		0,
		255,
		279,
		93,
		false,
		W_ABOUT_COMMAND
		);

	// AND NOW LOAD IT INTO A ScreenGUI
	mainMenuGUI->addButton(buttonToAdd);

	// AND LET'S ADD OUR SCREENS
	gui->addScreenGUI(GameState::GS_MAIN_MENU, mainMenuGUI);
}

/*
	initInGameGUI - initializes the game's in-game gui.
*/
void BugginOutDataLoader::initInGameGUI(GameGUI *gui, DirectXTextureManager *guiTextureManager)
{
	// NOW ADD THE IN-GAME GUI
	ScreenGUI *inGameGUI = new ScreenGUI();

	unsigned int normalTextureID = guiTextureManager->loadTexture(W_QUIT_IMAGE_PATH);
	unsigned int mouseOverTextureID = guiTextureManager->loadTexture(W_QUIT_IMAGE_MO_PATH);

	// INIT THE QUIT BUTTON
	Button *buttonToAdd = new Button();
	buttonToAdd->initButton(normalTextureID, 
							mouseOverTextureID,
							0,
							0,
							0,
							255,
							200,
							100,
							false,
							W_QUIT_COMMAND);
	inGameGUI->addButton(buttonToAdd);

	// AND LET'S ADD OUR SCREENS
	gui->addScreenGUI(GS_GAME_IN_PROGRESS,	inGameGUI);
}

/*
	initViewport - initializes the game's viewport.
*/
void BugginOutDataLoader::initViewport(GameGUI *gui, map<wstring,wstring> *properties)
{
	// AND NOW SPECIFY THE VIEWPORT SIZE AND LOCATION. NOTE THAT IN THIS EXAMPLE,
	// WE ARE PUTTING A TOOLBAR WITH A BUTTON ACCROSS THE NORTH OF THE APPLICATION.
	// THAT TOOLBAR HAS A HEIGHT OF 64 PIXELS, SO WE'LL MAKE THAT THE OFFSET FOR
	// THE VIEWPORT IN THE Y AXIS
	Viewport *viewport = gui->getViewport();

	int viewportOffsetX, viewportOffsetY, toggleOffsetY, screenWidth, screenHeight;
	wstring viewportOffsetXProp = (*properties)[W_VIEWPORT_OFFSET_X];
	wstring viewportOffsetYProp = (*properties)[W_VIEWPORT_OFFSET_Y];
	wstring toggleOffsetYProp = (*properties)[W_TOGGLE_OFFSET_Y];
	wstring screenWidthProp = (*properties)[W_SCREEN_WIDTH];
	wstring screenHeightProp = (*properties)[W_SCREEN_HEIGHT];
	wstringstream(viewportOffsetXProp) >> viewportOffsetX;
	wstringstream(viewportOffsetYProp) >> viewportOffsetY;
	wstringstream(toggleOffsetYProp) >> toggleOffsetY;
	wstringstream(screenWidthProp) >> screenWidth;
	wstringstream(screenHeightProp) >> screenHeight;
	int viewportWidth = screenWidth - viewportOffsetX;
	int viewportHeight = screenHeight - viewportOffsetY;
	viewport->setViewportWidth(viewportWidth);
	viewport->setViewportHeight(viewportHeight);
	viewport->setViewportOffsetX(viewportOffsetX);
	viewport->setViewportOffsetY(viewportOffsetY);
	viewport->setToggleOffsetY(toggleOffsetY);
}