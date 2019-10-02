#pragma once
#include "Object.h"
#include "Animation.h"
#include "Collision.h"
#include "Tank.h"

class Boss : public Object
{
protected:
	Animation * BossAnimation;
	int Team;
public:
	enum Stateboss
	{
		Alive,
		Dead
	};
	Stateboss StateBoss;

	Boss(Sprite* sprite, Sound* sound, int team);
	~Boss();

	int GetTeam();

	//Lấy trạng thái
	Stateboss GetState();
	//Set trạng thái
	void SetState(Stateboss State);

	//New
	void New();

	//Đổi chuyển động
	void ChangeAnimation(float gameTime);

	//Va chạm
	void OnCollision(std::vector <Tank*> &ListTank, float gameTime);

	//Update
	void Update(float gameTime);

	//Render
	void Render(Viewport* viewport);
};

