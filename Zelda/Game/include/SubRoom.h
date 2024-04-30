#pragma once
class SubRoom
{
public:

	enum eItemTypes
	{
		Sword,
		Heart,
		Bomb,
		Clock,
		Fairy,
		DangerousTxt,
		FireSprite,
		None
	};

	struct sExit {
		int outRoom;
		char direction;
		int xPos, yPos;	// bottom left pixel of exit hitbox
		int xSize, ySize;	// width and height of hitbox
		int ori; // orientation of the actual exit
	};

	struct sPickup {
		eItemTypes itemType;
		int xPos, yPos;
		int qWidth, qHeight;
		int leftx, rightx, uppery, lowery;
	};

	struct sWall {
		int qWidth, qHeight;
		int leftx, rightx, uppery, lowery;
	};

	SubRoom();
	SubRoom(int ID, int N, int W, int S, int E);
	void FillRoomObjects();
	sExit* GetExitArr();
	sExit CreateExit(int OutRoom, char Direction, int XPos, int YPos, int XSize, int YSize, int Ori);
	sPickup CreatePickup(eItemTypes ItemType, int XPos, int YPos, int QHeight, int QWidth, int leftX, int rightX, int upperY, int lowerY);
	sWall CreateWall(int QW, int QH, int leftX, int rightX, int upperY, int lowerY);
	int GetNumExits();
	char CheckExits(int p_Xpos, int p_Ypos);
	void RenderPickups();
	int CheckPickupsCol(int p_Xpos, int p_Ypos);
	int CheckCollisions(int p_Xpos, int p_Ypos);



private:
	int id, mPickupTextureMap, PickupSFX;
	bool hasPickups = false;
	bool hasSword = false;
	static SubRoom* srInstance;
	int width = 257;	// 257
	int height = 179; // 176
	int pWidth = 16;
	int pHeight = 16;
	int north, west, south, east;
	int cave = 0;
	int numExits = 1;
	int maxExits = 10;
	int numWalls = 1;
	int maxWalls = 30;
	int numPickups = 0;
	int maxPickups = 10;
	int imheight = 32;
	int imwidth = 260;	// items spritesheet is 260x32
	float xLeft = 0 - (7 / 2) * 1.43;
	float xRight = 0 + (7 / 2) * 1.43;
	float yBot = 0 + (16 / 2) * 1.43;
	float yTop = 0 - (16 / 2) * 1.43;

	float leftU = 79 / (float)imwidth;
	float rightU = 86 / (float)imwidth;
	float upperV = 0 / (float)imheight;
	float lowerV = 15 / (float)imheight;
	sExit Exits[10];
	sPickup Pickups[10];
	sWall Walls[30];
};