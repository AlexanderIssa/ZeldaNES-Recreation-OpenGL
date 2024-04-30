#include <cstddef>
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
#include "soil.h"
#include "SubRoom.h"

SubRoom::SubRoom()
{
	id = 0;
	north = 0;
	west = 0;
	south = 0;
	east = 0;
	cave = 0;

	// to insure all instances of SubRoom have their own unique Exits array we initialize it to some dummy value then repopulate it later
	for (int i = 0; i < maxExits; ++i) {
		Exits[i].outRoom = -1;
	}

	for (int i = 0; i < maxExits; ++i) {
		Pickups[i].itemType = None;
	}
}

SubRoom::SubRoom(int ID, int N, int W, int S, int E)
{
	mPickupTextureMap = SOIL_load_OGL_texture("asset/ItemsNpc.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (mPickupTextureMap == 0) {
		printf("SOIL loading error: %s\n", SOIL_last_result());
		// Handle the error appropriately, e.g., return an error code or exit the program.
	}

	PickupSFX = Sound::LoadSound("asset/ItemWAV.wav");

	id = ID;
	north = N;
	west = W;
	south = S;
	east = E;
	cave = 0;
	numExits = 0;

	// to insure all instances of SubRoom have their own unique Exits array we initialize it to some dummy value then repopulate it later
	for (int i = 0; i < maxExits; ++i) {
		Exits[i].outRoom = -1;
	}

	for (int i = 0; i < maxExits; ++i) {
		Pickups[i].itemType = None;
	}

	for (int i = 0; i < maxWalls; ++i) {
		Walls[i].leftx = -258;
		Walls[i].rightx = -257;
		Walls[i].uppery = 0;
		Walls[i].lowery = 0;
	}

	FillRoomObjects();
}

SubRoom::sExit* SubRoom::GetExitArr()
{
	return Exits;
}

int SubRoom::GetNumExits()
{
	return numExits;
}

SubRoom::sExit SubRoom::CreateExit(int OutRoom,char Direction,int XPos,int YPos, int XSize, int YSize, int Ori)
{
	sExit temp;
	temp.outRoom = OutRoom;
	temp.direction = Direction;
	temp.xPos = XPos;
	temp.yPos = YPos;
	temp.xSize = XSize;
	temp.ySize = YSize;
	temp.ori = Ori;

	return temp;
}

SubRoom::sPickup SubRoom::CreatePickup(eItemTypes ItemType, int XPos, int YPos, int QHeight, int QWidth, int leftX, int rightX, int upperY, int lowerY)
{
	sPickup temp;
	temp.itemType = ItemType;
	temp.xPos = XPos;
	temp.yPos = YPos;
	temp.qHeight = QHeight;
	temp.qWidth = QWidth;
	return temp;
}

SubRoom::sWall SubRoom::CreateWall(int QW, int QH, int leftX, int rightX, int upperY, int lowerY)
{
	sWall temp;
	temp.qWidth = QW;
	temp.qHeight = QH;
	temp.leftx = leftX;
	temp.rightx = rightX;
	temp.uppery = upperY;
	temp.lowery = lowerY;
	return temp;
}

