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
#include "openglframework.h"	
#include "gamedefs.h"
#include "soil.h"
#include "PauseMenu.h"

PauseMenu* PauseMenu::pmInstance = NULL;

PauseMenu::PauseMenu()
{

}

PauseMenu* PauseMenu::CreateInstance()
{
	if (pmInstance != NULL)return pmInstance;
	else
	{
		pmInstance = new PauseMenu();
	}
	return pmInstance;
}

void PauseMenu::init()
{
	mUITextureMap = SOIL_load_OGL_texture("asset/PauseMenuSheet.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	mPickupTextureMap = SOIL_load_OGL_texture("asset/ItemsNpc.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
}

void PauseMenu::RenderMenu()
{
	xLeft = pm_xLeft;
	xRight = pm_xRight;
	yBot = pm_yBot;
	yTop = pm_yTop;

	leftU = pm_LeftxPx / (float)pmWidth;
	rightU = pm_RightxPx / (float)pmWidth;
	upperV = pm_UpperyPx / (float)pmHeight;
	lowerV = pm_LoweryPx / (float)pmHeight;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mUITextureMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// stops pixels from getting blurry
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

void PauseMenu::RenderItems(bool hasSword)
{
	if (hasSword == true)
	{
		xLeft = sword_xLeft;
		xRight = sword_xRight;
		yBot = sword_yBot;
		yTop = sword_yTop;

		leftU = sword_LeftxPx / (float)imWidth;
		rightU = sword_RightxPx / (float)imWidth;
		upperV = sword_UpperyPx / (float)imHeight;
		lowerV = sword_LoweryPx / (float)imHeight;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mPickupTextureMap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// stops pixels from getting blurry
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