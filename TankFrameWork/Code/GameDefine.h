#pragma once

#include "dinput.h"

namespace Define
{
	//Windows
	const float GameWidth = 256.0f;
	const float GameHeight = 240.0f;
	const float WinWidth = GameWidth*2;
	const float WinHeight = GameHeight*2;
	const float FPSGame = 60.0f;

	//Control
	const int Dik_UP = DIK_UP;
	const int Dik_DOWN = DIK_DOWN;
	const int Dik_LEFT = DIK_LEFT;
	const int Dik_RIGHT = DIK_RIGHT;
	const int Dik_JUMP = DIK_Z;
	const int Dik_SHOOT = DIK_SPACE;
	//const int Dik_SELECT = DIK_M;
	const int Dik_START = DIK_RETURN;

	//Path
	//Menu
	const char SpriteTankPNG[] = "./Resource Files/SpriteTank/SpriteTank.png";
	const char SpriteTankXML[] = "./Resource Files/SpriteTank/SpriteTank.xml";
	const char SpriteTileScreen[] = "./Resource Files/SpriteTank/TileScreen.png";

		//Map
	const char TileSetPNG[] = "./Resource Files/MapTank/TileMap.png";
	const char Map1XML[] = "./Resource Files/MapTank/Map1.tmx";
	const char Map2XML[] = "./Resource Files/MapTank/Map2.tmx";
		//QuadTree

		//Sound
	const LPTSTR SoundBattleCity =(const LPTSTR) "./Resource Files/SoundTank/BattleCityNES.wav";

	const LPTSTR SoundTankRun = (const LPTSTR) "./Resource Files/SoundTank/TankRun.wav";
	const LPTSTR SoundTankStand = (const LPTSTR) "./Resource Files/SoundTank/TankStand.wav";
	const LPTSTR SoundTankShot = (const LPTSTR) "./Resource Files/SoundTank/TankShot.wav";
	const LPTSTR SoundTankDie = (const LPTSTR) "./Resource Files/SoundTank/TankDie.wav";

	const LPTSTR SoundEnemyDie = (const LPTSTR) "./Resource Files/SoundTank/EnemyDie.wav";
	const LPTSTR SoundEnemyHP = (const LPTSTR) "./Resource Files/SoundTank/EnemyHP.wav";

	const LPTSTR SoundBreak = (const LPTSTR) "./Resource Files/SoundTank/Break.wav";
	const LPTSTR SoundMetal = (const LPTSTR) "./Resource Files/SoundTank/Metal.wav";

	const LPTSTR SoundItem = (const LPTSTR) "./Resource Files/SoundTank/Item.wav";
	const LPTSTR SoundLifeUp = (const LPTSTR) "./Resource Files/SoundTank/LifeUp.wav";
	const LPTSTR SoundPowerUp = (const LPTSTR) "./Resource Files/SoundTank/PowerUp.wav";

	//Delay 
	const float RunDelay = 100.0f;
	const float DieDelay = 150.0f;
	//Speed
	const float RunSpeed = 5.0f;
	const float BulletSpeed = 12.0f;

	//Các giới hạn
	const int EnemyMax = 8;//số lượng xe tăng địch tối đa
	const float Immortal_Time = 3000;
	const float DEFAULT_SHOOT_COLDOWN = -500;

}