#pragma once
#include "Object.h"
#include "Bullet.h"
#include "Animation.h"
#include "Collision.h"
#include "GameDefine.h"
#include"SoundTank.h"
#include <vector>
using namespace Define;
using namespace PNet;
class Tank : public Object
{
protected:
	Animation * TankAnimation;
	//Thời gian bị thương và bất tử
	float TimeAppear, TimeBleed, TimeDie, TimeShoot;
	float TimeImmortal;
	float TankSpeed;
	float TimeShootColdown;
	float TimeShootLag;
	bool AllowSetlag;
	//Vị trí người chơi và Level xe tăng
	int Life, Level, Team, idNetWork;
	//bool bất tử
	bool Immortal, Shoot, ReLoad;

	//đạn
	vector <Bullet*> ListBullet;
	//	0: Up		1:Left		2:Down		3:Right	
	int DirectionTank;
	vector<BulletProperties> queue_bulletP;
public:
	int numBulletColision;

public:
	//đạn
	vector <Bullet*> GetListBullet();
	Statetank StateTank;
	Tank(Sprite* sprite, Sound* sound, int _id =-1);
	~Tank();

	//Get/Set một số thông tin
	int GetTeam();
	int GetIDNetWork();
	int GetLevel();
	int GetDirection();

	TankProperties GetProperties();
	void SetProperties(TankProperties p, float lag);
	void SetBulletProperties(TankBulletProperties p, float lag);
	D3DXVECTOR2 GetVelocity(Statetank state);
	void ServerShoot(float gametime, int bulletID);
	void ClientShoot(float gametime, int bulletID);

	void SetTimeImmortal(float time);
	void SetImmortal(bool immortal);
	bool isImmortal();

	void SetLockMove(bool lockmove);
	void SetShoot(bool isShoot, BulletProperties bulletP);
	void SetLevel(int level);
	void SetLife(int life);
	void SetTimeShootLag(float lag);
	int GetLife();

	//Lấy trạng thái
	Statetank GetState();
	//Set trạng thái
	void SetState(Statetank State);

	//New
	virtual void New();

	//Phím
	virtual void KeyHandle(Keyboard* key) {}
	virtual void KeyHandle(PNet::InputState input, float lag) {}

	//Đổi chuyển động
	virtual void ChangeAnimation(float gameTime);

	//Va chạm
	virtual void OnCollision(Tank *tank_0, float gameTime);
	virtual void OnCollision(Object *object_0, float gameTime);

	//Update
	virtual void Update(float gameTime);

	//Render
	virtual void Render(Viewport* viewport);


};

