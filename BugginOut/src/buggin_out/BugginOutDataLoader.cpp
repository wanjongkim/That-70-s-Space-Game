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

/*
	loadLevel - This method should load the data the level described by the
	levelInitFile argument in to the Game's game state manager.
*/
void BugginOutDataLoader::loadWorld(Game *game, wstring levelInitFile)	
{
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
	playerBodyDef.position.Set(69, 48.4375);
	playerBodyDef.angle = 0;
	b2Body* dynamicBody = bWorld->CreateBody(&playerBodyDef);
	b2PolygonShape boxShape;
	boxShape.SetAsBox(1, 1);
	
	b2FixtureDef boxFixtureDef;
	boxFixtureDef.shape = &boxShape;
	boxFixtureDef.density = 1;
	dynamicBody->CreateFixture(&boxFixtureDef);
	dynamicBody->SetUserData(player);
	player->setBody(dynamicBody);

	AnimatedSpriteType *shootingLeftSpriteType = spriteManager->getSpriteType(1);
	AnimatedSpriteType *shootingRightSpriteType = spriteManager->getSpriteType(2);
	AnimatedSpriteType *shootingUpSpriteType = spriteManager->getSpriteType(3);
	AnimatedSpriteType *shootingDownSpriteType = spriteManager->getSpriteType(5);
	makeShooterBot(game, shootingUpSpriteType, 3, 4416, 1000, 30, 50, 2); 
	/*makeShooterBot(game, shootingLeftSpriteType, 1, 1536, 128, 30, 70, 3);
	makeShooterBot(game, shootingRightSpriteType, 2, 30, 768, 30, 70, 3);
	makeShooterBot(game, shootingUpSpriteType, 3, 1152, 800, 30, 50, 2);
	makeShooterBot(game, shootingLeftSpriteType, 1, 3136, 128, 30, 50, 3);
	makeShooterBot(game, shootingRightSpriteType, 2, 30, 1300, 30, 45, 3);
	makeShooterBot(game, shootingDownSpriteType, 5, 896, 1218, 30, 70, 2);
	makeShooterBot(game, shootingLeftSpriteType, 1, 2176, 1400, 30, 70, 1);
    makeShooterBot(game, shootingLeftSpriteType, 1, 2304, 448, 30, 10, 3);
	makeShooterBot(game, shootingRightSpriteType, 2, 2370, 400, 30, 12, 3);
	makeShooterBot(game, shootingUpSpriteType, 3, 2304, 1856, 30, 40, 2);
	makeShooterBot(game, shootingLeftSpriteType, 1, 2880, 1280, 30, 10, 1);*/


}

// MAKE A BOT THAT MOVES IN A CERTAIN DIRECTION, FOR A CERTAIN NUMBER OF FRAMES, WITH A SHOT EVERY GIVEN NUMBER OF FRAMES
//DIRECTIONS: 1 = UP, 2 = RIGHT. 3 = DOWN, 4 = LEFT
void BugginOutDataLoader::makeShooterBot(Game *game, AnimatedSpriteType *shootingBotType, int spriteType, float initX, float initY, int shotCyc, int moveCyc, int moveDir)
{
	SpriteManager *spriteManager = game->getGSM()->getSpriteManager();
	Physics *physics = game->getGSM()->getPhysics();
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