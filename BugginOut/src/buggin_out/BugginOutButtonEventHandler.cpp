/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	BugginOutButtonEventHandler.cpp

	See BugginOutButtonEventHandler.h for a class description.
*/

#include "buggin_out_VS\stdafx.h"
#include "buggin_out\BugginOut.h"
#include "buggin_out\BugginOutButtonEventHandler.h"
#include "sssf\game\Game.h"
#include "sssf\gsm\state\GameStateManager.h"

void BugginOutButtonEventHandler::handleButtonEvents(	Game *game, 
													wstring command)
{
	// THE USER PRESSED THE Exit BUTTON ON THE MAIN MENU,
	// SO LET'S SHUTDOWN THE ENTIRE APPLICATION
	if (command.compare(W_EXIT_COMMAND) == 0)
	{
		game->shutdown();
	}
	// THE USER PRESSED THE MOUSE BUTTON ON THE SPLASH
	// SCREEN, SO LET'S GO TO THE MAIN MENU
	else if (command.compare(W_GO_TO_MM_COMMAND) == 0)
	{
		GameStateManager *gsm = game->getGSM();
		gsm->goToMainMenu();
	}
	// THE USER PRESSED THE Start BUTTON ON THE MAIN MENU,
	// SO LET'S START THE GAME FROM THE FIRST LEVEL
	else if (command.compare(W_START_COMMAND) == 0)
	{
		game->setCurrentLevelFileName(W_LEVEL_1_NAME);
		game->getGSM()->getAudioManager()->stopSound(CUE_TITLE_MUSIC);
		game->getGSM()->getAudioManager()->playSound(CUE_LEVEL_3_MUSIC);
		game->startGame();
	}
	// THE USER PRESSED THE Quit BUTTON ON THE IN-GAME MENU,
	// SO LET'S UNLOAD THE LEVEL AND RETURN TO THE MAIN MENU
	else if (command.compare(W_QUIT_COMMAND) == 0)
	{
		// Stop the ingame music
		game->getGSM()->getAudioManager()->stopSound(CUE_LEVEL_1_MUSIC);
		game->getGSM()->getAudioManager()->stopSound(CUE_LEVEL_2_MUSIC);
		game->getGSM()->getAudioManager()->stopSound(CUE_LEVEL_3_MUSIC);
		game->getGSM()->getAudioManager()->playSound(CUE_TITLE_MUSIC);
		game->quitGame();
	}
	// THE USER PRESSED THE Help BUTTON ON THE MAIN MENU,
	// SO LET'S DISPLAY THE CONTROLS
	else if (command == W_HELP_COMMAND) {
		GameStateManager *gsm = game->getGSM();
		gsm->goToHelpMenu();
	}
	// THE USER PRESSED THE About BUTTON ON THE IN-GAME MENU,
	// SO LET'S OPEN INFORMATION ABOUT THE GAME
	else if (command == W_ABOUT_COMMAND) {
		GameStateManager *gsm = game->getGSM();
		gsm->goToAboutMenu();
	}
}