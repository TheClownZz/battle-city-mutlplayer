#pragma once
#ifndef SAVE_WORLD_H
#define SAVE_WORLD_H
#include<TankFrameWork/Code/Game.h>
#include<TankFrameWork/Code/Tank.h>
#include<TankFrameWork/Code/Map.h>
#include<TankFrameWork/Code/Item.h>
#include<TankFrameWork/Code/Boss.h>

#include<vector>
using namespace std;
class Game;
class SaveWorld
{
private:
	void SaveWorldHandleInput();
public:
	vector <Tank*> listTank;
	vector <Item*> listItem;
	vector <Boss*> listBoss;
	vector<InputState> listInput;

	Map* map;
	long time;
	float gameTime;

	SaveWorld(Game *game, long _time, float _gameTime);
	~SaveWorld();
	void UpdateState(Game *game, long _time, float _gameTime);
	void UpdateState(SaveWorld *world);
	void UpdateSaveWorld(SaveWorld *nextWorld = NULL);
};
#endif 

