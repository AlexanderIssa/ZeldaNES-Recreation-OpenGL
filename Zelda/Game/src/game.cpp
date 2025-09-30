#define GAME_CPP
#include <windows.h>											// Header File For Windows
#include <stdio.h>												// Header File For Standard Input / Output
#include <stdarg.h>												// Header File For Variable Argument Routines
#include <math.h>												// Header File For Math Operations
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
//#include <iostream> 
//#include <string> 
#include "glut.h"
#include "baseTypes.h"
#include "openglframework.h"	
#include "gamedefs.h"
#include "collInfo.h"
#include "object.h"
#include "inputmapper.h"
#include "ball.h"
#include "field.h"
#include "random.h"
#include "gameObjects.h"
#include "openGLStuff.h"
#include "game.h"
#include "StateManager.h"
#include "BallManager.h"
#include "FieldManager.h"
#include "InputManager.h"
#include "sound.h"
#include "SpriteDemoManager.h"
#include "igame.h"
#include "Player.h"
#include "SubRoom.h"
#include "Room.h"
#include "PauseMenu.h"

// Defines for Controls

// -- Map Controls --
#define INPUT_MAPLEFT 'A'
#define INPUT_MAPRIGHT 'D'
#define INPUT_MAPDOWN 'S'
#define INPUT_MAPUP 'W'

// -- Player Controls --
#define INPUT_LEFT VK_LEFT
#define INPUT_RIGHT VK_RIGHT
#define INPUT_DOWN VK_DOWN
#define INPUT_UP VK_UP
#define INPUT_ATTACK 'X'
#define INPUT_SPECIAL 'Z'

// -- Menu Controls --
#define INPUT_START VK_RETURN

IGame* CreateGame() {
	return new CGame();
}

void DestroyGame(IGame* pGame) {
	delete pGame;
}

// Declarations
const char8_t CGame::mGameTitle[]= "The Legend Of Zelda";


CGame::CGame() {
	currState = Title;
	FieldManagerC::CreateInstance();
	SpriteDemoManagerC::CreateInstance();
	Player::CreateInstance();
	Room::CreateInstance();
	PauseMenu::CreateInstance();
}

CGame::~CGame()
{
	// prefer delete in reverse of create (also true for init/shutdown)
	//
	// also note: for singleton pattern, PauseMenu::DeleteInstance() would be better for intuitive pairing w/ Create
	PauseMenu::DeleteInstance();
	Room::DeleteInstance();
	Player::DeleteInstance();
	delete SpriteDemoManagerC::GetInstance();
	delete FieldManagerC::GetInstance();
}

bool CGame::Initialize (GL_Window* window, Keys* keys)
{
	initOpenGLDrawing(window,keys,0.0f, 0.0f, 0.0f);

	FieldManagerC::GetInstance()->init();
	SpriteDemoManagerC::GetInstance()->init(28, 26);
	Room::GetInstance()->init(); // initialize the rooms array(s)
	Player::GetInstance()->init();
	PauseMenu::GetInstance()->init();
	OverworldBGM = Sound::LoadSound("asset/OverworldWAV.wav");
	TitleBGM = Sound::LoadSound("asset/IntroWAV.wav");
	Sound::Playback(TitleBGM);
	return true;
}

void CGame::Deinitialize() 
{
	// always pair init/shutdown, by convention
	//PauseMenu::GetInstance()->shutdown
	Player::GetInstance()->shutdown();
	Room::GetInstance()->shutdown();
	FieldManagerC::GetInstance()->shutdown();
}

