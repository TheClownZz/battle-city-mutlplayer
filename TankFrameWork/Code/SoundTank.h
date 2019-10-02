#pragma once
#include "Sound.h"

extern HWND	G_hWnd;

class SoundTank
{
private:
	static SoundTank* Sound_Tank;
	
protected:
	//Sound
	Sound *sound;
public:
	SoundTank();
	static SoundTank* GetSoundTank();
	~SoundTank();

	//Các âm thanh hiệu ứng và nhạc nền
	GSound* soundBattleCity;

	GSound *soundTankRun;
	GSound *soundTankStand;
	GSound *soundTankDie;
	GSound *soundTankShot;

	GSound *soundEnemyDie;
	GSound *soundEnemyHP;

	GSound *soundBreak;
	GSound *soundMetal;

	GSound *soundItem;
	GSound *soundLifeUp;
	GSound *soundPowerUp;

};

