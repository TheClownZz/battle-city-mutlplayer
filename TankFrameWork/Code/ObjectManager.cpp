#include<PNet\Server.h>
#include "ObjectManager.h"
#include "string.h"
#include<iostream>
#include <time.h>

#pragma warning(disable : 4996)

ObjectManager* ObjectManager::instance = NULL;


ObjectManager* ObjectManager::GetObjectManager()
{
	if (instance == NULL)
	{
		instance = new ObjectManager();
	}
	return instance;
}

ObjectManager::~ObjectManager()
{
	delete SpriteTank;
	delete SpriteMap;
	delete viewport;
	delete map;
	//delete tank
	for (size_t i = 0; i < ListTank.size(); i++)
	{
		Tank* tank_0 = ListTank.at(i);
		delete tank_0;
	}
	//delete boss
	for (size_t i = 0; i < ListBoss.size(); i++)
	{
		Boss* boss_0 = ListBoss.at(i);
		delete boss_0;
	}
}

//Load Data Game
void ObjectManager::InitDT(Graphic* graphic, Sound *_sound)
{
	//
	this->sound = _sound;
	this->SpriteTank = new Sprite(graphic, SpriteTankPNG, 16, 16, 320, 16);
	this->SpriteMap = new Sprite(graphic, TileSetPNG, 8, 8, 18, 18);
	//Tạo Viewport
	viewport = new Viewport(GameWidth, GameHeight);
	viewport->SetPosition(D3DXVECTOR2(0.0f, GameHeight));

	//Tạo Object 
		//Object Map và map
	map = new Map(SpriteMap);
	map->New(1);
	map->SetUIIcon(numPlayer);

	//Boss team
	Boss *boss_0 = new Boss(SpriteTank, sound, 0);//Boss team 0
	boss_0->SetPositionStart(D3DXVECTOR2(120.0f, 16.0f));
	Boss *boss_1 = new Boss(SpriteTank, sound, 1);//Boss tean 1
	boss_1->SetPositionStart(D3DXVECTOR2(120.0f, 224.0f));
	ListBoss.push_back(boss_0);
	ListBoss.push_back(boss_1);
	for (size_t i = 0; i < ListBoss.size(); i++)
	{
		ListBoss.at(i)->New();
	}
	Item *item = new Item(SpriteTank, sound);
	ListItem.push_back(item);
}

void ObjectManager::ServerCreateObject(int _numPlayer)
{
	D3DXVECTOR2 listPos[4], listPosEnemy[4];
	listPos[0] = D3DXVECTOR2(88.0f, 16.0f);
	listPos[1] = D3DXVECTOR2(88.0f, 224.0f);
	listPos[2] = D3DXVECTOR2(152.0f, 16.0f);
	listPos[3] = D3DXVECTOR2(152.0f, 224.0f);

	listPosEnemy[0] = D3DXVECTOR2(24.0f, 80);
	listPosEnemy[1] = D3DXVECTOR2(24.0f, 160.0f);
	listPosEnemy[2] = D3DXVECTOR2(216.0f, 80.0f);
	listPosEnemy[3] = D3DXVECTOR2(216.0f, 160.0f);

	numPlayer = _numPlayer;

	// player
	for (int i = 0; i < numPlayer; i++)
	{
		int team = 0;
		if (i % 2 == 1)
			team = 1;
		TankPlayer *tankP = new TankPlayer(SpriteTank, sound, team, i);
		tankP->SetPositionStart(listPos[i]);
		ListTank.push_back(tankP);
	}

	Packet p(PacketType::PT_Start);
	p.write_bits(Server::serverPtr->totalConnect, 3);
	// enemy
	srand(time(NULL));
	for (int i = 0; i < NUM_ENEMY; i++)
	{
		int level = 4 + (rand() % 999) % 4;//Ngẫu nhiên loại Tank
		TankEnemy *tankE = new TankEnemy(SpriteTank, sound, numPlayer + i, level);
		tankE->SetPositionStart(listPosEnemy[i]);
		ListTank.push_back(tankE);
		//ListItem.push_back(tankE->GetItemTank());
		p.write_bits(level, 4);
	}

	for (int i = 0; i < Server::serverPtr->totalConnect; i++)
		Server::serverPtr->connections[i].pm.Append(p);

	for (size_t i = 0; i < ListTank.size(); i++)
	{
		ListTank.at(i)->New();
	}
	Start(1);
}

void ObjectManager::ClientCreateObject(int _numPlayer, int listLevelEnemy[])
{
	D3DXVECTOR2 listPos[4], listPosEnemy[4];
	listPos[0] = D3DXVECTOR2(88.0f, 16.0f);
	listPos[1] = D3DXVECTOR2(88.0f, 224.0f);
	listPos[2] = D3DXVECTOR2(152.0f, 16.0f);
	listPos[3] = D3DXVECTOR2(152.0f, 224.0f);

	listPosEnemy[0] = D3DXVECTOR2(24.0f, 80);
	listPosEnemy[1] = D3DXVECTOR2(24.0f, 160.0f);
	listPosEnemy[2] = D3DXVECTOR2(216.0f, 80.0f);
	listPosEnemy[3] = D3DXVECTOR2(216.0f, 160.0f);

	numPlayer = _numPlayer;

	// player
	for (int i = 0; i < numPlayer; i++)
	{
		int team = 0;
		if (i % 2 == 1)
			team = 1;
		TankPlayer *tankP = new TankPlayer(SpriteTank, sound, team, i);
		tankP->SetPositionStart(listPos[i]);
		ListTank.push_back(tankP);
	}


	// enemy
	for (int i = 0; i < NUM_ENEMY; i++)
	{
		TankEnemy *tankE = new TankEnemy(SpriteTank, sound, numPlayer + i, listLevelEnemy[i]);
		tankE->SetPositionStart(listPosEnemy[i]);
		ListTank.push_back(tankE);
		//ListItem.push_back(tankE->GetItemTank());
	}

	for (size_t i = 0; i < ListTank.size(); i++)
	{
		ListTank.at(i)->New();
	}
	// Start with map 1
	Start(1);
}

