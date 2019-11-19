#include<PNet\Server.h>
#include<PNet\Client.h>
#include "Map.h"
#include<iostream>
#pragma warning(disable : 4996)
const float TIME_UPDATE_MAP = 0.5f;

//id = 0 không có gì hết
//id = 1 Gạch nguyên
//id = 2 Gạch nửa phải
//id = 3 Gạch nửa dưới
//id = 4 Gạch nửa trái
//id = 5 Gạch nửa trên
//id = 6 Metal
//id = 7 bụi cây
//id = 8 đường băng
//id = 9 sông
//id >= 10 map render
void Map::UpdateTileMap(vector<TileInfo> serverMap, long timeSend, int _stageIndex)
{
	if (_stageIndex != this->stageIndex)
		return;

	//loai bo cac cap nhap o client
	{
		int endPos = -1;
		for (size_t i = 0; i < listTile.size(); i++)
		{
			if (listTile.at(i).timeSave + Max_Ping  <= timeSend)
			{
				int x, y;
				x = listTile.at(i).x;
				y = listTile.at(i).y;
				this->MapMatrix[y][x] = listTile.at(i).value;
				endPos = i + 1;
			}
			else
				break;
		}
		if (endPos > 0)
			listTile.erase(listTile.begin(), listTile.begin() + endPos);
	}
	//cap nhap lai tu server
	for (size_t i = 0; i < serverMap.size(); i++)
	{
		int x = serverMap.at(i).x;
		int y = serverMap.at(i).y;
		int pos = -1;
		for (size_t j = 0; j < listTile.size(); j++)
		{
			if (listTile.at(j).x == x && listTile.at(j).y == y)
			{
				pos = j;
				break;
			}
		}
		if (pos == -1 || listTile.at(pos).timeSave + Max_Ping < timeSend)
		{
			this->MapMatrix[y][x] = serverMap.at(i).value;
			if (pos != -1)
				listTile.erase(listTile.begin() + pos, listTile.begin() + pos + 1);
		}
	}
}

void Map::ServerSendUpdateMap()
{
	long time = GetTickCount();
	if ((float)(time - timeSend) / 1000.0f < TIME_UPDATE_MAP)
		return;

	timeSend = time;
	if (listTile.size() == 0)
		return;
	// sap xep theo thoi gian
	std::sort(listTile.begin(), listTile.end());

	Packet p(PacketType::PT_UpdateMap);
	int size = listTile.size();
	int endPos = -1;
	for (size_t i = 0; i < listTile.size(); i++)
	{
		if (timeSend - listTile.at(i).timeSave >= Min_Ping)
			endPos = i + 1;
		else
			break;
	}
	if (endPos == -1)
		return;
	p.write_bits(endPos, 8);
	p.write_bits(stageIndex, 6);
	p.write_bits(time, sizeof(long) * 8);

	for (int i = 0; i < endPos; i++)
	{
		p.write_bits(listTile.at(i).x, 6);
		p.write_bits(listTile.at(i).y, 5);
		p.write_bits(listTile.at(i).value, 3);
	}

	for (int i = 0; i < Server::serverPtr->totalConnect; i++)
		Server::serverPtr->connections[i].pm.Append(p);
	// xoa tile qua 2 x TIME_UPDATE_MAP
	endPos = -1;
	for (size_t i = 0; i < listTile.size(); i++)
	{
		if (timeSend - listTile.at(i).timeSave >= 2000 * TIME_UPDATE_MAP)
		{
			endPos = i + 1;
		}
		else
			break;
	}
	if (endPos >= 0)
		listTile.erase(listTile.begin(), listTile.begin() + endPos);

}

