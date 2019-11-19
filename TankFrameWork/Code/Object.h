#pragma once
#include<PNet\Constants.h>
#include "Viewport.h"
#include "Sound.h"
#include "Sprite.h"
#include "Collision.h"
class Object
{
protected:
	Sprite* sprite;
	Sound* sound;
	float Width, Height;
	bool FlipFlag, AllowDraw;
	D3DXVECTOR2 position; //vị trí vẽ
	D3DXVECTOR2 velocity;//Vector vận tốc
	D3DXVECTOR2 positionStart;
	D3DXVECTOR2 transform;
	float angle;
	RECT bound;
	int HP, Damage;
public:
	int id;
	enum tag
	{
		player,
		enemy,//Địch
		boss,//
		bullet,
		item,//item
		wall,//Tường
		metalwall,
		tree,
		ice,
		river,

	}Tag;

	Object();
	Object(Object *obj);
	~Object();
	void UpdatePosition(float gameTime);
	void CopyObject(Object *obj);
	virtual bool GetFlipFlag();
	virtual void SetFlipFlag(bool flag);
	//Khung va chạm
	virtual RECT GetBound();
	virtual void SetBound(float width, float height);
	virtual void SetBound(RECT bound);
	virtual void SetBoundZero();

	//Thông tin frame
	virtual float GetWidth();
	virtual void SetWidth(float width);
	virtual float GetHeight();
	virtual void SetHeight(float height);

	//Vị trí
	virtual D3DXVECTOR2 GetPosition();
	virtual D3DXVECTOR2 GetPositionStart();
	virtual void SetPosition(D3DXVECTOR2 Position);
	virtual void SetPositionStart(D3DXVECTOR2 Position);
	virtual void SetPositionX(float x);//lấy vị trí từ bound sau khi va chạm
	virtual void SetPositionY(float y);

	//Vận tốc
	virtual D3DXVECTOR2 GetVelocity();
	virtual void SetVelocity(D3DXVECTOR2 Velocity);
	virtual void SetVelocityX(float x);
	virtual void SetVelocityY(float y);
	virtual void SetVelocity(float x, float y);

	//Dịch chuyển
	virtual D3DXVECTOR2 GetTransform();
	virtual void SetTransform(D3DXVECTOR2 Transform);
	virtual void SetTransform(float x, float y);

	//Góc
	virtual float GetAngle();
	virtual void SetAngle(float Angle);

	//Tính khoảng cách trong khoảng time
	virtual D3DXVECTOR2 Distance(float time);

	//cho phép vẽ
	virtual void SetAllowDraw(bool allowDraw);
	virtual bool GetAllowDraw();

	//Máu
	virtual void SetHP(int hp);
	virtual int GetHP();
	//Damage
	virtual void SetDamage(int damage);
	virtual int GetDamage();

	virtual void LockAnimation(bool lock, int index);
	virtual void ChangeAnimation();

	virtual bool CheckDie();

	virtual void KeyHandle(Keyboard* key);
	virtual void Update(float gameTime);
	virtual void Render(Viewport* viewport);

	//Va chạm
	virtual void OnCollision(Object *obj, D3DXVECTOR2 distance);

	//Tạo lại Object
	virtual void New();
};

