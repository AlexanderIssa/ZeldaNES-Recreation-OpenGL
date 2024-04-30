#pragma once
class Room
{
public:
	Room();
	static Room* CreateInstance();
	static Room* GetInstance() { return rInstance; };
	void init();
	void shutdown();
	SubRoom* GetSubRooms();
	SubRoom& GetCurrSR();
	void MoveNorth();
	void MoveSouth();
	void MoveEast();
	void MoveWest();
	void MoveToCave();
	void MoveToRoom(int roomNum);

private:
	static Room* rInstance;
	GLuint mMapTextureMap = 0;
	int width = 4113;	// 257
	int height = 1598; // 176
	int quadWidth = 1920;
	int quadHeight = 1080;
	int numRooms = 128; // 129 rooms (including first cave) - 1 for 0 index
	int currSRNum = 8;
	int stairsSFX;
	SubRoom rOverWorldArr[128];
	SubRoom* currSubRoom = rOverWorldArr;
};