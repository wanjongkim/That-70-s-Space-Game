/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	BugginOutDataLoader.h

	This class provides a custom importer for the BugginOutGame to import
	game data, gui data, and world (i.e. level) data.
*/
#pragma once
#include "buggin_out_VS\stdafx.h"
#include "sssf\data_loader\GameDataLoader.h"
#include "sssf\game\Game.h"
#include "sssf\gsm\sprite\AnimatedSpriteType.h"
#include "sssf\platforms\DirectX\DirectXTextureManager.h"
#include "sssf\gsm\ai\bots\BasicBulletBot.h"
#include "sssf\gsm\ai\bots\ShootingBot.h"

class BugginOutDataLoader : public GameDataLoader
{
private:
	// WE NEED THESE GUYS TO INIT OUR WINDOWS WINDOW
	HINSTANCE hInstance;
	int nCmdShow;

public:
	BugginOutDataLoader()	{}
	~BugginOutDataLoader()	{}

	// INLINED MUTATOR METHOD
	void initWinHandle(HINSTANCE initHInstance, int initNCmdShow)
	{
		hInstance = initHInstance;
		nCmdShow = initNCmdShow;
	}
	

	// THESE SHOULD BE LOADING DATA FROM FILES. THEY
	// ARE ALL DEFINED IN BugginOutDataLoader.cpp
	void loadGame(Game *game, wstring gameInitFile);
	void loadGUI(Game *game, wstring guiInitFile);
	void loadWorld(Game *game, wstring levelInitFile);

	// THESE ARE HARD-CODED EXAMPLES OF GUI DATA LOADING
	void hardCodedLoadGUIExample(Game *game);

	// SOME HELPER METHODS FOR SETTING UP THE GAME
	void initColors(GameGraphics *graphics,	map<wstring,wstring> *properties);
	void initViewport(GameGUI *gui, map<wstring,wstring> *properties);
	void makeRandomJumpingBot(Game *game, AnimatedSpriteType *randomJumpingBotType, float initX, float initY);
	void makeShooterBot(Game *game, AnimatedSpriteType *shootingBotType, int spriteType, float initX, float initY, int shotCyc, int moveCyc, int moveDir);
	void makeNoAimBot(Game *game, AnimatedSpriteType *noAimBotType, int spriteType, float initX, float initY, int shotCyc, int shotDir);
	void makeHomingBot(Game *game, AnimatedSpriteType *homingBotType, int spriteType, float initX, float initY);

	// AND SOME HELPER METHODS FOR LOADING THE GUI
	void initCursor(GameGUI *gui, DirectXTextureManager *guiTextureManager);
	void initSplashScreen(Game *game, GameGUI *gui,	DirectXTextureManager *guiTextureManager);
	void initMainMenu(GameGUI *gui,	DirectXTextureManager *guiTextureManager);
	void initInGameGUI(GameGUI *gui, DirectXTextureManager *guiTextureManager);
	void initHelpScreen(Game*, GameGUI*, TextureManager*);
	void initAboutScreen(Game*, GameGUI*, TextureManager*);
};