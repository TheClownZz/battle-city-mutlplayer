#pragma once
#include<PNet\PacketManager.h>
#include "Object.h"
#include "Animation.h"
#include "Collision.h"
#include "GameDefine.h"
using namespace Define;

class Bullet : public Object
{
protected:
	Animation * BulletAnimation;
	//Time nổ đạn
	float TimeBurst;
	//Ảnh đạn hiện tại
	int SpriteBullet;
	int DirectionBullet;
public:
	PNet::PacketManager gamePacket;
	enum Statebullet
	{
		Firing,
		Bursting,
	};
	Statebullet StateBullet;
	Bullet(Sprite* sprite, Sound* sound);
	Bullet(Bullet *bullet);
	
	void CopyBullet(Bullet *bullet);
	~Bullet();

	//Lấy trạng thái
	Statebullet GetState();
	//Set trạng thái
	void SetState(Statebullet State);

	int GetDirection();

	//New
	void NewBullet(D3DXVECTOR2 position_tank, int direction_tank, int level);

	//Đổi chuyển động
	void ChangeAnimation(float gameTime);

	//Va chạm
	bool OnCollision(Object *object_0, float gameTime);

	//Update
	void Update(float gameTime);

	//Render
	void Render(Viewport* viewport);

	void SetTimeBurst(float time) { this->TimeBurst = time; }

	void SendBurstingBullet(int networkID);

};