//Update Game
void ObjectManager::Update(float gameTime, Keyboard* key)
{
	if (numPlayer == 0)
		return;
	//Kiểm tra va chạm Tank
	for (size_t i = 0; i < ListTank.size(); i++)
	{
		ListTank.at(i)->ChangeAnimation(gameTime);

		//Kiểm tra va chạm với Tank khác
		for (size_t j = 0; j < ListTank.size(); j++)
		{
			if (i != j)
			{
				//Va chạm với Tank
				ListTank.at(i)->OnCollision(ListTank.at(j), gameTime);
			}
		}
	}

	//Kiểm tra va chạm Item với TankPlayer và Update
	for (size_t i = 0; i < ListItem.size(); i++)
	{
		ListItem.at(i)->OnCollision(ListTank, map);
		//Update Item
		ListItem.at(i)->Update(gameTime);
	}

	//Kiểm tra va chạm Boss với Tank và Update
	for (size_t i = 0; i < ListBoss.size(); i++)
	{
		ListBoss.at(i)->OnCollision(ListTank, gameTime);
		//Update Boss
		ListBoss.at(i)->Update(gameTime);
	}

	//Kiểm tra va chạm MapObject với Tank
	map->OnCollision(ListTank, gameTime);
	//Set UI Tank Life
	map->SetUITankLife(ListTank, numPlayer);
	//Update Map
	map->Update(gameTime);

	//Update Tank
	for (size_t i = 0; i < ListTank.size(); i++)
	{
		ListTank.at(i)->Update(gameTime);
	}
}
//Thông số bắt đầu chơi
void ObjectManager::Start(int stage)
{
	endGame = -1;
	//New đối tượng
	for (size_t i = 0; i < ListItem.size(); i++)
	{
		ListItem.at(i)->SetItemType(Item::NoneItem);
		ListItem.at(i)->SetState(Item::None);
		ListItem.at(i)->SetAllowDraw(false);
		ListItem.at(i)->SetBoundZero();
	}
	for (size_t i = 0; i < ListBoss.size(); i++)
	{
		ListBoss.at(i)->New();

	}
	for (size_t i = 0; i < ListTank.size(); i++)
	{
		ListTank.at(i)->SetLife(3);
		ListTank.at(i)->New();
	}
	map->New(stage);
	map->SetUIIcon(numPlayer);
	SoundTank::GetSoundTank()->soundBattleCity->Play();
}
//Kiểm tra điều kiện kết thúc game
int ObjectManager::End()
{
	if (endGame != -1)
		return endGame;
	if (Server::serverPtr == NULL)
		return -1;
	//Kiểm tra với Boss
	bool isBoolDead = false;
	for (size_t i = 0; i < ListBoss.size(); i++)
	{
		if (ListBoss.at(i)->GetState() == Boss::Dead)
		{
			endGame = ListBoss.at(i)->GetTeam();
			isBoolDead = true;
			break;
		}
	}
	if (endGame == -1)
	{
		bool lifeTeam0 = false;
		bool lifeTeam1 = false;
		int numTeam0 = 0;
		int numTeam1 = 0;
		for (int i = 0; i < numPlayer; i++)
		{
			if (ListTank.at(i)->GetTeam() == 0)
			{
				numTeam0++;
				if (ListTank.at(i)->GetState() != Statetank::Dead)
					lifeTeam0 = true;
			}
			else
			{
				numTeam1++;
				if (ListTank.at(i)->GetState() != Statetank::Dead)
					lifeTeam1 = true;
			}
		}

		// send end game
		if (!lifeTeam0 && numTeam0 > 0)
		{
			endGame = 0;
		}
		if (!lifeTeam1 && numTeam1 > 0)
		{
			endGame = 1;
		}
	}
	if (endGame != -1)
	{
		Packet p(PacketType::PT_Reset);
		long time = GetTickCount();
		p.write_bits(endGame, 1);
		p.write_bits(isBoolDead, 1);
		p.write_bits(time, sizeof(long) * 8);
		for (int i = 0; i < Server::serverPtr->totalConnect; i++)
		{
			Server::serverPtr->connections[i].pm.Append(p);
		}
		cout << "PT_Reset" << endl;
	}
	return endGame;
}

void ObjectManager::SetClientEnd(int end)
{
	endGame = end;
}

void ObjectManager::Render()
{
	//Vẽ map
	map->Render(viewport);

	//Boss
	for (size_t i = 0; i < ListBoss.size(); i++)
	{
		ListBoss.at(i)->Render(viewport);
	}
	//Tank
	for (size_t i = 0; i < ListTank.size(); i++)
	{
		ListTank.at(i)->Render(viewport);
	}
	//Vẽ Tree
	map->RenderTree(viewport);
	//Vẽ Item
	for (size_t i = 0; i < ListItem.size(); i++)
	{
		ListItem.at(i)->Render(viewport);
	}
}
