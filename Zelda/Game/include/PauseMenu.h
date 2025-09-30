#pragma once

// this is not a pause menu... it is your heads-up-display (HUD)...
class PauseMenu
{
public:
	PauseMenu();
	void init();
	static PauseMenu* CreateInstance();
	static PauseMenu* GetInstance() { return pmInstance; };
	static void DeleteInstance() { delete pmInstance; };
	void RenderMenu();
	void RenderItems(bool hasSword);

private:
	static PauseMenu* pmInstance;
	GLuint mUITextureMap = 0;
	GLuint mPickupTextureMap = 0;

	int pmHeight = 238;
	int pmWidth = 256;	// pause menu sprite sheet is 256 x 238

	int imHeight = 32;
	int imWidth = 260;	// item sprite sheet is 260 x 32

	float pm_xLeft = -256;	// pm stands for pausemenu or the UI on the top of the screen
	float pm_xRight = 256;
	float pm_yBot = 232;
	float pm_yTop = 177;

	float pm_LeftxPx = 0;
	float pm_RightxPx = 255;
	float pm_UpperyPx = 182;
	float pm_LoweryPx = 237;


	float sword_QW = 7;
	float sword_QH = 16;

	float sword_xLeft = 57 - (sword_QW / 2) * 1.43;	// where the sword image goes on screen on the UI
	float sword_xRight = 57 + (sword_QW / 2) * 1.43;
	float sword_yBot = 200 + (sword_QH / 2) * 1.43;
	float sword_yTop = 200 - (sword_QH / 2) * 1.43;

	float sword_LeftxPx = 79;
	float sword_RightxPx = 86;
	float sword_UpperyPx = 0;
	float sword_LoweryPx = 15;


	float xLeft = pm_xLeft;
	float xRight = pm_xRight;
	float yBot = pm_yBot;
	float yTop = pm_yTop;

	float leftU = pm_LeftxPx / (float)pmWidth;
	float rightU = pm_RightxPx / (float)pmWidth;
	float upperV = pm_UpperyPx / (float)pmHeight;
	float lowerV = pm_LoweryPx / (float)pmHeight;
};