void Map::SaveTile(int x, int y, int value)
{
	long time = GetTickCount();
	if (Client::clientPtr != NULL)
		time += Client::timeDifference;
	for (size_t i = 0; i < listTile.size(); i++)
	{
		if (x == listTile.at(i).x && y == listTile.at(i).y)
		{
			if (Server::serverPtr != NULL)
			{
				listTile.at(i).x = x;
				listTile.at(i).y = y;
				listTile.at(i).value = value;
				listTile.at(i).timeSave = time;
				return;
			}
			else
				return;
		}
	}
	TileInfo ti;
	ti.x = x;
	ti.y = y;
	ti.value = value;
	ti.timeSave = time;
	listTile.push_back(ti);

}


Map::Map(Sprite* Sprite)
{
	this->sprite = Sprite;
	object_0 = new Object();
	object_1 = new Object();
	timeSend = 0;
	stageIndex = 0;
}
Map::Map(Map *map)
{
	this->listTile = map->listTile;
	this->Width = map->Width;
	this->Height = map->Height;
	this->TileWidth = map->TileWidth;
	this->TileHeight = map->TileHeight;

	this->MapMatrix = new int*[this->Width];
	for (int i = 0; i < this->Width; i++)
		this->MapMatrix[i] = new int[this->Height];

	for (int h = 0; h < this->Height; h++)
		for (int w = 0; w < this->Width; w++)
			this->MapMatrix[h][w] = map->MapMatrix[h][w];

	this->object_0 = new Object(map->object_0);
	this->object_1 = new Object(map->object_1);

	this->sprite = map->sprite;
	this->stageIndex = map->stageIndex;
}

Map::~Map()
{
	delete object_0;
	delete object_1;
}

void Map::CopyMap(Map * map)
{
	this->listTile = map->listTile;
	this->stageIndex = map->stageIndex;
	for (int h = 0; h < this->Height; h++)
		for (int w = 0; w < this->Width; w++)
			this->MapMatrix[h][w] = map->MapMatrix[h][w];
}

void Map::ReadMap(const char *path)
{
	//Load file .tmx
	TiXmlDocument doc(path);
	if (!doc.LoadFile())
	{
		MessageBox(NULL, "Can't load Map xml file", "Error", MB_OK);
		return;
	}

	//Đọc thông số Map
	TiXmlElement* root = doc.RootElement();
	root->QueryFloatAttribute("tilewidth", &this->TileWidth);
	root->QueryFloatAttribute("tileheight", &this->TileHeight);
	root->QueryIntAttribute("width", &this->Width);
	root->QueryIntAttribute("height", &this->Height);

	this->MapMatrix = new int*[Width];
	for (int i = 0; i < Width; i++)
	{
		this->MapMatrix[i] = new int[Height];
	}
	//Đọc thông số Tile Map
	TiXmlElement* child1 = root->FirstChildElement();
	child1->QueryIntAttribute("tilecount", &this->TileCount);
	child1->QueryIntAttribute("columns", &this->TileColumns);
	TiXmlElement* child2 = child1->NextSiblingElement();

	//Lấy chuỗi text đọc map
	TiXmlElement* child2_1 = child2->FirstChildElement();
	const char *dataText = child2_1->GetText();
	char *token = strtok((char *)(dataText), ",");

	int row = this->Height - 1;
	int column = 0;
	while (token != NULL)
	{
		//Lưu giá trị và tạo ObjectMap
		this->MapMatrix[row][column] = atoi(token);
		column++;
		if (column >= this->Width)
		{
			row--;
			column = 0;
		}
		token = strtok(NULL, ",");
	}
}

int** Map::GetMapMatrix()
{
	return this->MapMatrix;
}

//New
void Map::New(int level)
{
	stageIndex++;
	listTile.clear();

	//Đọc file xml
	switch (level)
	{
	case 1:
		this->ReadMap(Define::Map1XML);
		break;

	case 2:
		this->ReadMap(Define::Map2XML);
		break;
	default:
		this->ReadMap(Define::Map1XML);
		break;
	}

	//tạo tile map
	this->sprite->SetCenter(D3DXVECTOR2(TileWidth / 2, TileHeight / 2));
	for (int i = 0; i < TileCount; i++)
	{
		rect_tile[i].left = ((i % this->TileColumns)*this->TileWidth);
		rect_tile[i].top = (i / this->TileColumns)*this->TileHeight;
		rect_tile[i].right = rect_tile[i].left + this->TileWidth;
		rect_tile[i].bottom = rect_tile[i].top + this->TileHeight;
	}

}

