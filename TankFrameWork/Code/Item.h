#pragma once
#ifndef ITEM_H
#define ITEM_H
#include "Map.h"
#include "Object.h"
#include "Animation.h"
#include "Collision.h"
#include "Tank.h"
#include <vector>
#include "GameDefine.h"
#include<PNet\PacketManager.h>
using namespace Define;

class Map;
class Item : public Object
{
protected:
	Animation * ItemAnimation;
	float TimeApper;
	int SpriteItem;
	long timeSend;
public:
	PNet::PacketManager gamePacket;
	enum Itemtype
	{
		Hat, //bat tu
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
		None //Không làm gì
	};
	Stateitem StateItem;

	Item(Sprite* sprite, Sound* sound);
	Item(Item *item);
	~Item();
	void CopyItem(Item *item);
	
	//Lấy trạng thái
	Stateitem GetState();
	Itemtype GetItemType();
	//Set trạng thái
	void SetState(Stateitem State);
	void SetItemType(Itemtype itemtype);
	//New
	void New();

	//Đổi chuyển động
	void ChangeAnimation(float gameTime);

	//Va chạm
	void OnCollision(std::vector <Tank*> &ListTank, Map* map);

	//Update
	void Update(float gameTime);

	//Render
	void Render(Viewport* viewport);

	void ShowItem(float lag, int x, int y, Itemtype type);

	void EatItem(float lag, int playerID, vector <Tank*> ListTank, Itemtype type);

	void CreateItem();
};
#endif
