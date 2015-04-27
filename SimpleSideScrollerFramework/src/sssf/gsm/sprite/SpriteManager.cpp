/*	
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	SpriteManager.cpp

	See SpriteManager.h for a class description.
*/

#pragma once
#include "sssf_VS\stdafx.h"
#include "sssf\gsm\ai\Bot.h"
#include "sssf\gsm\physics\PhysicalProperties.h"
#include "sssf\graphics\GameGraphics.h"
#include "sssf\gsm\sprite\AnimatedSprite.h"
#include "sssf\gsm\sprite\AnimatedSpriteType.h"
#include "sssf\gsm\sprite\SpriteManager.h"
#include "sssf\gsm\state\GameStateManager.h"

/*
	addSpriteToRenderList - This method checks to see if the sprite
	parameter is inside the viewport. If it is, a RenderItem is generated
	for that sprite and it is added to the render list.
*/
void SpriteManager::addSpriteToRenderList(AnimatedSprite *sprite,
										  RenderList *renderList,
										  Viewport *viewport)
{
	// GET THE SPRITE TYPE INFO FOR THIS SPRITE
	AnimatedSpriteType *spriteType = sprite->getSpriteType();
	PhysicalProperties *pp = sprite->getPhysicalProperties();

	// IS THE SPRITE VIEWABLE?
	if (viewport->areWorldCoordinatesInViewport(	
									pp->getX(),
									pp->getY(),
									spriteType->getTextureWidth(),
									spriteType->getTextureHeight()))
	{
		// SINCE IT'S VIEWABLE, ADD IT TO THE RENDER LIST
		RenderItem itemToAdd;
		itemToAdd.id = sprite->getFrameIndex();
		renderList->addRenderItem(	sprite->getCurrentImageID(),
									pp->round(pp->getX()-viewport->getViewportX()),
									pp->round(pp->getY()-viewport->getViewportY()),
									pp->round(pp->getZ()),
									sprite->getAlpha(),
									spriteType->getTextureWidth(),
									spriteType->getTextureHeight());
	}
}

/*
	addSpriteItemsToRenderList - This method goes through all of the sprites,
	including the player sprite, and adds the visible ones to the render list.
	This method should be called each frame.
*/
void SpriteManager::addSpriteItemsToRenderList(	Game *game)
{
	GameStateManager *gsm = game->getGSM();
	GameGUI *gui = game->getGUI();
	if (gsm->isWorldRenderable())
	{
		GameGraphics *graphics = game->getGraphics();
		RenderList *renderList = graphics->getWorldRenderList();
		Viewport *viewport = gui->getViewport();

		// ADD THE PLAYER SPRITE
		addSpriteToRenderList(&player, renderList, viewport);

		// NOW ADD THE REST OF THE SPRITES
		list<Bot*>::iterator botIterator;
		botIterator = bots.begin();
		while (botIterator != bots.end())
		{			
			Bot *bot = (*botIterator);
			addSpriteToRenderList(bot, renderList, viewport);
			botIterator++;
		}
	}
}

/*
	addSprite - This method is for adding a new sprite to 
	this sprite manager. Once a sprite is added it can be 
	scheduled for rendering.
*/
void SpriteManager::addBot(Bot *botToAdd)
{
	bots.push_back(botToAdd);
}

/*
	addSpriteType - This method is for adding a new sprite
	type. Note that one sprite type can have many sprites. For
	example, we may say that there may be a "Bunny" type of
	sprite, and specify properties for that type. Then there might
	be 100 different Bunnies each with their own properties, but that
	share many things in common according to what is defined in
	the shared sprite type object.
*/
unsigned int SpriteManager::addSpriteType(AnimatedSpriteType *spriteTypeToAdd)
{
	spriteTypes.push_back(spriteTypeToAdd);
	return spriteTypes.size()-1;
}

/*
	clearSprites - This empties all of the sprites and sprite types.
*/
void SpriteManager::clearSprites()
{
	spriteTypes.clear();
	bots.clear();
}

/*
	getSpriteType - This gets the sprite type object that corresponds
	to the index argument.
*/
AnimatedSpriteType* SpriteManager::getSpriteType(unsigned int typeIndex)
{
	if (typeIndex < spriteTypes.size())
		return spriteTypes.at(typeIndex);
	else
		return NULL;
}

/*
	unloadSprites - This method removes all artwork from memory that
	has been allocated for game sprites.
*/
void SpriteManager::unloadSprites()
{
	// @TODO - WE'LL DO THIS LATER WHEN WE LEARN MORE ABOUT MEMORY MANAGEMENT
}

