#define STATE_MANAGER_CPP
#include <windows.h>											// Header File For Windows
#include <stdio.h>												// Header File For Standard Input / Output
#include <stdarg.h>												// Header File For Variable Argument Routines
#include <math.h>												// Header File For Math Operations
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include "glut.h"
#include <assert.h>
#include "baseTypes.h"
#include "openglframework.h"	
#include "gamedefs.h"
#include "spriteDemoManager.h"
#include "random.h"

#include "soil.h"

SpriteDemoManagerC* SpriteDemoManagerC::sInstance=NULL;


SpriteDemoManagerC *SpriteDemoManagerC::CreateInstance()
{
	if(sInstance!=NULL)return sInstance;
	else

	sInstance = new SpriteDemoManagerC();
	return sInstance;
}
void SpriteDemoManagerC::init(int height, int width)
{
/* load an image file directly as a new OpenGL texture */
	mSpriteTextureMap = SOIL_load_OGL_texture("asset/Overworld.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	mTitleTextureMap = SOIL_load_OGL_texture("asset/TitleSheet.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
}

// spots on output screen to print texture to
// x = -1920,1920, y = -1080 * AR, 1080 * AR
// Aspect Ratio(AR) = 1080/1920 = 0.56 + 1 = 1.56
// 257w x 177h for each section in the world map (top right pixel)
// 4113w x 1598h total dimensions of map
// 4113w x 1420h with the bottom ignored
// first room is 8 to the right
// 2056 pixel on x,  1420 pixel on y
// 0.50 u, 0.89 v bottom left pixel of room


void SpriteDemoManagerC::RenderTitle()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mTitleTextureMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// stops pixels from getting blurry
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBegin(GL_QUADS);

	leftU = 1 / (float)1029;	// the actual sprite left u / width_of_sheet
	rightU = 256 / (float)1029;	// the actual sprite right u / width_of_sheet
	upperV = 234 / (float)721;	// the actual sprite upper v / height_of_sheet
	lowerV = 11 / (float)721;	// the actual sprite lower v / height_of_sheet

	xLeft = -256;
	xRight = 256;
	yBot = 232;
	yTop = -224;

	glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
	// 0.16 on the u (x) for the basic movement sheet, 0-1 for the v (y)

	// bottom left of texture (0,0)
	glTexCoord2f(leftU, 1 - upperV);
	glVertex3f(xLeft, yTop, 0.0);	// printing to the output screen

	// bottom right of texture (1,0)
	glTexCoord2f(rightU, 1 - upperV);
	glVertex3f(xRight, yTop, 0.0);

	// top right of texture (1,1)
	glTexCoord2f(rightU, 1 - lowerV);
	glVertex3f(xRight, yBot, 0.0);

	// top left of texture (0,1)
	glTexCoord2f(leftU, 1 - lowerV);
	glVertex3f(xLeft, yBot, 0.0);

	glEnd();
}

void SpriteDemoManagerC::ChangeRender()
{
	xLeft = -256;
	xRight = 256;
	yBot = 176;
	yTop = -176;

	leftU = 1800 / (float)4113;
	rightU = 2055 / (float)4113;
	upperV = 1414 / (float)1598;
	lowerV = 1239 / (float)1598;
}

void SpriteDemoManagerC::RenderSprites()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, mSpriteTextureMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// stops pixels from getting blurry
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBegin (GL_QUADS);

	glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
	// 0.16 on the u (x) for the basic movement sheet, 0-1 for the v (y)

	// bottom left of texture (0,0)
	glTexCoord2f(leftU, 1 - upperV);
	//glTexCoord2f(0.50, 0.89);
	glVertex3f(xLeft, yTop, 0.0);	// printing to the output screen

	// bottom right of texture (1,0)
	glTexCoord2f(rightU, 1 - upperV);
	//glTexCoord2f(0.75, 0.89);
	glVertex3f(xRight, yTop, 0.0);

	// top right of texture (1,1)
	glTexCoord2f(rightU, 1 - lowerV);
	//glTexCoord2f(0.75, 1);
	glVertex3f(xRight, yBot, 0.0);

	// top left of texture (0,1)
	glTexCoord2f(leftU, 1 - lowerV);
	//glTexCoord2f(0.50, 1);
	glVertex3f(xLeft, yBot, 0.0);

	glEnd ();
}

void SpriteDemoManagerC::MoveUVLeft()
{
	leftU = leftU * (float)tWidth; // reset back to the actual left x pixel in the sprite sheet
	leftU = (leftU - subRoomWidth) / (float)tWidth;	// get the actual left x pixel dec by the width of one subroom to set it to the left subroom

	rightU = rightU * (float)tWidth;	// do the same with the rightmost x pixel
	rightU = (rightU - subRoomWidth) / (float)tWidth;
}

void SpriteDemoManagerC::MoveUVRight()
{
	leftU = leftU * (float)tWidth; // reset back to the actual left x pixel in the sprite sheet
	leftU = (leftU + subRoomWidth) / (float)tWidth;	// get the actual left x pixel inc by the width of one subroom to set it to the right subroom

	rightU = rightU * (float)tWidth;	// do the same with the rightmost x pixel
	rightU = (rightU + subRoomWidth) / (float)tWidth;
}

void SpriteDemoManagerC::MoveUVUp()
{
	lowerV = lowerV * (float)tHeight; // reset back to the actual left x pixel in the sprite sheet
	lowerV = (lowerV - subRoomHeight) / (float)tHeight;	// get the actual left x pixel inc by the width of one subroom to set it to the right subroom

	upperV = upperV * (float)tHeight;	// do the same with the rightmost x pixel
	upperV = (upperV - subRoomHeight) / (float)tHeight;
}

void SpriteDemoManagerC::MoveUVDown()
{
	lowerV = lowerV * (float)tHeight; // reset back to the actual left x pixel in the sprite sheet
	lowerV = (lowerV + subRoomHeight) / (float)tHeight;	// get the actual left x pixel inc by the width of one subroom to set it to the right subroom

	upperV = upperV * (float)tHeight;	// do the same with the rightmost x pixel
	upperV = (upperV + subRoomHeight) / (float)tHeight;
}

void SpriteDemoManagerC::MoveUVToCave()
{
	// this leads the uv pointer to the cave uv
	leftU = 1 / (float)tWidth;
	rightU = 256 / (float)tWidth;
	lowerV =  1421 / (float)tHeight;
	upperV = 1596 / (float)tHeight;
}

void SpriteDemoManagerC::MoveUVToFirstRoom()
{
	leftU = 1800 / (float)4113;
	rightU = 2055 / (float)4113;
	upperV = 1414 / (float)1598;
	lowerV = 1239 / (float)1598;
}