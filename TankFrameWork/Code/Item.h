#pragma once
#include "Object.h"
#include "Animation.h"
#include "Collision.h"
#include "Tank.h"
#include "Map.h"
#include <vector>
#include "GameDefine.h"
using namespace Define;

class Item : public Object
{
protected:
	Animation * ItemAnimation;
	float TimeApper, TimeStand;
	int SpriteItem, SpriteTank;
public:
	enum Itemtype
	{
		Shovel, //Xây metal wall cho boss
		TankLife, //Tăng mạng
		Gun, //Max power
		NoneItem
		//Hat, //Mũ bất tử
		//Clock, //đồng hồ bất động
		//Star, //Ngôi sao tăng power
		//Grenade, //Giết hết địch
	};
	Itemtype ItemType;

	enum Stateitem
	{
		Appearing,//Xuất hiện
		Standing,//Đứng yên
		None //Không làm gì
	};
	Stateitem StateItem;

	Item(Sprite* sprite, Sound* sound);
	~Item();

	//Lấy trạng thái
	Stateitem GetState();
	Itemtype GetItemType();
	//Set trạng thái
	void SetState(Stateitem State);
	void SetItemType(Itemtype itemtype);
	//New
	void New();

	//Set 
	void SetItem(D3DXVECTOR2 Position, int sprite_tank);

	//Đổi chuyển động
	void ChangeAnimation(float gameTime);

	//Va chạm
	void OnCollision(std::vector <Tank*> &ListTank, Map* map);

	//Update
	void Update(float gameTime);

	//Render
	void Render(Viewport* viewport);
};