//SetMetalWall0
void Map::SetWallTeam0(int id)
{
	this->MapMatrix[1][13] = id;
	this->MapMatrix[2][13] = id;
	this->MapMatrix[3][13] = id;
	this->MapMatrix[3][14] = id;
	this->MapMatrix[3][15] = id;
	this->MapMatrix[1][16] = id;
	this->MapMatrix[2][16] = id;
	this->MapMatrix[3][16] = id;
}
//SetMetalWall1
void Map::SetWallTeam1(int id)
{
	this->MapMatrix[26][13] = id;
	this->MapMatrix[27][13] = id;
	this->MapMatrix[28][13] = id;
	this->MapMatrix[26][14] = id;
	this->MapMatrix[26][15] = id;
	this->MapMatrix[26][16] = id;
	this->MapMatrix[27][16] = id;
	this->MapMatrix[28][16] = id;
}

//Hiển thị lift tank lên UI
void Map::SetUITankLife(std::vector <Tank*> &ListTank, int numPlayer)
{
	//Số 0 là 14 và thứ tự UI của các TankPlayer
	for (int i = 0; i < numPlayer; i++)
	{
		switch (i)
		{
		case 0:
		{
			MapMatrix[2][30] = 14 + ListTank.at(i)->GetLife();
			break;
		}
		case 1:
		{
			MapMatrix[20][30] = 14 + ListTank.at(i)->GetLife();
			break;
		}
		case 2:
		{
			MapMatrix[8][30] = 14 + ListTank.at(i)->GetLife();
			break;
		}
		case 3:
		{
			MapMatrix[26][30] = 14 + ListTank.at(i)->GetLife();
			break;
		}
		default:
			std::cout << "SetUITankLife:" << i << std::endl;
			break;
		}
	}
}

void Map::SetUIIcon(int numplayer)
{
	for (int i = 0; i < numplayer; i++)
	{
		switch (i)
		{
		case 0:
		{
			this->MapMatrix[3][29] = 15;
			this->MapMatrix[3][30] = 12;
			this->MapMatrix[2][29] = 13;
			break;
		}
		case 1:
		{
			this->MapMatrix[21][29] = 16;
			this->MapMatrix[21][30] = 12;
			this->MapMatrix[20][29] = 13;
			break;
		}
		case 2:
		{
			this->MapMatrix[9][29] = 17;
			this->MapMatrix[9][30] = 12;
			this->MapMatrix[8][29] = 13;
			break;
		}
		case 3:
		{
			this->MapMatrix[27][29] = 18;
			this->MapMatrix[27][30] = 12;
			this->MapMatrix[26][29] = 13;
			break;
		}
		default:
			std::cout << "SetUIIcon " << i << std::endl;
			break;
		}
	}
}

