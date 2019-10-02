#include "SoundTank.h"
#include "Graphic.h"
#include "GameDefine.h"

HWND G_hWnd;

SoundTank* SoundTank::Sound_Tank = NULL;

SoundTank* SoundTank::GetSoundTank()
{
	if (Sound_Tank == NULL)
	{
		Sound_Tank = new SoundTank();
	}
	return Sound_Tank;
}

SoundTank::SoundTank()
{
	//sound
	sound = new Sound(G_hWnd);
	sound->Init_DirectSound();
	//effect
	this->soundBattleCity = sound->LoadSound(Define::SoundBattleCity);

	this->soundTankRun = sound->LoadSound(Define::SoundTankRun);
	this->soundTankStand = sound->LoadSound(Define::SoundTankStand);
	this->soundTankDie = sound->LoadSound(Define::SoundTankDie);
	this->soundTankShot = sound->LoadSound(Define::SoundTankShot);

	this->soundEnemyDie = sound->LoadSound(Define::SoundEnemyDie);
	this->soundEnemyHP = sound->LoadSound(Define::SoundEnemyHP);

	this->soundBreak = sound->LoadSound(Define::SoundBreak);
	this->soundMetal = sound->LoadSound(Define::SoundMetal);

	this->soundItem = sound->LoadSound(Define::SoundItem);
	this->soundLifeUp = sound->LoadSound(Define::SoundLifeUp);
	this->soundPowerUp = sound->LoadSound(Define::SoundPowerUp);
}

SoundTank::~SoundTank()
{
	delete sound;
	//Sound effect
	delete soundBattleCity;

	delete soundTankRun;
	delete soundTankStand;
	delete soundTankDie;
	delete soundTankShot;

	delete soundEnemyDie;
	delete soundEnemyHP;

	delete soundBreak;
	delete soundMetal;

	delete soundItem;
	delete soundLifeUp;
	delete soundPowerUp;
}