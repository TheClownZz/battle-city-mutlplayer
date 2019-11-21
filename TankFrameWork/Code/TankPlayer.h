#pragma once
#include "Tank.h"
class TankPlayer : public Tank
{
protected:
	Animation * EffectAnimation;
public:
	TankPlayer(Sprite* sprite, Sound* sound, int team, int id);
	TankPlayer(Tank *tank) :Tank(tank) { this->EffectAnimation = NULL; }

	~TankPlayer();
	void SetLevel(int level);
	void SetLife(int life);
	int GetLife();
	void SetImmortal(bool immortal);
	//New
	void New();

	//Phím
	void KeyHandle(Keyboard* key);
	void KeyHandle(PNet::InputState input, float lag);

	//Đổi chuyển động
	void ChangeAnimation(float gameTime);

	//Va chạm
	void OnCollision(Tank *tank_0, float gameTime);
	void OnCollision(Object *object_0, float gameTime);

	//Update
	void Update(float gameTime);

	//Render
	void Render(Viewport* viewport);
};