void Map::FindObjectMap(Object* object, int derection, int &x0, int &y0, int &x1, int &y1)
{
	if (!object->GetAllowDraw())
		return;

	//Tile Tank
	int w_object = object->GetPosition().x / TileWidth;
	int h_object = object->GetPosition().y / TileHeight;

	//Tránh lỗi nếu vào đã bị lỗi
	if (w_object < 0 || w_object > Width)
		w_object = 2;
	if (h_object < 0 || h_object > Height)
		h_object = 2;

	//chưa tìm thấy đối tượng
	bool isObj = false;

	if (derection == 0) //up
	{
		if (object->Tag == Object::bullet)
			h_object -= 2;

		x0 = w_object;//Tile trên Phải
		x1 = w_object - 1;//Tile trên Trái
		y0 = h_object;
		y1 = h_object;

		for (int h = h_object; h < this->Height; h++)
		{
			int id0 = this->MapMatrix[h][x0];
			int id1 = this->MapMatrix[h][x1];

			//Kiểm tra object va chạm
			if (object->Tag == Object::bullet)
			{
				if ((id0 > 0 && id0 < 7 && !isObj) || (id1 > 0 && id1 < 7 && !isObj))
				{
					//đã tìm thấy
					isObj = true;
					y0 = h;
					y1 = h;
					break;
				}
			}
			else
				if ((id0 != 0 && id0 != 7 && id0 != 8 && !isObj) || (id1 != 0 && id1 != 7 && id1 != 8 && !isObj))
				{
					//đã tìm thấy
					isObj = true;
					y0 = h;
					y1 = h;
					break;
				}

		}
	}
	else if (derection == 1) //left
	{
		if (object->Tag == Object::bullet)
			w_object += 2;

		x0 = w_object;
		x1 = w_object;
		y0 = h_object;//Tile trái trên
		y1 = h_object - 1;//Tile trái dưới

		for (int w = w_object; w >= 0; w--)
		{
			int id0 = this->MapMatrix[y0][w];
			int id1 = this->MapMatrix[y1][w];

			//Kiểm tra object va chạm
			if (object->Tag == Object::bullet)
			{
				if ((id0 > 0 && id0 < 7 && !isObj) || (id1 > 0 && id1 < 7 && !isObj))
				{
					//đã tìm thấy
					isObj = true;
					x0 = w;
					x1 = w;
					break;
				}
			}
			else
				if ((id0 != 0 && id0 != 7 && id0 != 8 && !isObj) || (id1 != 0 && id1 != 7 && id1 != 8 && !isObj))
				{
					//đã tìm thấy
					isObj = true;
					x0 = w;
					x1 = w;
					break;
				}

		}
	}
	else if (derection == 2) //down
	{
		if (object->Tag == Object::bullet)
			h_object += 2;

		x0 = w_object;//Tile trên Phải
		x1 = w_object - 1;//Tile trên Trái
		y0 = h_object;
		y1 = h_object;

		for (int h = h_object; h >= 0; h--)
		{
			int id0 = this->MapMatrix[h][x0];
			int id1 = this->MapMatrix[h][x1];

			//Kiểm tra object va chạm
			if (object->Tag == Object::bullet)
			{
				if ((id0 > 0 && id0 < 7 && !isObj) || (id1 > 0 && id1 < 7 && !isObj))
				{
					//đã tìm thấy
					isObj = true;
					y0 = h;
					y1 = h;
					break;
				}
			}
			else
				if ((id0 != 0 && id0 != 7 && id0 != 8 && !isObj) || (id1 != 0 && id1 != 7 && id1 != 8 && !isObj))
				{
					//đã tìm thấy
					isObj = true;
					y0 = h;
					y1 = h;
					break;
				}

		}
	}
	else if (derection == 3) //right
	{
		if (object->Tag == Object::bullet)
			w_object -= 2;

		x0 = w_object;
		x1 = w_object;
		y0 = h_object;//Tile trái trên
		y1 = h_object - 1;//Tile trái dưới

		for (int w = w_object; w < this->Width; w++)
		{
			int id0 = this->MapMatrix[y0][w];
			int id1 = this->MapMatrix[y1][w];

			//Kiểm tra object va chạm
			if (object->Tag == Object::bullet)
			{
				if ((id0 > 0 && id0 < 7 && !isObj) || (id1 > 0 && id1 < 7 && !isObj))
				{
					//đã tìm thấy
					isObj = true;
					x0 = w;
					x1 = w;
					break;
				}
			}
			else
				if ((id0 != 0 && id0 != 7 && id0 != 8 && !isObj) || (id1 != 0 && id1 != 7 && id1 != 8 && !isObj))
				{
					//đã tìm thấy
					isObj = true;
					x0 = w;
					x1 = w;
					break;
				}

		}
	}

	//Nếu có obecjt
	if (isObj)
	{
		object_0->SetPosition(D3DXVECTOR2((x0 + 0.5)*TileWidth, (y0 + 0.5)*TileHeight));
		object_0->SetBound(TileWidth, TileHeight);

		object_1->SetPosition(D3DXVECTOR2((x1 + 0.5)*TileWidth, (y1 + 0.5)*TileHeight));
		object_1->SetBound(TileWidth, TileHeight);
	}
	else
	{
		object_0->SetBoundZero();
		object_1->SetBoundZero();
	}
}

