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
	timeSend = 0;
}
Item::Item(Item *item)
{
	CopyItem(item);
	this->ItemAnimation = NULL;
}
Item::~Item()
{
	if (ItemAnimation != NULL)
		delete ItemAnimation;
}

void Item::CopyItem(Item * item)
{
	CopyObject(item);
	this->ItemType = item->ItemType;
	this->SpriteItem = item->SpriteItem;
	this->StateItem = item->StateItem;
	this->TimeApper = item->TimeApper;
	this->timeSend = item->timeSend;
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

	int x, y;
	x = 32 + (rand() % 999) % (200 - 32);
	y = 32 + (rand() % 999) % (200 - 32);
	this->position = D3DXVECTOR2(x, y);

	//Ngẫu nhiên loại Item
	int Num = (rand() % 999) % 3;

	switch (Num)
	{
	case 0:
		this->ItemType = Item::Hat;
		this->SpriteItem = 265;
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

	Packet p(PacketType::PT_Item);
	long time = GetTickCount();
	p.write_bits(true, 1);
	p.write_bits(time, sizeof(long) * 8);
	p.write_bits(ItemType, 2);
	p.write_bits(x, 8);
	p.write_bits(y, 8);

	for (int i = 0; i < Server::serverPtr->totalConnect; i++)
		Server::serverPtr->connections[i].pm.Append(p);
	if (ItemAnimation != NULL)
		this->ItemAnimation->SetFrame(this->position, false, 0, SpriteItem, SpriteItem);

}
//Đổi chuyển động
void Item::ChangeAnimation(float gameTime)
{
	if (StateItem == Item::Appearing)
	{
		this->SetBound(16, 16);
		this->TimeApper += gameTime;
		if (TimeApper / 1000 >= TIME_LIVE_ITEM / 2)
		{
			int TimePause = TimeApper / 150;
			if (TimePause % 2 == 0)
			{
				this->AllowDraw = true;
			}
			else if (TimePause % 2 == 1)
			{
				this->AllowDraw = false;
			}
		}
		//Sau 10s không ăn thì mất
		if (TimeApper / 1000 >= TIME_LIVE_ITEM)
		{
			this->StateItem = Item::None;
			this->ItemType = Item::NoneItem;
			this->SetBoundZero();
			this->AllowDraw = false;
		}
	}
}

void Item::OnCollision(std::vector <Tank*> &ListTank, Map* map)
{
	if (ItemType != Item::NoneItem && StateItem == Item::Appearing)
		for (size_t i = 0; i < ListTank.size(); i++)
		{
			if (ListTank.at(i)->Tag == Object::player &&
				Collision::isCollision1(bound, ListTank.at(i)->GetBound()) &&
				Server::serverPtr != NULL)
			{
				switch (ItemType)
				{
					//Player bất tử
				case Item::Hat:
					ListTank.at(i)->SetImmortal(true);
					break;
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

				// hide item khi bi an
				{
					this->StateItem = Item::None;
					this->ItemType = Item::NoneItem;
					this->SetBoundZero();
					this->AllowDraw = false;
				}

				// send cho client
				{
					Packet p(PacketType::PT_Item);
					long time = GetTickCount();
					p.write_bits(false, 1);
					p.write_bits(time, sizeof(long) * 8);
					p.write_bits(ItemType, 2);
					p.write_bits(ListTank.at(i)->GetIDNetWork(), 3);

					gamePacket.Append(p);
				}
			}

		}
}

//Update
void Item::Update(float gameTime)
{
	if (this->ItemType == Item::NoneItem)
		return;
	if (ItemAnimation != NULL)
		this->ItemAnimation->Update(gameTime);
	this->ChangeAnimation(gameTime);
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

void Item::ShowItem(float lag, int x, int y, Itemtype type)
{
	this->AllowDraw = true;
	this->StateItem = Item::Appearing;
	this->TimeApper = lag;
	this->position = D3DXVECTOR2(x, y);
	ItemType = type;
	switch (ItemType)
	{
	case Item::Hat:
		this->SpriteItem = 265;
		break;
	case Item::TankLife:
		this->SpriteItem = 270;
		break;
	case Item::Gun:
		this->SpriteItem = 271;
		break;
	case Item::NoneItem:
		break;
	default:
		break;
	}
	this->ItemAnimation->SetFrame(this->position, false, 0, SpriteItem, SpriteItem);

}

void Item::EatItem(float lag, int playerID, vector<Tank*> ListTank, Itemtype type)
{
	for (size_t i = 0; i < ListTank.size(); i++)
	{
		if (ListTank.at(i)->GetIDNetWork() == playerID)
		{
			switch (ItemType)
			{
			case Item::Hat:
				ListTank.at(i)->SetImmortal(true);
				ListTank.at(i)->SetTimeImmortal(lag);
				break;
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

			// hide item khi bi an
			{
				this->StateItem = Item::None;
				this->ItemType = Item::NoneItem;
				this->SetBoundZero();
				this->AllowDraw = false;
			}
			break;
		}
	}
}

void Item::CreateItem()
{
	long time = GetTickCount();
	if ((float)(time - timeSend) / 1000.0f >= TIME_UPDATE_ITEM)
	{
		timeSend = time;
		this->New();
	}
}
