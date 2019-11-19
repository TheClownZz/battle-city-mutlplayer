#include<PNet/Server.h>
#include "SaveWorld.h"


SaveWorld::SaveWorld(Game *game, long _time, float _gameTime)
{
	this->listInput = game->listInput;
	this->time = _time;
	this->gameTime = _gameTime;

	ObjectManager *gameObjects = game->GetScene()->GetObjectManager();

	this->map = new Map(gameObjects->GetMap());

	for (size_t i = 0; i < gameObjects->GetListTank().size(); i++)
	{
		Tank *tank = new Tank(gameObjects->GetListTank().at(i));
		listTank.push_back(tank);
		tank->map = this->map;
	}

	for (size_t i = 0; i < gameObjects->GetListItem().size(); i++)
	{
		Item *item = new Item(gameObjects->GetListItem().at(i));
		listItem.push_back(item);
	}

	for (size_t i = 0; i < gameObjects->GetListBoss().size(); i++)
	{
		Boss *boss = new Boss(gameObjects->GetListBoss().at(i));
		listBoss.push_back(boss);
	}


}


SaveWorld::~SaveWorld()
{
	// delete map
	delete map;

	//delete tank
	for (size_t i = 0; i < listTank.size(); i++)
	{
		Tank* tank_0 = listTank.at(i);
		delete tank_0;
	}
	//delete boss
	for (size_t i = 0; i < listBoss.size(); i++)
	{
		Boss* boss_0 = listBoss.at(i);
		delete boss_0;
	}
	//delete item
	for (size_t i = 0; i < listItem.size(); i++)
	{
		Item* item = listItem.at(i);
		delete item;
	}
}

void SaveWorld::SaveWorldHandleInput()
{
	if (listTank.size() <= 0)
	{
		return;
	}
	for (size_t i = 0; i < listInput.size(); i++)
	{
		float lag = (float)(time - listInput.at(i).timeSend);
		if (lag < 0 || lag  >  Max_Ping)
			continue;
		Tank *tank = listTank.at(listInput.at(i).clientID);
		tank->KeyHandle(listInput.at(i), lag);
	}
}

void SaveWorld::UpdateSaveWorld(SaveWorld *nextWorld)
{
	SaveWorldHandleInput();
	//Kiểm tra va chạm Tank
	for (size_t i = 0; i < listTank.size(); i++)
	{
		listTank.at(i)->ChangeAnimation(gameTime);

		//Kiểm tra va chạm với Tank khác
		for (size_t j = 0; j < listTank.size(); j++)
		{
			if (i != j)
			{
				//Va chạm với Tank
				listTank.at(i)->OnCollision(listTank.at(j), gameTime);
			}
		}
	}

	//Kiểm tra va chạm Item với TankPlayer và Update
	for (size_t i = 0; i < listItem.size(); i++)
	{
		listItem.at(i)->OnCollision(listTank, map);
		//Update Item
		listItem.at(i)->Update(gameTime);

	}

	//Kiểm tra va chạm Boss với Tank và Update
	for (size_t i = 0; i < listBoss.size(); i++)
	{
		listBoss.at(i)->OnCollision(listTank, gameTime);
		//Update Boss
		listBoss.at(i)->Update(gameTime);
	}

	//Kiểm tra va chạm MapObject với Tank
	map->OnCollision(listTank, gameTime);

	// update tank
	for (size_t i = 0; i < listTank.size(); i++)
	{
		listTank.at(i)->Update(gameTime);
	}

	//send shoot packet
	for (size_t i = 0; i < listTank.size(); i++)
	{
		for (size_t j = 0; j < listTank.at(i)->GetListBullet().size(); j++)
		{
			Bullet *b = listTank.at(i)->GetListBullet().at(j);
			while (b->gamePacket.HasPendingPackets())
			{
				Packet p = b->gamePacket.Retrieve();
				if (p.GetPacketType() == PT_Bullet_Shoot)
				{
					if (!nextWorld || !nextWorld->listTank.at(i)->GetShootState())
					{
						int bitIndex = NUM_BIT_PACKET;
						bool isShoot = p.read_bits(bitIndex, 1);
						uint8_t tankID = p.read_bits(bitIndex, 3);
						long timeSend = p.read_bits(bitIndex, sizeof(long) * 8);
						int x = p.read_bits(bitIndex, 12);
						int y = p.read_bits(bitIndex, 12);
						int direct = p.read_bits(bitIndex, 2);

						timeSend = this->time;

						Packet packet(PacketType::PT_Bullet_Shoot);
						packet.write_bits(true, 1);
						packet.write_bits(tankID, 3);
						packet.write_bits(timeSend, sizeof(long) * 8);
						packet.write_bits(x, 12);
						packet.write_bits(y, 12);
						packet.write_bits(direct, 2);

						for (int k = 0; k < Server::serverPtr->totalConnect; k++)
						{
							Server::serverPtr->connections[k].pm.Append(packet);
						}
					}
				}
				p._buffer.clear();
			}
		}
	}

}

void SaveWorld::UpdateState(Game *game, long _time, float _gameTime)
{
	this->time = time;
	this->gameTime = _gameTime;

	this->listInput = game->listInput;
	ObjectManager *gameObjects = game->GetScene()->GetObjectManager();

	this->map->CopyMap(gameObjects->GetMap());

	for (size_t i = 0; i < listTank.size(); i++)
	{
		listTank.at(i)->CopyTank(gameObjects->GetListTank().at(i));
	}

	for (size_t i = 0; i < listItem.size(); i++)
	{
		listItem.at(i)->CopyItem(gameObjects->GetListItem().at(i));
	}

	for (size_t i = 0; i < listBoss.size(); i++)
	{
		listBoss.at(i)->CopyBoss(gameObjects->GetListBoss().at(i));
	}
}

void SaveWorld::UpdateState(SaveWorld *world)
{
	this->map->CopyMap(world->map);

	for (size_t i = 0; i < listTank.size(); i++)
	{
		listTank.at(i)->CopyTank(world->listTank.at(i));
	}

	for (size_t i = 0; i < listItem.size(); i++)
	{
		listItem.at(i)->CopyItem(world->listItem.at(i));
	}

	for (size_t i = 0; i < listBoss.size(); i++)
	{
		listBoss.at(i)->CopyBoss(world->listBoss.at(i));
	}
}