//Va chạm
void Map::OnCollision(std::vector <Tank*> &ListTank, float gameTime)
{
	for (size_t i = 0; i < ListTank.size(); i++)
	{
		int x0, y0, x1, y1;
		FindObjectMap(ListTank.at(i), ListTank.at(i)->GetDirection(), x0, y0, x1, y1);

		//Kiểm tra object va chạm

		//wall cản
		ListTank.at(i)->OnCollision(object_0, gameTime);
		ListTank.at(i)->OnCollision(object_1, gameTime);

		//Kiểm tra với đạn Tank
		for (size_t j = 0; j < ListTank.at(i)->GetListBullet().size(); j++)
		{
			if (!ListTank.at(i)->GetListBullet().at(j)->GetAllowDraw())
				continue;
			int direction = ListTank.at(i)->GetListBullet().at(j)->GetDirection();
			FindObjectMap(ListTank.at(i)->GetListBullet().at(j), direction, x0, y0, x1, y1);
			int id0 = this->MapMatrix[y0][x0];
			int id1 = this->MapMatrix[y1][x1];
			//Nếu là tường
			if (id0 > 0 && id0 < 7 &&
				ListTank.at(i)->GetListBullet().at(j)->OnCollision(object_0, gameTime))
			{
				//Nếu là tường đơn 
				if (id0 > 1 && id0 < 6)
				{
					this->MapMatrix[y0][x0] = 0;
				}
				//Nếu là tường nguyên
				else if (id0 == 1)
				{
					this->MapMatrix[y0][x0] += direction + 1;
				}
				//Nếu là Metal và Level tăng max
				else if (id0 == 6 && ListTank.at(i)->GetLevel() == 3) //Đạn level 3 mới bể Metal
				{
					this->MapMatrix[y0][x0] = 0;
				}

				ListTank.at(i)->GetListBullet().at(j)->SetState(Bullet::Bursting);
			}
			if (id1 > 0 && id1 < 7 &&
				ListTank.at(i)->GetListBullet().at(j)->OnCollision(object_1, gameTime))
			{
				if (id1 > 1 && id1 < 6)
				{
					this->MapMatrix[y1][x1] = 0;
				}
				else if (id1 == 1)
				{
					this->MapMatrix[y1][x1] += direction + 1;
				}
				else if (id1 == 6 && ListTank.at(i)->GetLevel() == 3)
				{
					this->MapMatrix[y1][x1] = 0;
				}

				ListTank.at(i)->GetListBullet().at(j)->SetState(Bullet::Bursting);
			}

			if (id0 != this->MapMatrix[y0][x0])
			{
				if (Server::serverPtr != NULL)
					SaveTile(x0, y0, this->MapMatrix[y0][x0]);
				else
					SaveTile(x0, y0, id0);
			}

			if (id1 != this->MapMatrix[y1][x1])
			{
				if (Server::serverPtr != NULL)
					SaveTile(x1, y1, this->MapMatrix[y1][x1]);
				else
					SaveTile(x1, y1, id1);
			}

		}//end for j

	}//end for i
}

