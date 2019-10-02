#pragma once
#include "Graphic.h"
#include "Keyboard.h"
#include "Sound.h"
#include "Sprite.h"
#include "Tank.h"
#include "Bullet.h"
#include "TankPlayer.h"
#include "TankEnemy.h"
#include "Bullet.h"
#include "Boss.h"
#include "Map.h"
#include "Viewport.h"
#include "Collision.h"
#include <vector>
#include "GameDefine.h"
#include"SoundTank.h"
using namespace Define;

struct TankP
{
	float x;
	float y;
	Statetank state;
	int level;
	int life;
	//bool Immortal;
	//bool ReLoad;
};

class ObjectManager
{
private:
	static ObjectManager* instance;
	ObjectManager() {};
protected:
	Sprite* SpriteTank;
	Sprite* SpriteMap;
	Viewport* viewport;
	Map* map;
	//GUI
	//QuadTree

	//Sound
	GSound* soundTank;
	GSound* soundRun;
	GSound* soundShoot;
	GSound* soundBreak;
	GSound* soundDie;
	Sound*	sound;
	//Các Object


	vector <Tank*> ListTank;
	vector <Bullet*> ListBullet;
	vector <Item*> ListItem;
	vector <Boss*> ListBoss;

	int numPlayer = 0;
	int endGame = -1;

public:
	static ObjectManager* GetObjectManager();
	~ObjectManager();

	//Load Data Game
	void InitDT(Graphic* graphic, Sound *sound);

	void ServerCreateObject(int numPlayer);

	void ClientCreateObject(int numPlayer, int listLevelEnemy[]);
	//Update Game sau khoảng thời gian gameTime
	void Update(float gameTime, Keyboard* key);

	//Vẽ Object lên màn hình
	void Render();

	//Bắt đầu chơi reset lại
	void Start(int stage);
	//Điều kiện kết thúc game
	int End();

	vector <Tank*> GetListTank() { return ListTank; }
	vector <Boss*> GetListBoss() { return ListBoss; }
	Map* GetMap() { return map; }
	void SetClientEnd(int end);

};
