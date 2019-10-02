#pragma once
#include "Graphic.h"
#include "Keyboard.h"
#include "Sprite.h"
#include "Animation.h"
#include "ObjectManager.h"
#include "Sound.h"
#include "GameDefine.h"

using namespace Define;

class Scene
{
protected:

	//Sound
	Sound *sound;
	//Các âm thanh hiệu ứng và nhạc nền
public:
	float timedelay;

	Scene();
	~Scene();

	//Load Data Game
	void InitDT(Graphic* graphic);

	//Update Game sau khoảng thời gian gameTime
	void Update(float gameTime, Keyboard* key);

	//Vẽ Object lên màn hình
	void Render();

	ObjectManager* GetObjectManager();
};