void Map::OnCollision(Bullet * bullet, int level, float gameTime)
{
	if (!bullet->GetAllowDraw())
		return;
	int x0, y0, x1, y1;
	int direction = bullet->GetDirection();
	FindObjectMap(bullet, direction, x0, y0, x1, y1);
	int id0 = this->MapMatrix[y0][x0];
	int id1 = this->MapMatrix[y1][x1];

	//Nếu là tường
	if (id0 > 0 && id0 < 7 &&
		bullet->OnCollision(object_0, gameTime))
	{
		//Nếu là tường đơn 
		if (id0 > 1 && id0 < 6)
		{
			this->MapMatrix[y0][x0] = 0;
		}
		//Nếu là tường nguyên
		else if (id0 == 1)
		{
			this->MapMatrix[y0][x0] += direction + 1;
		}
		//Nếu là Metal và Level tăng max
		else if (id0 == 6 && level == 3) //Đạn level 3 mới bể Metal
		{
			this->MapMatrix[y0][x0] = 0;
		}

		bullet->SetState(Bullet::Bursting);
	}
	if (id1 > 0 && id1 < 7 &&
		bullet->OnCollision(object_1, gameTime))
	{
		if (id1 > 1 && id1 < 6)
		{
			this->MapMatrix[y1][x1] = 0;
		}
		else if (id1 == 1)
		{
			this->MapMatrix[y1][x1] += direction + 1;
		}
		else if (id1 == 6 && level == 3)
		{
			this->MapMatrix[y1][x1] = 0;
		}

		bullet->SetState(Bullet::Bursting);
	}

	if (id0 != this->MapMatrix[y0][x0])
	{
		if (Server::serverPtr != NULL)
			SaveTile(x0, y0, this->MapMatrix[y0][x0]);
		else
			SaveTile(x0, y0, id0);
	}

	if (id1 != this->MapMatrix[y1][x1])
	{
		if (Server::serverPtr != NULL)
			SaveTile(x1, y1, this->MapMatrix[y1][x1]);
		else
			SaveTile(x1, y1, id1);
	}
}

void Map::OnCollision(Tank * tank, float gameTime)
{
	int x0, y0, x1, y1;
	FindObjectMap(tank, tank->GetDirection(), x0, y0, x1, y1);
	tank->OnCollision(object_0, gameTime);
	tank->OnCollision(object_1, gameTime);
}

//Update
void Map::Update(float gameTime)
{
	//chuyển đổi ảnh river
	this->TimeChangeRiver += gameTime;
	int Time = TimeChangeRiver / 500;
	if (Time % 2 == 0)
	{
		this->ChangeRiver = true;
	}
	else if (Time % 2 == 1)
	{
		this->ChangeRiver = false;
	}

	//Thời gian MetalWall

	if (ChangMetalWall)
	{
		this->TimeChangeMetalWall += gameTime;
		if (this->TimeChangeMetalWall >= 15000)
		{
			this->TimeChangeMetalWall = 0.0f;
			this->ChangMetalWall = false;
			SetWallTeam0(1);
			SetWallTeam1(1);
		}
	}
}

//Render
void Map::Render(Viewport* viewport)
{
	for (int h = 0; h < this->Height; h++)
		for (int w = 0; w < this->Width; w++)
		{
			int id = this->MapMatrix[h][w];
			if (id != 0)
			{
				if (id == 9 && ChangeRiver)
				{
					id++;
				}
				D3DXVECTOR2 position((w + 0.5) * this->TileWidth, (h + 0.5) * this->TileHeight);
				this->sprite->SetRect(rect_tile[id - 1]);
				this->sprite->SetPosition(position);
				this->sprite->Render(viewport);
			}
		}
}

//Render
void Map::RenderTree(Viewport* viewport)
{
	for (int h = 0; h < this->Height; h++)
		for (int w = 0; w < this->Width; w++)
		{
			int id = this->MapMatrix[h][w];
			if (id == 7)
			{
				D3DXVECTOR2 position((w + 0.5) * this->TileWidth, (h + 0.5) * this->TileHeight);
				this->sprite->SetRect(rect_tile[id - 1]);
				this->sprite->SetPosition(position);
				this->sprite->Render(viewport);
			}
		}
}

