#include <Windows.h>

class SpriteDemoManagerC
{
public:
	static SpriteDemoManagerC	*CreateInstance();
	static SpriteDemoManagerC	*GetInstance() {return sInstance;};
	~SpriteDemoManagerC(){};

	void		init(int width, int height);
	void		RenderSprites();
	void		RenderTitle();

	void	ChangeRender();
	void	MoveUVLeft();
	void	MoveUVRight();
	void	MoveUVUp();
	void	MoveUVDown();
	void	MoveUVToCave();
	void	MoveUVToFirstRoom();

private:
	static SpriteDemoManagerC *sInstance;
	SpriteDemoManagerC() {};

	float xLeft = -256; //  -1920, -257
	float xRight = 256;	// 1920, 257
	float yBot = 176; //1080 * 1.56, 176 * 1.68
	float yTop = -176; // -1080 * 1.56, -176 * 1.68
	int subRoomWidth = 257;
	int subRoomHeight = 176;
	int tWidth = 4113;
	int tHeight = 1598;

	float leftU = 1800 / (float)4113; // 1798 / (float)width // 0.50
	float rightU = 2055 / (float)4113; // 2055 / (float)width // 0.75
	float upperV = 1414 / (float)1598; // 1414 / (float)height // 0.89
	float lowerV = 1239 / (float)1598; // 1239 / (float)height // 1

	GLuint mSpriteTextureMap = 0;
	GLuint mTitleTextureMap = 0;

	DWORD mLastUpdateTime = 0;
	DWORD mCurrentTime = 0;
};