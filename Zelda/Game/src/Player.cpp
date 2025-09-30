#include <windows.h>											// Header File For Windows
#include <stdio.h>												// Header File For Standard Input / Output
#include <stdarg.h>												// Header File For Variable Argument Routines
#include <math.h>												// Header File For Math Operations
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include "glut.h"
#include <assert.h>
#include "baseTypes.h"
#include "sound.h"
#include "openglframework.h"	
#include "gamedefs.h"
#include <cstddef>
#include "soil.h"
#include "Player.h"

Player* Player::pInstance = NULL;

Player::Player()	// constructor
{
	x = 0;
	y = 0;
	currentFrame = 0;
	totalFramesPerWalk = 2;
	totalFramesPerAttack = 4;
	currDirection = DOWN;
	mLinkTextureMap = 0;
	mLinkAttacksTextureMap = 0;
	mLinkWalkTextureMap = 0;
	prevDirection = NONE;
	swordBeamSFX = 0;
	swordSwingSFX = 0;
	swordHitLX = 0;
	swordHitLY = 0;
	swordHitRX = 0;
	swordHitUY = 0;
}

Player* Player::CreateInstance()
{
	if (pInstance != NULL)return pInstance;
	else
	{
		pInstance = new Player();
	}
	return pInstance;
}

