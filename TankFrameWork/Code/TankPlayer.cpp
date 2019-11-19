#include<PNet\Server.h>
#include "TankPlayer.h"



TankPlayer::TankPlayer(Sprite* sprite, Sound* sound, int team, int _id) : Tank(sprite, sound, _id)
{
	this->velocity = D3DXVECTOR2(0, 0);
	this->EffectAnimation = new Animation(this->sprite);
	this->Tag = Object::player;
	this->Team = team;
	this->AllowDraw = false;
	this->Life = 3;
	this->TimeImmortal = 0.0f;
}


TankPlayer::~TankPlayer()
{
	if (EffectAnimation)
		delete EffectAnimation;
}



//Kiểm tra level
void TankPlayer::SetLevel(int level)
{
	this->Level = level;
	if (this->Level >= 3)
		this->Level = 3;

	//if (this->Level >= 2 && ListBullet.size() == 1)
	//{
	//	Bullet *Bullet_0 = new Bullet(sprite, sound);
	//	this->ListBullet.push_back(Bullet_0);
	//}
}

void TankPlayer::SetLife(int life)
{
	this->Life = life;
}

int TankPlayer::GetLife()
{
	return this->Life;
}

//Bật bất tử
void TankPlayer::SetImmortal(bool immortal)
{
	this->Immortal = immortal;
	if (immortal)
		this->TimeImmortal = 0.0f;
}
//New
void TankPlayer::New()
{
	Tank::New();
	//TankPlayer
	this->TimeBleed = 0.0f;
	this->TimeImmortal = 0;
	this->Immortal = false;
	this->AllowSetlag = true;

	this->TankSpeed = RunSpeed;
	this->Level = 0;

	this->Life--;

	if (this->Life < 0)
	{
		this->Life = 0;
		this->StateTank = Statetank::Dead;
		return;
	}
	else
		this->StateTank = Statetank::Appearing;
	//Bullet
	Bullet *Bullet_0 = new Bullet(sprite, sound);
	this->ListBullet.push_back(Bullet_0);
}

//Phím
void TankPlayer::KeyHandle(Keyboard* key)
{
	//Xuất hiện, đang bị thương và chết không di chuyển được
	if (this->StateTank == Statetank::Appearing || this->StateTank == Statetank::Dying || this->StateTank == Statetank::Dead)
	{
		//this->Shoot = false;
		return;
	}
	if (this->StateTank == Statetank::Bleeing)
	{
		return;
	}

	if (key->IsKeyDown(Dik_LEFT) && key->GIsKeyUp(Dik_RIGHT))
	{
		this->StateTank = Statetank::RunningLeft;

	}
	else if (key->IsKeyDown(Dik_RIGHT) && key->GIsKeyUp(Dik_LEFT))
	{
		this->StateTank = Statetank::RunningRight;

	}
	else if (key->IsKeyDown(Dik_UP) && key->GIsKeyUp(Dik_DOWN))
	{
		this->StateTank = Statetank::RunningUp;

	}
	else if (key->IsKeyDown(Dik_DOWN) && key->GIsKeyUp(Dik_UP))
	{
		this->StateTank = Statetank::RunningDown;

	}
	else
	{

		this->StateTank = Statetank::Standing;
	}
}

void TankPlayer::KeyHandle(PNet::InputState input, float lag)
{
	//Xuất hiện, đang bị thương và chết không di chuyển được
	if (this->StateTank == Statetank::Appearing || this->StateTank == Statetank::Dying || this->StateTank == Statetank::Dead)
	{
		this->Shoot = false;
		return;
	}
	//Cho phép tank bắn
	if (input.isShoot)
	{
		this->Shoot = true;
		SetTimeShootLag(lag);
	}
	else
	{
		this->Shoot = false;
	}
	//Bị thương vẫn bắn được
	if (this->StateTank == Statetank::Bleeing)
	{
		return;
	}

	if (input.isLeft && !input.isRight)
	{
		this->StateTank = Statetank::RunningLeft;

	}
	else if (!input.isLeft && input.isRight)
	{
		this->StateTank = Statetank::RunningRight;

	}
	else if (input.isUp && !input.isDown)
	{
		this->StateTank = Statetank::RunningUp;

	}
	else if (!input.isUp && input.isDown)
	{
		this->StateTank = Statetank::RunningDown;

	}
	else
	{

		this->StateTank = Statetank::Standing;
	}
}

//Đổi chuyển động
void TankPlayer::ChangeAnimation(float gameTime)
{
	//Nếu đang bất tử
	if (Immortal)
	{
		TimeImmortal += gameTime;
		if (TimeImmortal >= Immortal_Time)
		{
			this->Immortal = false;
			if (EffectAnimation)
				this->EffectAnimation->SetPosition(D3DXVECTOR2(0.0f, 0.0f));
			this->TimeImmortal = 0.0f;
			return;
		}
		if (EffectAnimation)
			this->EffectAnimation->SetFrame(this->position, false, 50, 304, 305);
	}

	Tank::ChangeAnimation(gameTime);
}

void TankPlayer::OnCollision(Tank *tank_0, float gameTime)
{

	Object* object_0 = tank_0;
	Tank::OnCollision(object_0, gameTime);

	//Kiểm tra đạn
	for (size_t i = 0; i < ListBullet.size(); i++)
	{
		//Kiểm tra với Tank
		if (ListBullet.at(i)->GetState() == Bullet::Firing && ListBullet.at(i)->OnCollision(tank_0, gameTime))
		{
			//Đồng đội thì bleeding địch thì chết
			if (tank_0->Tag == Object::player && tank_0->GetTeam() == this->Team)
			{
				if (!tank_0->isImmortal() && Server::serverPtr != NULL)
					tank_0->SetState(Statetank::Bleeing);
			}
			else
			{
				if (!tank_0->isImmortal())
				{
					tank_0->numBulletColision++;
					if (Server::serverPtr != NULL)
						tank_0->SetState(Statetank::Dying);
				}
			}

			ListBullet.at(i)->SetState(Bullet::Bursting);
			if (Server::serverPtr != NULL)
			{
				ListBullet.at(i)->SendBurstingBullet(this->idNetWork);
			}

		}
		//Kiểm tra với đạn tank khác
		for (size_t j = 0; j < tank_0->GetListBullet().size(); j++)
		{
			if (ListBullet.at(i)->GetState() == Bullet::Firing && tank_0->GetListBullet().at(j)->GetState() == Bullet::Firing &&
				ListBullet.at(i)->OnCollision(tank_0->GetListBullet().at(j), gameTime))
			{
				ListBullet.at(i)->SetState(Bullet::Bursting);
				tank_0->GetListBullet().at(j)->SetState(Bullet::Bursting);
				if (Server::serverPtr != NULL)
				{
					ListBullet.at(i)->SendBurstingBullet(this->idNetWork);
					tank_0->GetListBullet().at(j)->SendBurstingBullet(tank_0->GetIDNetWork());
				}
			}
		}
	}

}
void TankPlayer::OnCollision(Object *object_0, float gameTime)
{
	Tank::OnCollision(object_0, gameTime);
}
//Update
void TankPlayer::Update(float gameTime)
{
	//Tank
	Tank::Update(gameTime);
	if (EffectAnimation)
		this->EffectAnimation->Update(gameTime);
}
//Render
void TankPlayer::Render(Viewport* viewport)
{
	Tank::Render(viewport);

	//Vẽ hiệu ứng
	if (Immortal)
		this->EffectAnimation->Render(viewport);
}