void CGame::UpdateFrame(uint32_t milliseconds)			
{
	// this logic should be broken out into several helper functions for readability, maintainability, clarity of intent

	if (currState == Title)
	{
		if (g_keys->keyDown[VK_RETURN]) // enter key
		{
			currState = Overworld;
			Sound::Stop(TitleBGM);
			Sound::Playback(OverworldBGM);
			SpriteDemoManagerC::GetInstance()->ChangeRender();

			// these changes to the projection matrix should not have been necessary for a 2d game
			
			// lets lower the projection matrix dimensions since the title screen is bigger than the overworld
			glMatrixMode(GL_PROJECTION);	// Select The Projection Matrix
			glLoadIdentity();	// Reset The Projection Matrix

			glOrtho(-257, 257, -176, 232, -1.0, 1.0);	// a subroom's size is 257x176 but the UI on top is an extra 56 pixels tall so we account for that
		
			glMatrixMode(GL_MODELVIEW);	// Select The Modelview Matrix
			glLoadIdentity();	// Reset The Modelview Matrix
			
			g_keys->keyDown[0x5A] = false;	// reset the key to false so it doesnt spam the button on tic
		}
		if (g_keys->keyDown[VK_ESCAPE])								// Is ESC Being Pressed?
		{
			TerminateApplication(g_window);						// Terminate The Program
		}
	}
	else if (currState == Overworld)
	{
		pX = Player::GetInstance()->GetXPos();	// update the player pos
		pY = Player::GetInstance()->GetYPos();

		if (g_keys->keyDown[VK_ESCAPE])								// Is ESC Being Pressed?
		{
			TerminateApplication(g_window);						// Terminate The Program
		}

		if (g_keys->keyDown[VK_F1])									// Is F1 Being Pressed?
		{
			ToggleFullscreen(g_window);							// Toggle Fullscreen Mode
		}

		if (Player::GetInstance()->GetCanMove() == true)	// if the player can move in the first place
		{
			// in the actual game UP and DOWN take priority over RIGHT and LEFT
			if (g_keys->keyDown[INPUT_UP])
			{
				Player::GetInstance()->Move(UP);	// Up = 3
			}
			else if (g_keys->keyDown[INPUT_DOWN])
			{
				Player::GetInstance()->Move(DOWN);	// Down = 4
			}
			else if (g_keys->keyDown[INPUT_RIGHT])
			{
				Player::GetInstance()->Move(RIGHT);	// right = 1
			}
			else if (g_keys->keyDown[INPUT_LEFT])
			{
				Player::GetInstance()->Move(LEFT);	// left = 2
			}
			else
			{
				Player::GetInstance()->Move(NONE);	// None = 0
			}
		}

		if (g_keys->keyDown[INPUT_ATTACK] && !pressingX) // X key
		{
			if (Player::GetInstance()->GetHasSword() == true)	// only able to attack once the player has picked up the sword
			{
				if (Player::GetInstance()->GetIsAttackng() == false)
				{
					Player::GetInstance()->Attack();
				}
			}
			pressingX = true; // reset the key to false so it doesnt spam the button on tick
		}
		else if (!g_keys->keyDown[INPUT_ATTACK] && pressingX)	// to make sure player can't hold X
		{
			pressingX = false;
		}

		if (g_keys->keyDown[INPUT_SPECIAL]) // Z key
		{
			//Player::GetInstance()->Special();

			// modifying the global keys is an atypical pattern for this... you don't own that keys array, it is part of the
			//  framework. better would be to implement your own latching, like you did for the X button - and typically that
			//  would happen in some kind of helper class in a case like this where they are so many inputs. i'm also not sure
			//  that this is actually doing anything, since the framework appears to set keyDown each frame

			g_keys->keyDown[INPUT_SPECIAL] = false;	// reset the key to false so it doesnt spam the button on tic
		}

		if (g_keys->keyDown[INPUT_START]) // enter key
		{
			//Player::GetInstance()->PauseGame();
			g_keys->keyDown[INPUT_ATTACK] = false;	// reset the key to false so it doesnt spam the button on tic
		}

		if (g_keys->keyDown[INPUT_MAPUP]) // W key
		{
			if (Room::GetInstance()->MoveNorth())
			{
				SpriteDemoManagerC::GetInstance()->MoveUVUp();
			}
			g_keys->keyDown[INPUT_MAPUP] = false;	// reset the key to false so it doesnt spam the button on tick
		}

		if (g_keys->keyDown[INPUT_MAPDOWN]) // S key
		{
			if (Room::GetInstance()->MoveSouth())
			{
				SpriteDemoManagerC::GetInstance()->MoveUVDown();
			}
			g_keys->keyDown[INPUT_MAPDOWN] = false;	// reset the key to false so it doesnt spam the button on tick
		}

		if (g_keys->keyDown[INPUT_MAPLEFT]) // A key
		{
			if (Room::GetInstance()->MoveWest())
			{
				SpriteDemoManagerC::GetInstance()->MoveUVLeft();
			}
			g_keys->keyDown[INPUT_MAPLEFT] = false;	// reset the key to false so it doesnt spam the button on tick
		}

		if (g_keys->keyDown[INPUT_MAPRIGHT]) // D Key
		{
			if (Room::GetInstance()->MoveEast())
			{
				SpriteDemoManagerC::GetInstance()->MoveUVRight();
			}
			g_keys->keyDown[INPUT_MAPRIGHT] = false;
		}

		if (Room::GetInstance()->GetCurrSR().CheckCollisions(pX, pY) == 1)
		{
			// Don't let the player go past the wall
			// idea: keep the players position the same as they were before colliding - 1 based on direction facing
			Player::GetInstance()->WallHandler(pX, pY);
		}
		else
		{
			Player::GetInstance()->EnableDirections();
		}


		switch (Room::GetInstance()->GetCurrSR().CheckExits(pX, pY)) //switch (Room::GetInstance()->GetCurrSR()->CheckExits(pX, pY))
		{
		case 'n':
			Room::GetInstance()->MoveNorth();
			Player::GetInstance()->invYPos(spawnOffset);
			SpriteDemoManagerC::GetInstance()->MoveUVUp();
			srNumExits = Room::GetInstance()->GetCurrSR().GetNumExits(); // srNumExits = Room::GetInstance()->GetCurrSR().GetNumExits();
			Room::GetInstance()->GetSubRooms();
			// put some kind of wait pause to avoid constant collision triggers
			break;

		case 's':
			Room::GetInstance()->MoveSouth();
			Player::GetInstance()->invYPos(-spawnOffset);
			SpriteDemoManagerC::GetInstance()->MoveUVDown();
			srNumExits = Room::GetInstance()->GetCurrSR().GetNumExits(); // srNumExits = Room::GetInstance()->GetCurrSR().GetNumExits();
			break;

		case 'e':
			Room::GetInstance()->MoveEast();
			Player::GetInstance()->invXPos(spawnOffset);
			SpriteDemoManagerC::GetInstance()->MoveUVRight();
			srNumExits = Room::GetInstance()->GetCurrSR().GetNumExits();
			break;

		case 'w':
			Room::GetInstance()->MoveWest();
			Player::GetInstance()->invXPos(-spawnOffset);
			SpriteDemoManagerC::GetInstance()->MoveUVLeft();
			srNumExits = Room::GetInstance()->GetCurrSR().GetNumExits(); // srNumExits = Room::GetInstance()->GetCurrSR().GetNumExits();
			break;

		case 'c':
			Room::GetInstance()->MoveToCave();	// moveToCave()
			Player::GetInstance()->SetPlayerPos(0, -146);	// set player y = -height + offset (to avoid exit hitbox) and x = 0
			SpriteDemoManagerC::GetInstance()->MoveUVToCave(); // moveuvToCave()
			srNumExits = Room::GetInstance()->GetCurrSR().GetNumExits();
			break;

		case 'f':	// special case to go back to the first room [8]
			Room::GetInstance()->MoveToRoom(8);	// moveToCave()
			Player::GetInstance()->SetPlayerPos(-100, 100);
			SpriteDemoManagerC::GetInstance()->MoveUVToFirstRoom(); // moveuvToCave()
			srNumExits = Room::GetInstance()->GetCurrSR().GetNumExits();
			break;

		default:	// default should be 'o'
			break;
		}

		switch (Room::GetInstance()->GetCurrSR().CheckPickupsCol(pX, pY))
		{
		case 1:
			Player::GetInstance()->SetHasSword(true);	// if pickups col function returns 1 its the sword
			break;
		default:
			break;
		}

		Player::GetInstance()->updateSprite(milliseconds);
	}
	
}

void CGame::DrawScene(void)											
{
	if (currState == Title)
	{
		startOpenGLDrawing();
		SpriteDemoManagerC::GetInstance()->RenderTitle();
	}
	else if (currState == Overworld)
	{
		startOpenGLDrawing();
		SpriteDemoManagerC::GetInstance()->RenderSprites();
		Room::GetInstance()->GetCurrSR().RenderPickups();
		Player::GetInstance()->RenderPlayer();
		PauseMenu::GetInstance()->RenderMenu();
		PauseMenu::GetInstance()->RenderItems(Player::GetInstance()->GetHasSword());
	}

}