void Player::init()
{	// initialize things like textures that we cant init in the constructor
	mLinkWalkTextureMap = SOIL_load_OGL_texture("asset/BasicMovementSheet.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	assert(mLinkWalkTextureMap != 0);
	if (mLinkWalkTextureMap == 0) {
		fprintf(stderr,"SOIL loading error: %s\n", SOIL_last_result());
		// Handle the error appropriately, e.g., return an error code or exit the program.
	}
	mLinkAttacksTextureMap = SOIL_load_OGL_texture("asset/LinkAttacksNoFlip.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (mLinkAttacksTextureMap == 0) {
		fprintf(stderr, "SOIL loading error: %s\n", SOIL_last_result());
		// Handle the error appropriately, e.g., return an error code or exit the program.
	}

	mLinkTextureMap = SOIL_load_OGL_texture("asset/LinkSpritesRev.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (mLinkTextureMap == 0) {
		fprintf(stderr, "SOIL loading error: %s\n", SOIL_last_result());
		// Handle the error appropriately, e.g., return an error code or exit the program.
	}

	swordSwingSFX = Sound::LoadSound("asset/LOZ_Sword_Slash.wav");
}

void Player::shutdown() {
	Sound::UnloadSound(swordSwingSFX);

	// should also be unloading textures here, but since there is not a clear, documented API for doing so... *shrug*
}

int Player::GetXPos()
{
	return x;
}

int Player::GetYPos()
{
	return y;
}

void Player::invXPos(int spawnOffset)
{
	x = -x + spawnOffset;	// invert the player's x pos and add an offset to avoid hitboxes to go back
}

void Player::invYPos(int spawnOffset)
{
	y = -y + spawnOffset;	// invert the player's y pos and add an offset to avoid hitboxes to go back
}

void Player::SetPlayerPos(int X, int Y)
{
	x = X;
	y = Y;
}

void Player::WallHandler(int X, int Y)
{
	switch (prevDirection)
	{
	case DOWN:
		y = Y + 6;
		canMoveDown = false;
		break;
	case UP:
		y = Y - 6;
		canMoveUp = false;
		break;
	case RIGHT:
		x = X - 6;
		canMoveRight = false;
		break;
	case LEFT:
		x = X + 6;
		canMoveLeft = false;
		break;
	default:
		break;
	}
}

void Player::EnableDirections()
{
	if (!canMoveDown)
	{
		canMoveDown = true;
	}
	if (!canMoveUp)
	{
		canMoveUp = true;
	}
	if (!canMoveRight)
	{
		canMoveRight = true;
	}
	if (!canMoveLeft)
	{
		canMoveLeft = true;
	}
}

void Player::SetHasSword(bool value)
{
	hasSword = value;
	// play pickup animation
	leftX = 128;
	rightX = 143;
	lowerY = 57;
	upperY = 42;
	isPickingUp = true;
	DisableMovementInput();
}

bool Player::GetHasSword()
{
	return hasSword;
}

void Player::Move(MoveDirection direction)	// move the player by the direction that will be passed
{
	if (direction != NONE)
	{
		prevDirection = currDirection;
		currDirection = direction;
	}

	switch (direction) {
		case RIGHT:
		{
			if (canMoveRight)
			{
				if ((x + quadWidth / 2) + moveSpeed <= xBounds) // only move right if its less than or equal to the xBounds
				{
					x += moveSpeed;
					isMoving = true;
				}
			}
			break;
		}
		case LEFT:
		{
			if (canMoveLeft)
			{
				if ((x - quadWidth / 2) - moveSpeed >= -xBounds) // only move right if its greater than or equal to the -xBounds
				{
					x -= moveSpeed;
					isMoving = true;
				}
			}
			break;
		}
		case UP:
		{
			if (canMoveUp)
			{
				if ((y + quadHeight / 2) + moveSpeed <= yBounds)
				{
					y += moveSpeed;
					isMoving = true;
				}
			}
			break;
		}
		case DOWN:
		{
			if (canMoveDown)
			{
				if ((y - quadHeight / 2) - moveSpeed >= -yBounds)
				{
					y -= moveSpeed;
					isMoving = true;
				}
			}
			break;
		}
		case NONE:
			isMoving = false;
			break;
		default:
			// given direction not supported
			assert(false);
	}
}

void Player::Attack()
{
	Sound::Playback(swordSwingSFX);

	//Disable movement input
	DisableMovementInput();
	// set the right params and values for the attack animation depending on where link is facing right now
	switch (prevDirection) // right 1, left 2, up 3, down 4
	{
	case RIGHT:	// when looking right link's sword goes 11 pixels right from his hand and the sword is 16 pixels tall
		swordHitRX = (x + quadWidth / 2) + attackRange;	// x pos + 16/2 (8) + attackRange will get me the furthest x point on the sword's HB
		swordHitLX = x + quadWidth / 2;	// x pos + 16/2 (8) will get me the hilt of the sword's HB
		swordHitUY = (y  + (quadHeight/2)) - 1;	// y pos + 16/2 (8) - 1 will get me the sword's highest Y value on it's HB, - 1 because the sprite has an odd 1 height lower
		swordHitLY = (y - (quadHeight/2)) - 1;
		isAttacking = true;
		break;

	case LEFT: // when looking right link's sword goes 11 pixels left from his hand and the sword is 16 pixels tall
		swordHitRX = x - quadWidth / 2;	// x pos - 16/2 (8) will get me the hilt of the sword's HB
		swordHitLX = (x - quadWidth / 2) - attackRange;	// x pos - 16/2 (8) - attackRange will get me the furthest/lowest x point on the sword's HB
		swordHitUY = (y + (quadHeight / 2)) - 1;	// y pos + 16/2 (8) - 1 will get me the sword's highest Y value on it's HB, - 1 because the sprite has an odd 1 height lower
		swordHitLY = (y - (quadHeight / 2)) - 1;
		isAttacking = true;
		break;

	case UP:	// when looking down link's sword goes 12 pixels up (there's an extra row of pixels for his head) from his head and the sword is 8 pixels wide
		swordHitRX = (x + quadWidth/2) - 5;	// xpos + 16/2(8) - 5 will get me the right most x val for this HB, -5 because the sword is 5 pixels left from link's rightmost point
		swordHitLX = (x - quadWidth/2) + 3;	// xpos - 16/2(8) + 3 will get me the left most x val for this HB, + 3 because the sword is 3 pixels right from link's leftmost point
		swordHitUY = (y + (quadHeight / 2)) + attackRange;	// ypos + 16/2(8) + attackRange will get me the furthest Y point on the sword's HB
		swordHitLY = (y + (quadHeight / 2));	// ypos + 16/2(8) will get me the hilt of the sword's HB
		isAttacking = true;
		break;

	case DOWN:	// when looking down link's sword goes 11 pixels down from his feet and the sword is 8 pixels wide
		swordHitRX = (x + quadWidth / 2) - 3;	// xpos + 16/2(8) - 3 will get me the right most x val for this HB,- 3 because the sword is 3 pixels left from link's rightmost point 
		swordHitLX = (x - quadWidth / 2) + 5;	// xpos - 16/2(8) + 5 will get me the left most x val for this HB, +5 because the sword is 5 pixels right from link's leftmost point
		swordHitUY = (y - (quadHeight / 2));	// ypos - 16/2(8) will get me the hilt of the sword's HB
		swordHitLY = (y - (quadHeight / 2)) - attackRange;	// ypos + 16/2(8) - attackRange will get me the furthest/lowest Y point on the sword's HB
		isAttacking = true;
		break;

	default:
		break;
	}

	//if (isAttacking == true)
	//{
	//	mChosenTexMap = mLinkAttacksTextureMap;
	//}
}

void Player::DisableMovementInput()
{
	canMove = false;
	isMoving = false;
}

void Player::EnableMovementInput()
{
	canMove = true;
}

void Player::updateSprite(DWORD milliseconds)
{
	mCurrentTime += milliseconds;

	if (isPickingUp)
	{
		if (mCurrentTime - mLastUpdateTime > 1000)	// link should hold the pickup animation for 1 second
		{
			isPickingUp = false;

			leftX = 0;	// set him back to the looking down pose
			rightX = 15;
			upperY = 42;
			lowerY = 57;
			EnableMovementInput();	// bring back the controls
		}
	}

	if (isAttacking)
	{
		if (prevDirection == DOWN)
		{
			leftX = 16;
			rightX = 31;
			upperY = 0;
			lowerY = 38;
			quadHeight = 38;	// link's quad gets larger in height
			quadWidth = 16;
		}
		else if (prevDirection == UP)
		{
			leftX = 80;
			rightX = 95;
			upperY = 0;
			lowerY = 37;
			quadHeight = 38;
			quadWidth = 16;
		}
		else if (prevDirection == RIGHT)
		{
			leftX = 144;
			rightX = 171;
			upperY = 11;
			lowerY = 25;
			quadHeight = 16;
			quadWidth = 28;
		}
		else if (prevDirection == LEFT)
		{
			leftX = 171;	// swap the x's to invert it on the x axis
			rightX = 144;
			upperY = 11;
			lowerY = 25;
			quadHeight = 16;
			quadWidth = 28;
		}

		if (mCurrentTime - mLastUpdateTime > 500)	// link can only move half a second after he begins his attack animation
		{
			mLastUpdateTime = mCurrentTime;
			EnableMovementInput();
			swordHitLX = x - quadWidth / 2;	// reset the sword's HB to be the player's sprite dimensions
			swordHitRX = x + quadWidth / 2;
			swordHitUY = y + quadHeight / 2;
			swordHitLY = y - quadHeight / 2;
			isAttacking = false;

			if (prevDirection == DOWN)
			{
				leftX = 0;
				rightX = 15;
				upperY = 42;
				lowerY = 57;
				quadHeight = 16;
				quadWidth = 16;	// set link's quad back to his walking cycle's dimensions
			}
			if (prevDirection == UP)
			{
				leftX = 64;
				rightX = 79;
				upperY = 42;
				lowerY = 57;
				quadHeight = 16;
				quadWidth = 16;	// set link's quad back to his walking cycle's dimensions
			}
			if (prevDirection == RIGHT)
			{
				leftX = 32;
				rightX = 47;
				upperY = 42;
				lowerY = 57;
				quadHeight = 16;
				quadWidth = 16;	// set link's quad back to his walking cycle's dimensions
			}
			if (prevDirection == LEFT)
			{
				leftX = 96;
				rightX = 111;
				upperY = 42;
				lowerY = 57;
				quadHeight = 16;
				quadWidth = 16;	// set link's quad back to his walking cycle's dimensions
			}
		}
	}

	if (isMoving)
	{
		if (mCurrentTime - mLastUpdateTime > 100)
		{
			mLastUpdateTime = mCurrentTime;

			// Increment current frame and reset if it exceeds total frames
			currentFrame = (currentFrame + 1) % totalFramesPerWalk;

			// Toggle between the two frames of walk cycle based on direction
			if (currDirection == DOWN)
			{
				if (currentFrame % 2 == 0)
				{
					leftX = 0;
					rightX = 15;
					upperY = 42;
					lowerY = 57;
					quadHeight = 16;
					quadWidth = 16;
				}
				else
				{
					leftX = 16;
					rightX = leftX + quadWidth;
					upperY = 42;
					lowerY = 57;
					quadHeight = 16;
					quadWidth = 16;
				}
			}
			else if (currDirection == UP)
			{
				if (currentFrame % 2 == 0)
				{
					leftX = 64;
					rightX = leftX + quadWidth;
					upperY = 42;
					lowerY = 57;
					quadHeight = 16;
					quadWidth = 16;
				}
				else
				{
					leftX = 80;
					rightX = leftX + quadWidth;
					upperY = 42;
					lowerY = 57;
					quadHeight = 16;
					quadWidth = 16;
				}
			}
			else if (currDirection == LEFT)
			{
				if (currentFrame % 2 == 0)
				{
					leftX = 96;
					rightX = leftX + quadWidth;
					upperY = 42;
					lowerY = 57;
					quadHeight = 16;
					quadWidth = 16;
				}
				else
				{
					leftX = 112;
					rightX = leftX + quadWidth;
					upperY = 42;
					lowerY = 57;
					quadHeight = 16;
					quadWidth = 16;
				}
			}
			else if (currDirection == RIGHT)
			{
				if (currentFrame % 2 == 0)
				{
					leftX = 32;
					rightX = leftX + quadWidth;
					upperY = 42;
					lowerY = 57;
					quadHeight = 16;
					quadWidth = 16;
				}
				else
				{
					leftX = 48;
					rightX = leftX + quadWidth;
					upperY = 42;
					lowerY = 57;
					quadHeight = 16;
					quadWidth = 16;
				}
			}
		}
	}


}

void Player::RenderPlayer()
{
	float xLeft = x - (quadWidth / 2) * 1.56; //-960
	float xRight = x + (quadWidth / 2) * 1.56; //960
	float yBot = y + (quadHeight / 2) * 1.56; //540 * 1.56
	float yTop = y - (quadHeight / 2) * 1.56; // -540 * 1.56

	float leftU = leftX / (float)width;
	float rightU = rightX / (float)width;
	float upperV = upperY / (float)height;
	float lowerV = lowerY / (float)height;

	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, mChosenTexMap);
	glBindTexture(GL_TEXTURE_2D, mLinkTextureMap);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// stops pixels from getting blurry
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glDisable(GL_CULL_FACE);

	glBegin(GL_QUADS);

	glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

	// top left of texture (0,1)
	glTexCoord2f(leftU, 1 - lowerV);
	glVertex3f(xLeft, yTop, 0.0);

	// top right of texture (1,1)
	glTexCoord2f(rightU, 1 - lowerV);
	glVertex3f(xRight, yTop, 0.0);

	// bottom right of texture (1,0)
	glTexCoord2f(rightU, 1 - upperV);
	glVertex3f(xRight, yBot, 0.0);

	// bottom left of texture (0,0)
	glTexCoord2f(leftU, 1 - upperV);
	glVertex3f(xLeft, yBot, 0.0);	// printing to the output screen

	glEnd();
}