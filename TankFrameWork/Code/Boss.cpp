#include<PNet\Server.h>
#include "Boss.h"



Boss::Boss(Sprite* sprite, Sound* sound, int team)
{
	this->Tag = Object::boss;
	this->sound = sound;
	this->sprite = sprite;
	this->BossAnimation = new Animation(this->sprite);
	this->AllowDraw = true;
	this->Team = team;
	this->SetVelocity(0.0f, 0.0f);
}


Boss::~Boss()
{
}

int Boss::GetTeam()
{
	return this->Team;
}

//Lấy trạng thái
Boss::Stateboss Boss::GetState()
{
	return this->StateBoss;
}

//Set trạng thái
void Boss::SetState(Stateboss state)
{
	this->StateBoss = state;
}

//New
void Boss::New()
{
	this->StateBoss = Boss::Alive;
	this->HP = 1;
	this->position = this->positionStart;
	this->SetVelocity(0.0f, 0.0f);
}

//Đổi chuyển động
void Boss::ChangeAnimation(float gameTime)
{
	switch (StateBoss)
	{
	case Boss::Alive:
	{
		this->SetBound(15, 15);
		this->BossAnimation->SetFrame(this->position, false, 0, 286, 286);
		break;
	}

	case Boss::Dead:
	{
		this->SetBoundZero();
		this->BossAnimation->SetFrame(this->position, false, 60, 287, 287);
		break;
	}

	default:
		break;
	}
}

//Va chạm
void Boss::OnCollision(std::vector <Tank*> &ListTank, float gameTime)
{
	for (size_t i = 0; i < ListTank.size(); i++)
	{
		ListTank.at(i)->OnCollision(this, gameTime);
		//Kiểm tra với đạn Tank
		for (size_t j = 0; j < ListTank.at(i)->GetListBullet().size(); j++)
		{
			//Kiểm tra với Tank
			if (ListTank.at(i)->GetListBullet().at(j)->GetState() == Bullet::Firing &&
				ListTank.at(i)->GetListBullet().at(j)->OnCollision(this, gameTime))
			{
				if (Server::serverPtr != NULL)
				{
					this->StateBoss = Boss::Dead;
				}
				ListTank.at(i)->GetListBullet().at(j)->SetState(Bullet::Bursting);
			}
		}
	}
}

//Update
void Boss::Update(float gameTime)
{
	ChangeAnimation(gameTime);
	this->BossAnimation->Update(gameTime);
}

//Render
void Boss::Render(Viewport* viewport)
{
	//Không cho vẽ
	if (!AllowDraw)
		return;
	this->BossAnimation->Render(viewport);
}