Bot* SpriteManager::removeBot(Bot *botToRemove)
{
	return NULL;
	// @TODO - WE'LL DO THIS LATER WHEN WE LEARN MORE ABOUT MEMORY MANAGEMENT
}

/*
	update - This method should be called once per frame. It
	goes through all of the sprites, including the player, and calls their
	update method such that they may update themselves.
*/
void SpriteManager::update(Game *game)
{
	static const wstring DEAD = (L"DEAD");
	static const wstring IDLE = (L"IDLE");

	// UPDATE PHYSICAL PROPERTIES OF EACH SPRITE FROM BOX2D
	// THIS IS SO THE RENDERING STEP WON'T NEED TO BE CHANGED, ALL CONVERSION IS DONE HERE
	b2Body* b = game->getGSM()->getb2World()->GetBodyList();//get start of list
	while (b != NULL) {


		AnimatedSprite* body = static_cast<AnimatedSprite*>(b->GetUserData());
		
		if (body != NULL) {
			if (body->getPhysicalProperties()->getSpriteType() != 0){
				body->getPhysicalProperties()->setPosition((b->GetPosition().x * 64) - (body->getSpriteType()->getTextureWidth() / 2), ((50 - b->GetPosition().y) * 64) - (body->getSpriteType()->getTextureHeight() / 2));
				body->getPhysicalProperties()->setVelocity(b->GetLinearVelocity().x, b->GetLinearVelocity().y);
			}
			else{
				body->getPhysicalProperties()->setPosition((b->GetPosition().x * 64) - 54, ((50 - b->GetPosition().y) * 64) - 62);
				body->getPhysicalProperties()->setVelocity(b->GetLinearVelocity().x, b->GetLinearVelocity().y);
			}
		}

		//continue to next body
		b = b->GetNext();
	}

	// UPDATE THE PLAYER SPRITE
	player.updateSprite();
	if (player.getCurrentState() == DEAD){
		if (player.getPhysicalProperties()->getDeathCount() == 2){
			player.getPhysicalProperties()->setVelocity(0, 0);
			player.getBody()->SetLinearVelocity(b2Vec2(0, 0));
		}
		if (player.getPhysicalProperties()->getDeathCount() == 100){
			player.setCurrentlyCollidable(false);
			//player.getPhysicalProperties()->setPosition(100, 100);
			player.setCurrentState(IDLE);
			player.getPhysicalProperties()->setDeathCount(0);
		}
		else{
			player.getPhysicalProperties()->setDeathCount(player.getPhysicalProperties()->getDeathCount() + 1);
		}
	}

	// NOW UPDATE THE REST OF THE SPRITES
	list<Bot*>::iterator botIterator;
	botIterator = bots.begin();
	while (botIterator != bots.end())
	{
		Bot *bot = (*botIterator);
		if (bot->getFrameCount() >= 100){
			bot->setCurrentState(DEAD);
		}
		if (bot->getCurrentState() == DEAD){
			bot->getPhysicalProperties()->setVelocity(0, 0);
			bot->getBody()->SetLinearVelocity(b2Vec2(0, 0));
			if (bot->getPhysicalProperties()->getSpriteType() == 4 || bot->getPhysicalProperties()->getSpriteType() == 8){
				bot->getPhysicalProperties()->setCollidable(false);
				game->getGSM()->getb2World()->DestroyBody(bot->getBody());
				botIterator = bots.erase(botIterator);
				//delete(bot);
			}
			else if (bot->getPhysicalProperties()->getDeathCount() == 40){
				bot->getPhysicalProperties()->setCollidable(false);
				game->getGSM()->getb2World()->DestroyBody(bot->getBody());
				botIterator = bots.erase(botIterator);
				//delete(bot);
			}
			else{
				bot->getPhysicalProperties()->setDeathCount(bot->getPhysicalProperties()->getDeathCount() + 1);
				bot->updateSprite();
				botIterator++;
			}
		}
		else if (bot->getPhysicalProperties()->getX() <= 0 || bot->getPhysicalProperties()->getX() >= 9600 || bot->getPhysicalProperties()->getY() <= 0 || bot->getPhysicalProperties()->getY() >= 3200){
			botIterator = bots.erase(botIterator);
		}
		else{
			bot->think(game);
			bot->updateSprite();
			botIterator++;
		}
	}
}