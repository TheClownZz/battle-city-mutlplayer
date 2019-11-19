#pragma once
#ifndef GAME_H
#define GAME_H
#include<Server/SaveWorld.h>
#include "Graphic.h"
#include "Keyboard.h"
#include "Scene.h"
#include "GameDefine.h"
#include "SoundTank.h"
#include <Windows.h>
class SaveWorld;
using namespace Define;
using namespace PNet;
class Game
{
private:
	//HINSTANCE của chương trình 
	HINSTANCE Hinsantance;
	//HWND của của sổ
	HWND Hwnd;
	//Kích thước cửa sổ
	int Width;
	int Height;
	//Tên cửa sổ
	char WindowName[256];
	//Xử lý cửa sổ
	static HRESULT CALLBACK WindowProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

	//Khai báo đối tượng graphic
	Graphic* graphic;
	//Khai báo đối tượng Keyboard
	Keyboard* keyboard;

	Scene* SceneManager;
	uint8_t localID;
	long timeSend;

public:
	vector<InputState> listInput;
	Game(HINSTANCE Hins, int width, int height, char* name, uint8_t id = UNDEFINE_ID);
	~Game();
	//Thiết lập các giá trị cho cửa sổ
	bool InitWD();
	//Load Data Game
	void InitDT();
	//Update Game sau khoảng thời gian gameTime
	void Update(float gameTime);
	//Vẽ Object lên màn hình
	void Render();
	void CreateObject(uint8_t numPlayer, uint8_t id, int listLevelEnemy[] = NULL);
	void HandleInput();
	void ClearInput() { listInput.clear(); }
	void HandleInput(InputState input);
	void UpdateTankProperties(long timeServerSend, TankProperties listProperties[]);
	void Shoot(int tankID, BulletProperties bulletP);
	void BurstingBullet(int tankID, long timeSend);
	void SetEndGame(long timeServerSend, int team, bool isBossDead);
	void UpdateMap(vector<TileInfo> serverMap, long timeServerSend, int stageIndex);
	void ShowItem(long timeServerSend, int x, int y, int type);
	void EatItem(long timeServerSend, int playerID, int type);
	void UpdateState(SaveWorld *saveWorld);
	Scene* GetScene() { return this->SceneManager; }
private:
	void ClientSendInput();
	void ServerSendProperties();
};
#endif 


