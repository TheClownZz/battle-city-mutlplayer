#pragma once
#include "Tank.h"

class TankEnemy : public Tank
{
protected:
	float TimeLock, TimeChangeMove, TimeMove;
	bool LockMove;
public:
	TankEnemy(Sprite* sprite, Sound* sound, int _id, int level = 4);
	~TankEnemy();


	void SetLockMove(bool lockmove);

	//Kiểm tra HP
	void CheckHP();

	//New
	void New();

	//Phím
	void AI();

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

