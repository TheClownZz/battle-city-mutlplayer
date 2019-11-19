#pragma once
#ifndef MAP_H
#define MAP_H
#include<PNet/Constants.h>
#include "Tank.h"
#include "tinyxml.h"
#include "Sprite.h"
#include<vector>
class Object;
class Tank;
class Bullet;
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
	vector<PNet::TileInfo> listTile;
	long timeSend;
	int stageIndex;
private:
	void SaveTile(int x, int y, int value);
public:
	Map(Sprite* sprite);
	Map(Map *map);
	~Map();

	void CopyMap(Map *map);

	//kiểm tra đối tượng cần sét va chạm
	void FindObjectMap(Object* object, int derection, int &x0, int &y0, int &x1, int &y1);

	//Đọc tmx
	void ReadMap(const char *path);

	
	void SetWallTeam0(int id);
	void SetWallTeam1(int id);
	void SetUITankLife(std::vector <Tank*> &ListTank, int numPlayer);
	void SetUIIcon(int numplayer);
	//New
	void New(int level);

	int** GetMapMatrix();

	//Va chạm
	void OnCollision(std::vector <Tank*> &ListTank, float gameTime);
	void OnCollision(Bullet *bullet,int level, float gameTime);
	void OnCollision(Tank *tank, float gameTime);

	//Update
	void Update(float gameTime);

	//Render
	void Render(Viewport* viewport);
	void RenderTree(Viewport* viewport);
	void UpdateTileMap(vector<PNet::TileInfo> serverMap, long timeSend, int stageIndex);
	void ServerSendUpdateMap();
};
#endif
