#pragma once
#include "tinyxml.h"
#include "Sprite.h"
#include "Object.h"
#include "Tank.h"
#include<vector>

class Map
{
protected:
	Sprite* sprite;
	Object* object_0;
	Object* object_1;

	RECT rect_tile[20];
	int** MapMatrix;
	int Width, Height, TileCount, TileColumns;
	float TileWidth, TileHeight;

	float TimeChangeRiver, TimeChangeMetalWall;
	bool ChangeRiver, ChangMetalWall;
	vector<TileInfo> listTile;
	long timeSend;
	int stageIndex;
private:
	void SaveTile(int x, int y, int value);
	void ServerSendUpdateMap();
public:
	Map(Sprite* sprite);
	~Map();

	//kiểm tra đối tượng cần sét va chạm
	void FindObjectMap(Object* object, int derection, int &x0, int &y0, int &x1, int &y1);

	//Đọc tmx
	void ReadMap(const char *path);

	
	//SetMetalWall
	bool isChangMetalWall();
	void SetWallTeam0(int id);
	void SetWallTeam1(int id);
	void SetUITankLife(std::vector <Tank*> &ListTank, int numPlayer);
	void SetUIIcon(int numplayer);
	//New
	void New(int level);

	int** GetMapMatrix();

	//Va chạm
	void OnCollision(std::vector <Tank*> &ListTank, float gameTime);

	//Update
	void Update(float gameTime);

	//Render
	void Render(Viewport* viewport);
	void RenderTree(Viewport* viewport);
	void UpdateTileMap(vector<TileInfo> serverMap, long timeSend, int stageIndex);
};

