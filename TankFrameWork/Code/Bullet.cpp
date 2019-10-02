#include "Bullet.h"



Bullet::Bullet(Sprite* sprite, Sound* sound)
{
	this->Tag = Object::bullet;
	this->sound = sound;
	this->sprite = sprite;
	this->BulletAnimation = new Animation(this->sprite);
	this->positionStart = D3DXVECTOR2(0.0f, 0.0f);
	this->StateBullet = Bullet::Firing;
	this->AllowDraw = false;
}


Bullet::~Bullet()
{
	delete BulletAnimation;
}

//Lấy trạng thái
Bullet::Statebullet Bullet::GetState()
{
	return this->StateBullet;
}

//Set trạng thái
void Bullet::SetState(Statebullet state)
{
	this->StateBullet = state;
}

int Bullet::GetDirection()
{
	return this->DirectionBullet;
}

//New
void Bullet::NewBullet(D3DXVECTOR2 position_tank, int direction_tank, int level)
{
	this->StateBullet = Bullet::Firing;
	this->AllowDraw = true;
	this->DirectionBullet = direction_tank;
	float bulletspeed = BulletSpeed;
	this->TimeBurst = 0.0f;
	if (level >= 1 && level <= 3)
	{
		bulletspeed = BulletSpeed * 1.5f;
	}

	switch (direction_tank)
	{
	//Tank Up
	case 0:
	{
		this->position.x = position_tank.x;
		this->position.y = position_tank.y + 4;
		this->SetVelocity(0.0f, bulletspeed);
		this->SpriteBullet = 294;
		break;
	}
	//Tank Left
	case 1:
	{
		this->position.x = position_tank.x - 4;
		this->position.y = position_tank.y;
		this->SetVelocity(-bulletspeed, 0.0f);
		this->SpriteBullet = 295;
		break;
	}
	//Tank Down
	case 2:
	{
		this->position.x = position_tank.x;
		this->position.y = position_tank.y - 4;
		this->SetVelocity(0.0f, -bulletspeed);
		this->SpriteBullet = 296;
		break;
	}
	//Tank Right
	case 3:
	{
		this->position.x = position_tank.x + 4;
		this->position.y = position_tank.y;
		this->SetVelocity(bulletspeed, 0.0f);
		this->SpriteBullet = 297;
		break;
	}

	default:
		break;
	}
	
	this->SetBound(3, 3);
}

//Đổi chuyển động
void Bullet::ChangeAnimation(float gameTime)
{
	switch (StateBullet)
	{
	case Bullet::Firing:
	{
		this->SetBound(3, 3);
		this->BulletAnimation->SetFrame(this->position, false, 0, SpriteBullet, SpriteBullet);
		break;
	}

	case Bullet::Bursting:
	{
		this->SetBoundZero();
		this->SetVelocity(0.0f, 0.0f);
		this->BulletAnimation->SetFrame(this->position, false, 60, 306, 308);
		//Kết thúc việc bắn
		this->TimeBurst += gameTime;
		if (TimeBurst >= 200)
		{
			this->AllowDraw = false;
		}
		break;
	}

	default:
		break;
	}
}

//Va chạm
bool Bullet::OnCollision(Object *object_0, float gameTime)
{
	if (!this->AllowDraw)
		return false;

	if (Collision::isCollision(bound, object_0->GetBound()))
	{
		return true;
	}

	D3DXVECTOR2 distance = this->Distance(gameTime) - object_0->Distance(gameTime);
	RECT board = Collision::GetBoard(this->GetBound(), distance);
	//Nếu tank trong board
	if (Collision::isCollision(board, object_0->GetBound()))
	{
		//Lấy cạnh va chạm
		D3DXVECTOR2 SideCollision;
		//lấy thời gian va chạm
		float Time = Collision::CollisionAABB(this->GetBound(), object_0->GetBound(), distance, SideCollision);

		//bé hơn 1 thì có va chạm
		if (Time < 1.0f)
		{
			if (SideCollision.x == -1)//chạm bên trái mình
				this->position.x = object_0->GetBound().right;
			else if (SideCollision.x == 1)//chạm bên phải mình
				this->position.x = object_0->GetBound().left;
			else if (SideCollision.y == 1)//chạm bên dưới mình
				this->position.y = object_0->GetBound().top;
			else if (SideCollision.y == -1)//chạm bên trên mình
				this->position.y = object_0->GetBound().bottom;

			return true;
		}
	}
	//không va chạm
	return false;
}

//Update
void Bullet::Update(float gameTime)
{
	//Không cho update
	if (!AllowDraw)
		return;

	ChangeAnimation(gameTime);

	Object::Update(gameTime);
	this->BulletAnimation->Update(gameTime);

	//Kiểm tra ngoài màn  hình
	if (!(position.x > 16 && position.x < 224 && position.y > 8 && position.y < 232))
	{
		this->StateBullet = Bullet::Bursting;
	}
}

//Render
void Bullet::Render(Viewport* viewport)
{
	//Không cho vẽ
	if (!AllowDraw)
		return;
	this->BulletAnimation->Render(viewport);
}