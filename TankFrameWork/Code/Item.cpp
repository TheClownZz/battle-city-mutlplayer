#include<PNet\Server.h>
#include "Item.h"


Item::Item(Sprite* sprite, Sound* sound)
{
	this->Tag = Object::item;
	this->sound = sound;
	this->sprite = sprite;
	this->ItemAnimation = new Animation(this->sprite);
	this->ItemType = Item::NoneItem;
	this->StateItem = Item::None;
	this->AllowDraw = false;
	this->position = D3DXVECTOR2(-20.0f, -20.0f);
}


Item::~Item()
{
	delete ItemAnimation;
}

//Lấy loại Item
Item::Itemtype Item::GetItemType()
{
	return this->ItemType;
}

void Item::SetItemType(Itemtype itemtype)
{
	this->ItemType = itemtype;
}

//Lấy trạng thái
Item::Stateitem Item::GetState()
{
	return this->StateItem;
}
//Set trạng thái
void Item::SetState(Stateitem state)
{
	this->StateItem = state;
}

//New
void Item::New()
{
	if (this->ItemType != Item::NoneItem)
		return;

	this->AllowDraw = true;
	this->StateItem = Item::Appearing;
	this->TimeApper = 0.0f;
	this->TimeStand = 0.0f;
	this->position = D3DXVECTOR2(-20.0f, -20.0f);

	//Ngẫu nhiên loại Item
	int Num = (rand() % 999) % 3;

	switch (Num)
	{
	case 0:
		this->ItemType = Item::Shovel;
		this->SpriteItem = 267;
		break;
	case 1:
		this->ItemType = Item::TankLife;
		this->SpriteItem = 270;
		break;

	case 2:
		this->ItemType = Item::Gun;
		this->SpriteItem = 271;
		break;
		/*case 3:
			this->ItemType = Item::Star;
			this->SpriteItem = 268;
			break;

		case 4:
			this->ItemType = Item::Grenade;
			this->SpriteItem = 269;
			break;
		case 5:
			this->ItemType = Item::Hat;
			this->SpriteItem = 265;
			break;

		case 6:
			this->ItemType = Item::Clock;
			this->SpriteItem = 266;
			break;
	*/
	default:
		break;
	}
}

//Set 
void Item::SetItem(D3DXVECTOR2 Position, int sprite_tank)
{
	if (this->StateItem == Item::Appearing)
	{
		this->position = Position;
		this->SpriteTank = sprite_tank;
	}
}

//Đổi chuyển động
void Item::ChangeAnimation(float gameTime)
{
	//Cho xuất hiện
	this->TimeApper += gameTime;

	int TimePause = TimeApper / 150;
	if (TimePause % 2 == 0)
	{
		this->AllowDraw = true;
	}
	else if (TimePause % 2 == 1)
	{
		this->AllowDraw = false;
	}

	switch (StateItem)
	{
	case Item::Appearing:
	{
		this->SetBoundZero();
		this->ItemAnimation->SetFrame(this->position, false, 0, SpriteTank, SpriteTank);
		break;
	}
	case Item::Standing:
	{
		//Sau 20s không ăn thì mất
		this->TimeStand += gameTime;
		if (TimeStand >= 20000)
		{
			this->StateItem = Item::None;
			this->ItemType = Item::NoneItem;
		}

		this->SetBound(16, 16);
		this->ItemAnimation->SetFrame(this->position, false, 0, SpriteItem, SpriteItem);
		break;
	}
	case Item::None:
	{
		this->SetBoundZero();
		this->AllowDraw = false;
		break;
	}
	default:
		break;
	}
}

void Item::OnCollision(std::vector <Tank*> &ListTank, Map* map)
{
	if (ItemType != Item::NoneItem && StateItem == Item::Standing)
		for (size_t i = 0; i < ListTank.size(); i++)
		{
			if (ListTank.at(i)->Tag == Object::player &&
				Collision::isCollision1(bound, ListTank.at(i)->GetBound()) &&
				Server::serverPtr != NULL)
			{
				switch (ItemType)
				{
					//Player bất tử
					//case Item::Hat:
					//	ListTank.at(i)->SetImmortal(true);
					//	break;

					//Tank Enemy đứng yên 15s
					//case Item::Clock:
					//	for (size_t j = 0; j < ListTank.size(); j++)
					//	{
					//		if (ListTank.at(j)->Tag == Object::enemy)
					//		{
					//			ListTank.at(j)->SetLockMove(true);
					//			if (ListTank.at(j)->GetState() != Statetank::Appearing && ListTank.at(j)->GetState() != Statetank::Dying)
					//				ListTank.at(j)->SetState(Statetank::Standing);
					//		}
					//	}
					//	break;

					//Xây Metal cho boss team
				case Item::Shovel:
					if (ListTank.at(i)->GetTeam() == 0)
					{
						map->isChangMetalWall();
						map->SetWallTeam0(6);
						map->SetWallTeam1(1);
					}
					else if (ListTank.at(i)->GetTeam() == 1)
					{
						map->isChangMetalWall();
						map->SetWallTeam0(1);
						map->SetWallTeam1(6);
					}
					break;

					////Tăng 1 level cho Player
					//case Item::Star:
					//	ListTank.at(i)->SetLevel(ListTank.at(i)->GetLevel() + 1);
					//	break;

					////All Enemy Dying
					//case Item::Grenade:
					//	for (size_t j = 0; j < ListTank.size(); j++)
					//	{
					//		if (ListTank.at(j)->Tag == Object::enemy)
					//		{
					//			//Nếu đang hồi sinh hoặc đang chết thì không chết
					//			if (ListTank.at(j)->GetState() != Statetank::Appearing && 
					//				ListTank.at(j)->GetState() != Statetank::Dying)
					//			ListTank.at(j)->SetState(Statetank::Dying);
					//		}
					//	}
					//	break;

					//Tăng 1 mạng cho Player khi có ít hơn 4 mạng
				case Item::TankLife:
					if (ListTank.at(i)->GetLife() < 4)
						ListTank.at(i)->SetLife(ListTank.at(i)->GetLife() + 1);
					break;

					//Tăng Max Level cho Player
				case Item::Gun:
					ListTank.at(i)->SetLevel(ListTank.at(i)->GetLevel() + 3);
					break;

				default:
					break;
				}

				this->StateItem = Item::None;
				this->ItemType = Item::NoneItem;
			}

		}
}

//Update
void Item::Update(float gameTime)
{
	if (this->ItemType == Item::NoneItem)
		return;

	this->ItemAnimation->Update(gameTime);
}

//Render
void Item::Render(Viewport* viewport)
{
	//Không cho vẽ
	if (!AllowDraw)
		return;

	//Vẽ Tank
	this->ItemAnimation->Render(viewport);
}