char SubRoom::CheckExits(int p_Xpos, int p_Ypos)	// checks each exit in this current subroom to see if the player is colliding with any
{													// if they are then return the direction they move in the map
	char temp = 'o';

	for (int i = 0; i < numExits; ++i)
	{
		// orientation numbers: 1 = horizontal up, 2 = horizontal down, 3 = vertical right, 4 = vertical left
		switch (Exits[i].ori)
		{
		case 1: 	// the only thing to check the range of for a horizontal up exit is the range of xPos and xPos + width, the yPos never changes
			if ((p_Xpos + pWidth/2 >= Exits[i].xPos) && (p_Xpos + pWidth / 2 <= Exits[i].xPos + Exits[i].xSize) && (p_Ypos + pHeight/2 >= Exits[i].yPos))
			{
				return Exits[i].direction;
			}
			break;

		case 2: 	// the only thing to check the range of for a horizontal down exit is the range of xPos and xPos + width, the yPos never changes
			if ((p_Xpos >= Exits[i].xPos) && (p_Xpos <= Exits[i].xPos + Exits[i].xSize) && (p_Ypos - pHeight/2 <= Exits[i].yPos))
			{
				return Exits[i].direction;
			}
			break;

		case 3: 	// the only thing to check the range of for a vertical right exit is the range of yPos and yPos + height, the xPos never changes
			if ((p_Ypos <= Exits[i].yPos) && (p_Ypos >= Exits[i].yPos - Exits[i].ySize) && (p_Xpos + pWidth/2 >= Exits[i].xPos))
			{
				return Exits[i].direction;
			}
			break;

		case 4: 	// the only thing to check the range of for a vertical left exit is the range of yPos and yPos + height, the xPos never changes
			if ((p_Ypos <= Exits[i].yPos) && (p_Ypos >= Exits[i].yPos - Exits[i].ySize) && (p_Xpos - pWidth/2 <= Exits[i].xPos))
			{
				return Exits[i].direction;
			}
			break;

		default:
			break;
		}
	}
	return temp;
}

int SubRoom::CheckPickupsCol(int p_Xpos, int p_Ypos)
{
	if (hasPickups == true)
	{
		if (hasSword == true)
		{
			if (p_Xpos + pWidth / 2 >= 0 && p_Ypos + pHeight / 2 >= 0)	// pickup sword
			{
				Sound::Playback(PickupSFX);
				hasSword = false;
				return 1;
			}
		}
	}

	return 0;
}

int SubRoom::CheckCollisions(int p_Xpos, int p_Ypos)
{
	for (int i = 0; i < numWalls; ++i)
	{
		if (p_Xpos + pWidth/2 >= Walls[i].leftx && p_Ypos + pHeight/2 >= Walls[i].lowery && p_Xpos - pWidth/2 <= Walls[i].rightx && p_Ypos - pHeight/2 <= Walls[i].uppery)
		{
			// the player is within this wall's bounds
			return 1;	// return 1 for wall collision
		}
	}
	return 0;
}

void SubRoom::RenderPickups()
{
	if (id == 127)	// the first cave
	{
		if (hasSword == true)
		{
			// this is for the wooden sword sprite
			xLeft = 0 - (7 / 2) * 1.43;
			xRight = 0 + (7 / 2) * 1.43;
			yBot = 0 + (16 / 2) * 1.43;
			yTop = 0 - (16 / 2) * 1.43;

			leftU = 79 / (float)imwidth;
			rightU = 86 / (float)imwidth;
			upperV = 0 / (float)imheight;
			lowerV = 15 / (float)imheight;


			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, mPickupTextureMap);
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


			// this is for the text
			xLeft = 0 - (294 / 2) * 1.12;
			xRight = 0 + (294 / 2) * 1.12; //147 x 2 to make it bigger
			yBot = 90 + (30 / 2) * 1.12;
			yTop = 90 - (30 / 2) * 1.12; //15 x 2 to make it bigger

			leftU = 89 / (float)imwidth;
			rightU = 236 / (float)imwidth;
			upperV = 16 / (float)imheight;
			lowerV = 31 / (float)imheight;

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, mPickupTextureMap);
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

			// this is for the old man
			xLeft = 0 - (16 / 2) * 2;
			xRight = 0 + (16 / 2) * 2;
			yBot = 40 + (16 / 2) * 2;
			yTop = 40 - (16 / 2) * 2;

			leftU = 0 / (float)imwidth;
			rightU = 15 / (float)imwidth;
			upperV = 16 / (float)imheight;
			lowerV = 31 / (float)imheight;

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, mPickupTextureMap);
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

		// this is for the left fire sprite
		xLeft = 128 - (16 / 2) * 2;
		xRight = 128 + (16 / 2) * 2;
		yBot = 40 + (16 / 2) * 2;
		yTop = 40 - (16 / 2) * 2;

		leftU = 48 / (float)imwidth;
		rightU = 63 / (float)imwidth;
		upperV = 16 / (float)imheight;
		lowerV = 31 / (float)imheight;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mPickupTextureMap);
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

		// this is for the right fire sprite
		xLeft = -128 - (16 / 2) * 2;
		xRight = -128 + (16 / 2) * 2;
		yBot = 40 + (16 / 2) * 2;
		yTop = 40 - (16 / 2) * 2;

		leftU = 48 / (float)imwidth;
		rightU = 63 / (float)imwidth;
		upperV = 16 / (float)imheight;
		lowerV = 31 / (float)imheight;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mPickupTextureMap);
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
}

