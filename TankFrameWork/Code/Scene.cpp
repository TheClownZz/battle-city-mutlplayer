#include "Scene.h"


Scene::Scene()
{
}


Scene::~Scene()
{
	delete sound;
}

//Load Data Game
void Scene::InitDT(Graphic* graphic)
{
	//sound
	sound = new Sound(graphic->GetHwnd());
	sound->Init_DirectSound();
	//Tạo ảnh Menu
	timedelay = 0.0f;

	//Tạo GamePlay
	ObjectManager::GetObjectManager()->InitDT(graphic, sound);
}

//Update Game sau khoảng thời gian gameTime
void Scene::Update(float gameTime, Keyboard* key)
{
	ObjectManager::GetObjectManager()->Update(gameTime, key);

	int end = ObjectManager::GetObjectManager()->End();

	if (end != -1)
	{
		timedelay += gameTime;
		//sau 3s chuyển scene
		if (timedelay >= 3000)
		{
			timedelay = 0.0f;
			//Chuyển Scene
			ObjectManager::GetObjectManager()->Start(1);
		}
	}
}

//Vẽ Object lên màn hình
void Scene::Render()
{
	ObjectManager::GetObjectManager()->Render();
}

ObjectManager * Scene::GetObjectManager()
{
	return ObjectManager::GetObjectManager();
}
