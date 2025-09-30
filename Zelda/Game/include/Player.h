#pragma once

typedef enum {
	NONE,
	RIGHT,
	LEFT,
	UP,
	DOWN
} MoveDirection;

class Player
{
public:
	Player();
	void Move(MoveDirection direction);
	void init();
	void shutdown();
	static Player* CreateInstance();
	static Player* GetInstance() { return pInstance; };
	static void DeleteInstance() { delete pInstance; };
	bool GetCanMove() { return canMove; };
	bool GetIsAttackng() { return isAttacking; };
	void RenderPlayer();
	int GetXPos();
	int GetYPos();
	void invXPos(int spawnOffset);
	void invYPos(int spawnOffset);
	void SetPlayerPos(int x, int y);
	void SetHasSword(bool value);
	void updateSprite(DWORD milliseconds);
	bool GetHasSword();
	void Attack();
	void DisableMovementInput();
	void EnableMovementInput();
	void WallHandler(int X, int Y);
	void EnableDirections();

private:
	GLuint mLinkTextureMap;
	GLuint mLinkAttacksTextureMap;
	GLuint mLinkWalkTextureMap;
	static Player* pInstance;
	int x, y;
	int height = 58;
	int width = 213;	// link's sprite sheet is 16 x 128
	int quadWidth = 16;
	int quadHeight = 16; // quad width and height are 1:1 to the sprites dimensions
	int moveSpeed = 1;
	int attackRange = 11;
	int swordHitRX, swordHitLX;	// sword's hitbox left and right most X
	int swordHitUY, swordHitLY; // sword's hitbox upper and lower y
	int swordSwingSFX, swordBeamSFX;

	//float xLeft = x - (quadWidth / 2); //-960
	//float xRight = x + (quadWidth / 2); //960
	//float yBot = y + (quadHeight / 2) * 1.56; //540 * 1.56
	//float yTop = y - (quadHeight / 2) * 1.56; // -540 * 1.56

	float leftX = 0;
	float rightX = 15;
	float upperY = 42;
	float lowerY = 57;

	float xBounds = 256; // screen bounds to tie the player to
	float yBounds = 176;

	bool hasSword = false;
	bool isMoving = false;
	bool canMove = true;
	bool isAttacking = false;
	bool isPickingUp = false;
	bool canMoveUp = true;
	bool canMoveDown = true;
	bool canMoveRight = true;
	bool canMoveLeft = true;

	DWORD mLastUpdateTime = 0;
	DWORD mCurrentTime = 0;

	int currentFrame, totalFramesPerWalk, totalFramesPerAttack;
	MoveDirection currDirection, prevDirection;

	//float leftU = leftX / (float)width;
	//float rightU = rightX / (float)width;
	//float upperV = upperY / (float)height;
	//float lowerV = lowerY / (float)height;

};