void SubRoom::FillRoomObjects()
{
	// orientation numbers: 1 = horizontal up, 2 = horizontal down, 3 = vertical right, 4 = vertical left
	switch (id)
	{
	case 0:

		break;

	case 7:
		sExit e7Exit = CreateExit(east, 'e', 256, 8, 0, 15, 3);
		Exits[0] = e7Exit;
		numExits = 1;
		break;

	case 8:	// spawn room
		sExit nExit = CreateExit(north, 'n', -30, 176, 61, 0, 1);	// int OutRoom,char Direction,int XPos,int YPos, int XSize, int YSize, char Ori
																	// 256 x 176, 256/2 = 128 so 128 on spritesheet is 0 on x screen x
																	// 176/2 = 88 so 88 on spritesheet is 0 on screen y
		Exits[0] = nExit;
		sExit wExit = CreateExit(west, 'w', -256, 8, 0, 15, 4);	// this one goes horizontally, I started at x,y of top of hitbox, must add 1320 + 15 for the width not -15
		Exits[1] = wExit;
		sExit eExit = CreateExit(east, 'e', 256, 8, 0, 15, 3);	// same with this one
		Exits[2] = eExit;
		cave = 127; // the first cave shall be the last room in the subrooms list (128 - 1 = 127)
		sExit cExit = CreateExit(cave, 'c', -125, 119, 35, 0, 1);	// 'c' for cave
		Exits[3] = cExit;
		numExits = 4;

		sWall rightChunk = CreateWall(112, 80, 35, 256, 176, 15); // int QW, int QH, int leftX, int rightX, int upperY, int lowerY
		Walls[0] = rightChunk;
		sWall caveRChunk = CreateWall(32, 32, -120, -30, 176, 122);
		Walls[1] = caveRChunk;
		sWall botChunk = CreateWall(256, 32, -256, 256, -110, -176);
		Walls[2] = botChunk;
		sWall botLChunk = CreateWall(31, 47, -256, -200, -15, -109);
		Walls[3] = botLChunk;
		sWall botRChunk = CreateWall(31, 47, 200, 256, -15, -109);
		Walls[4] = botRChunk;
		sWall curveLChunk = CreateWall(24, 80, -256, -225, 176, 25);
		Walls[5] = curveLChunk;
		sWall curveMChunk = CreateWall(16, 71, -226, -200, 176, 35);
		Walls[6] = curveMChunk;
		sWall curveRChunk = CreateWall(16, 71, -199, -130, 176, 45);
		Walls[7] = curveRChunk;
		numWalls = 8;
		break;

	case 9:
		sExit w9Exit = CreateExit(west, 'w', -256, 8, 0, 15, 4);
		Exits[0] = w9Exit;
		numExits = 1;
		break;

	case 24:
		sExit s24Exit = CreateExit(south, 's', -30, -176, 61, 0, 2);
		Exits[0] = s24Exit;
		numExits = 1;
		break;

	case 127: // first cave
		sExit sExit = CreateExit(8, 'f', -30, -176, 61, 0, 2);	// 'f' for first room (spawn room)
		Exits[0] = sExit;
		numExits = 1;
		hasPickups = true;
		hasSword = true;
		sPickup sword = CreatePickup(Sword, 0, 0, 7, 16, 79, 86, 0, 15);	// eItemTypes ItemType, int XPos, int YPos, int IHeight, int IWidth
		Pickups[0] = sword;	// sword sprite is 7 x 16, most are 8 x 16
		numPickups = 1;
		break;

	default:
		break;
	}
}