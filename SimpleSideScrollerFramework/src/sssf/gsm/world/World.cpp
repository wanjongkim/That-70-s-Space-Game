/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	World.cpp

	See World.h for a class description.
*/

#include "sssf_VS\stdafx.h"
#include "sssf\data_loader\GameDataLoader.h"
#include "sssf\game\Game.h"
#include "sssf\graphics\GameGraphics.h"
#include "sssf\graphics\RenderList.h"
#include "sssf\graphics\TextureManager.h"
#include "sssf\gsm\sprite\SpriteManager.h"
#include "sssf\gsm\state\GameStateManager.h"
#include "sssf\gsm\world\World.h"
#include "sssf\gui\Viewport.h"

/*
	World - Default Constructor, it constructs the layers
	vector, allowing new layers to be added.
*/
World::World()	
{
	layers = new vector<WorldLayer*>();
	worldWidth = 0;
	levelNum = 1;
	move1 = false;
	move2 = false;
	move3 = false;
	move4 = false;
	move5 = false;
	move6 = false;
	move7 = false;
	move8 = false;
	move9 = false;
	move10 = false;
	move11 = false;
	stop = false;
	worldHeight = 0;
}

/*
	~World - This destructor will remove the memory allocated
	for the layer vector.
*/
World::~World()	
{
	delete layers;
}

/*
	addLayer - This method is how layers are added to the World.
	These layers might be TiledLayers, SparseLayers, or 
	IsometricLayers, all of which are child classes of WorldLayer.
*/
void World::addLayer(WorldLayer *layerToAdd)
{
	layers->push_back(layerToAdd);
}

/*
	addWorldRenderItemsToRenderList - This method sends the render
	list and viewport to each of the layers such that they
	may fill it with RenderItems to draw.
*/
void World::addWorldRenderItemsToRenderList(Game *game)
{
	GameStateManager *gsm = game->getGSM();
	GameGUI *gui = game->getGUI();
	if (gsm->isWorldRenderable())
	{
		GameGraphics *graphics = game->getGraphics();
		RenderList *renderList = graphics->getWorldRenderList();
		Viewport *viewport = gui->getViewport();
		for (unsigned int i = 0; i < layers->size(); i++)
		{
			layers->at(i)->addRenderItemsToRenderList(	renderList,
														viewport);
		}
	}
}

/*
	clear - This method removes all data from the World. It should
	be called first when a level is unloaded or changed. If it
	is not called, an application runs the risk of having lots
	of extra data sitting around that may slow the progam down.
	Or, if the world thinks a level is still active, it might add
	items to the render list using image ids that have already been
	cleared from the GameGraphics' texture manager for the world.
	That would likely result in an exception.
*/
void World::unloadWorld()
{
	layers->clear();
	worldWidth = 0;
	worldHeight = 0;
}

/*
	update - This method should be called once per frame. Note that
	the World is for static objects, so we don't have anything
	to update as/is. But, should the need arise, one could add non-
	collidable layers to a game world and update them here. For
	example, particle systems.
*/
void World::update(Game *game)
{
	Viewport *viewport = game->getGUI()->getViewport();
	if (this->levelNum == 1){
		if (stop){

		}
		else if (!move1){
			viewport->moveViewport((int)floor(0 + 0.5f), (int)floor(3 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(3);
			viewport->setVx(0);
			if (viewport->getViewportY() >= 732){
				move1 = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
		else if (move11){
			viewport->moveViewport((int)floor(-3 + 0.5f), (int)floor(0 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(0);
			viewport->setVx(-3);
			if (viewport->getViewportX() <= 1186){
				stop = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
		else if (move10){
			viewport->moveViewport((int)floor(0 + 0.5f), (int)floor(-3 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(-3);
			viewport->setVx(0);
			if (viewport->getViewportY() <= 1762){
				move11 = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
		else if (move9){
			viewport->moveViewport((int)floor(3 + 0.5f), (int)floor(0 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(0);
			viewport->setVx(3);
			if (viewport->getViewportX() >= 8170){
				move10 = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
		else if (move8){
			viewport->moveViewport((int)floor(0 + 0.5f), (int)floor(3 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(3);
			viewport->setVx(0);
			if (viewport->getViewportY() >= 2450){
				move9 = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
		else if (move7){
			viewport->moveViewport((int)floor(-3 + 0.5f), (int)floor(0 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(0);
			viewport->setVx(-3);
			if (viewport->getViewportX() <= 0){
				move8 = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
		else if (move6){
			viewport->moveViewport((int)floor(0 + 0.5f), (int)floor(-3 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(-3);
			viewport->setVx(0);
			if (viewport->getViewportY() <= 30){
				move7 = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
		else if (move5){
			viewport->moveViewport((int)floor(-3 + 0.5f), (int)floor(0 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(0);
			viewport->setVx(-3);
			if (viewport->getViewportX() <= 1288){
				move6 = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
		else if (move4){
			viewport->moveViewport((int)floor(0 + 0.5f), (int)floor(3 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(3);
			viewport->setVx(0);
			if (viewport->getViewportY() >= 732){
				move5 = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
		else if (move3){
			viewport->moveViewport((int)floor(-3 + 0.5f), (int)floor(0 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(0);
			viewport->setVx(-3);
			if (viewport->getViewportX() <= 4864){
				move4 = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
		else if (move2){
			viewport->moveViewport((int)floor(0 + 0.5f), (int)floor(-3 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(-3);
			viewport->setVx(0);
			if (viewport->getViewportY() <= 30){
				move3 = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
		else if (move1){
			viewport->moveViewport((int)floor(3 + 0.5f), (int)floor(0 + 0.5f), game->getGSM()->getWorld()->getWorldWidth(), game->getGSM()->getWorld()->getWorldHeight());
			viewport->setVy(0);
			viewport->setVx(3);
			if (viewport->getViewportX() >= 8170){
				move2 = true;
				viewport->setVx(0);
				viewport->setVy(0);
			}
		}
	}
}