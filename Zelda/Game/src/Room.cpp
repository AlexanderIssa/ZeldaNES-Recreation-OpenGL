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
#include "Room.h"


Room* Room::rInstance = NULL;

Room::Room()	// constructor
{

}

Room* Room::CreateInstance()
{
	if (rInstance != NULL)return rInstance;
	else

		rInstance = new Room();
	return rInstance;
}

void Room::init()
{
	//mMapTextureMap = SOIL_load_OGL_texture("asset/Overworld.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
	//	SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	for (int i = 0; i < numRooms; ++i)
	{
		rOverWorldArr[i] = SubRoom(i, i+16,i-1,i-16,i+1); // ID,n,w,s,e
	}

	currSubRoom = &rOverWorldArr[8]; // sub room 8 is the subroom link spawns in, i think this locks the curr room into [8] and changes its id to the curr room id
	currSRNum = 8;
	stairsSFX = Sound::LoadSound("asset/LOZ_Stairs.wav");
}
void Room::shutdown()
{
	//free(rOverWorldArr); // only use this if we use malloc
}

SubRoom* Room::GetSubRooms()
{
	return rOverWorldArr;
}

SubRoom& Room::GetCurrSR()
{
	return rOverWorldArr[currSRNum];
}

void Room::MoveNorth()
{
	assert(currSRNum + 16 <= numRooms);
	currSRNum += 16;
	currSubRoom = &rOverWorldArr[currSRNum];	// currSubRoom changes only effect the 8th index
}

void Room::MoveEast()
{
	assert(currSRNum + 1 <= numRooms);
	currSRNum += 1;
	currSubRoom = &rOverWorldArr[currSRNum];
}

void Room::MoveWest()
{
	assert(currSRNum - 1 >= 0);
	currSRNum -= 1;
	currSubRoom = &rOverWorldArr[currSRNum];
}

void Room::MoveSouth()
{
	assert(currSRNum - 16 >= 0);
	currSRNum -= 16;
	currSubRoom = &rOverWorldArr[currSRNum];
}

void Room::MoveToCave()
{
	currSRNum = 127; // this is soley for the first cave which should be index 128
	currSubRoom = &rOverWorldArr[currSRNum];
	Sound::Playback(stairsSFX);
}

void Room::MoveToRoom(int roomNum)
{
	currSRNum = roomNum; // this is to move to a specific room number
	currSubRoom = &rOverWorldArr[currSRNum];
}

// THIS DEACTIVATED CONTROLS FOR SOME REASON
//void Room::RenderRoomSprite()
//{
//	// first room info:
//	// bottom left pixel = 1799 1415
//	// top left pixel = 1799 1239
//	// top right pixel = 2056 1239
//	// bottom right pixel = 2056 1415
//
//	float xLeft = -quadWidth; //-960
//	float xRight = quadWidth; //960
//	float yBot = -quadHeight * 1.56; //540 * 1.56
//	float yTop = quadHeight * 1.56; // -540 * 1.56
//
//	float leftU = 1798 / (float)width; // 1798 / (float)width // 0.50
//	float rightU = 2055 / (float)width; // 2055 / (float)width // 0.75
//	float upperV = 1414 / (float)height; // 1414 / (float)height // 0.89
//	float lowerV = 1239 / (float)height; // 1239 / (float)height // 1
//
//	//mMapTextureMap = SOIL_load_OGL_texture("asset/Overworld.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
//	//	SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
//
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, mMapTextureMap);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// stops pixels from getting blurry
//	glDisable(GL_CULL_FACE);
//
//	glBegin(GL_QUADS);
//
//	glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
//
//	// top left of texture (0,1)
//	glTexCoord2f(leftU, 1 - lowerV);
//	glVertex3f(xLeft, yTop, 0.0);
//
//	// top right of texture (1,1)
//	glTexCoord2f(rightU, 1 - lowerV);
//	glVertex3f(xRight, yTop, 0.0);
//
//	// bottom right of texture (1,0)
//	glTexCoord2f(rightU, 1 - upperV);
//	glVertex3f(xRight, yBot, 0.0);
//
//	// bottom left of texture (0,0)
//	glTexCoord2f(leftU, 1 - upperV);
//	glVertex3f(xLeft, yBot, 0.0);	// printing to the output screen
//
//	glEnd();
//
//}