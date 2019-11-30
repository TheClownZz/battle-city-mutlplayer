#include<PNet\Server.h>
#include<PNet\Client.h>
#include<PNet\Packet.h>
#include "Game.h"
const int SERVER_SEND_RATE = 25;
const int CLIENT_SEND_RATE = 40;

Game::Game(HINSTANCE Hins, int width, int height, char* name, uint8_t id)
{
	this->Hinsantance = Hins;
	this->Width = width;
	this->Height = height;
	this->Hwnd = NULL;
	this->localID = id;
	strcpy_s(this->WindowName, name);
	timeSend = 0;
}

Game::~Game()
{
	delete graphic;
	keyboard->KillKeyboard();
	delete keyboard;
	delete SceneManager;
}

//Hàm xử lý cửa sổ
LRESULT CALLBACK Game::WindowProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (Message == WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	return DefWindowProc(Hwnd, Message, wParam, lParam);
}

//Tạo cửa sổ Game
bool Game::InitWD()
{
	WNDCLASSEX Wndclass;
	Wndclass.cbSize = sizeof(WNDCLASSEX);
	Wndclass.cbClsExtra = 0;
	Wndclass.cbWndExtra = 0;
	Wndclass.hIconSm = 0;
	Wndclass.hIcon = 0;
	Wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	Wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	Wndclass.hInstance = this->Hinsantance;
	Wndclass.lpfnWndProc = (WNDPROC)WindowProc;
	Wndclass.lpszClassName = "Tank";
	Wndclass.lpszMenuName = NULL;
	Wndclass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&Wndclass))
	{
		return false;
	}
	int x = (GetSystemMetrics(SM_CXSCREEN) - this->Width) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - this->Height) / 2;


	this->Hwnd = CreateWindow(
		"Tank",
		this->WindowName,
		WS_OVERLAPPEDWINDOW,
		x,
		y,
		this->Width,
		this->Height,
		NULL, NULL,
		this->Hinsantance,
		NULL);
	G_hWnd = this->Hwnd;
	if (!this->Hwnd)
		return false;
	ShowWindow(this->Hwnd, SW_NORMAL);
	UpdateWindow(this->Hwnd);

	//Tạo màn hình vẽ
	graphic = new Graphic(this->Hwnd, GameWidth, GameHeight);
	if (!graphic->InitD3D())
	{
		return false;
	}
	return true;
}

//Load Data Game
void Game::InitDT()
{
	keyboard = new Keyboard(Hwnd);
	keyboard->Init();

	SceneManager = new Scene();
	SceneManager->InitDT(graphic);

}

// Cập nhật game
void Game::Update(float gameTime)
{
	SceneManager->Update(gameTime, keyboard);

	if (localID == SERVER_ID)
		ServerSendProperties();
	else if (localID != UNDEFINE_ID)
		ClientSendInput();
}

void Game::Render()
{
	//Bắt đầu vẽ xóa còn màu đen
	graphic->Begin();
	//Vẽ
	SceneManager->Render();
	//Đóng lại thể hiển ra màn hình
	graphic->End();
}

void Game::CreateObject(uint8_t numPlayer, uint8_t id, int listLevelEnemy[])
{
	localID = id;
	if (localID == SERVER_ID)
		SceneManager->GetObjectManager()->ServerCreateObject(numPlayer);
	else
		SceneManager->GetObjectManager()->ClientCreateObject(numPlayer, listLevelEnemy);
}

void Game::ClientSendInput()
{
	long time = GetTickCount();
	if (time - timeSend < 1000.0f / CLIENT_SEND_RATE)
		return;
	keyboard->GetStage();
	timeSend = time;
	InputState input;
	input.isShoot = keyboard->IsKeyDown(Dik_SHOOT);
	input.isLeft = keyboard->IsKeyDown(Dik_LEFT);
	input.isRight = keyboard->IsKeyDown(Dik_RIGHT);
	input.isUp = keyboard->IsKeyDown(Dik_UP);
	input.isDown = keyboard->IsKeyDown(Dik_DOWN);

	Packet p(PNet::PacketType::PT_Input);
	uint16_t packetID = Client::clientPtr->packetSendID++;
	p.write_bits(localID, 2);
	p.write_bits(packetID, sizeof(uint16_t) * 8);
	p.write_bits(time + Client::timeDifference, sizeof(long) * 8);
	p.write_bits(input.isShoot, 1);
	p.write_bits(input.isLeft, 1);
	p.write_bits(input.isRight, 1);
	p.write_bits(input.isUp, 1);
	p.write_bits(input.isDown, 1);

	Client::clientPtr->udpConnection.pm.Append(p);
}

void Game::ServerSendProperties()
{
	long time = GetTickCount();
	if (time - timeSend < 1000.0f / SERVER_SEND_RATE)
		return;
	timeSend = time;
	size_t numTank = SceneManager->GetObjectManager()->GetListTank().size();
	if (numTank <= 0)
		return;
	Packet p(PacketType::PT_TankProperties);
	uint16_t packetID = Server::serverPtr->packetSendID++;
	p.write_bits(packetID, sizeof(uint16_t) * 8);
	p.write_bits(time, sizeof(long) * 8);
	for (size_t i = 0; i < numTank; i++)
	{
		TankProperties properties = SceneManager->GetObjectManager()->GetListTank().at(i)->GetProperties();
		int x, y;
		x = properties.x * 10;
		y = properties.y * 10;

		p.write_bits(x, 12);
		p.write_bits(y, 12);
		p.write_bits(properties.state, 4);
		p.write_bits(properties.direct, 2);
		p.write_bits(properties.dx + 1, 2);
		p.write_bits(properties.dy + 1, 2);

	}
	Server::serverPtr->udpConnection.pm.Append(p);
}

void Game::HandleInput()
{
	if (localID == UNDEFINE_ID || SceneManager->GetObjectManager()->GetListTank().size() <= 0)
	{
		return;
	}
	long current = GetTickCount();
	for (size_t i = 0; i < listInput.size(); i++)
	{
		float lag = (float)(current - listInput.at(i).timeSend);
		if (lag < 0 || lag  >  2 * Max_Ping)
			continue;
		Tank *tank = SceneManager->GetObjectManager()->GetListTank().at(listInput.at(i).clientID);
		tank->KeyHandle(listInput.at(i), lag);
	}
}

void Game::HandleInput(InputState input)
{
	if (localID == UNDEFINE_ID || SceneManager->GetObjectManager()->GetListTank().size() <= 0)
		return;
	long current = GetTickCount();
	float lag = (float)(current - input.timeSend);
	if (lag < 0 || lag > 2 * Max_Ping)
		return;
	Tank *tank = SceneManager->GetObjectManager()->GetListTank().at(input.clientID);
	tank->KeyHandle(input, lag);
}

void Game::UpdateTankProperties(long timeServerSend, TankProperties listProperties[])
{
	if (localID == UNDEFINE_ID || SceneManager->GetObjectManager()->GetListTank().size() <= 0)
		return;
	long current = GetTickCount();
	float lag = ((float)(current + Client::timeDifference - timeServerSend));
	size_t numTank = SceneManager->GetObjectManager()->GetListTank().size();
	for (size_t i = 0; i < numTank; i++)
	{
		Tank *tank = SceneManager->GetObjectManager()->GetListTank().at(i);
		tank->SetProperties(listProperties[i], lag);
	}
}

void Game::Shoot(int tankID, BulletProperties bulletP)
{
	if (localID == UNDEFINE_ID || SceneManager->GetObjectManager()->GetListTank().size() <= 0)
		return;
	Tank *tank = SceneManager->GetObjectManager()->GetListTank().at(tankID);
	tank->SetShoot(true, bulletP);
}

void Game::BurstingBullet(int tankID, long timeServerSend)
{
	if (localID == UNDEFINE_ID || SceneManager->GetObjectManager()->GetListTank().size() <= 0)
		return;
	Tank *tank = SceneManager->GetObjectManager()->GetListTank().at(tankID);
	tank->SetBurstingBullet(timeServerSend);
}

void Game::SetEndGame(long timeServerSend, int team, bool isBossDead)
{
	long current = GetTickCount();
	float lag = ((float)(current + Client::timeDifference - timeServerSend));
	SceneManager->timedelay += lag;
	SceneManager->GetObjectManager()->SetClientEnd(team);
	if (isBossDead)
		SceneManager->GetObjectManager()->GetListBoss().at(team)->SetState(Boss::Dead);
}

void Game::UpdateMap(vector<TileInfo> serverMap, long timeServerSend, int stageIndex)
{
	SceneManager->GetObjectManager()->GetMap()->UpdateTileMap(serverMap, timeServerSend, stageIndex);
}

void Game::ShowItem(long timeServerSend, int x, int y, int type)
{
	vector <Item*> ListItem = SceneManager->GetObjectManager()->GetListItem();
	if (ListItem.size() <= 0)
		return;
	long current = GetTickCount();
	float lag = ((float)(current + Client::timeDifference - timeServerSend));
	ListItem.at(0)->ShowItem(lag, x, y, (Item::Itemtype)type);
}

void Game::EatItem(long timeServerSend, int playerID, int type)
{
	vector <Item*> ListItem = SceneManager->GetObjectManager()->GetListItem();

	if (ListItem.size() <= 0)
		return;
	long current = GetTickCount();
	float lag = ((float)(current + Client::timeDifference - timeServerSend));
	ListItem.at(0)->EatItem(lag, playerID,
		SceneManager->GetObjectManager()->GetListTank(), (Item::Itemtype)type);

}

void Game::UpdateState(SaveWorld *saveWorld)
{
	ObjectManager *gameObjects = GetScene()->GetObjectManager();

	gameObjects->GetMap()->CopyMap(saveWorld->map);

	for (size_t i = 0; i < gameObjects->GetListTank().size(); i++)
	{
		gameObjects->GetListTank().at(i)->CopyTank(saveWorld->listTank.at(i));
	}

	for (size_t i = 0; i < gameObjects->GetListItem().size(); i++)
	{
		gameObjects->GetListItem().at(i)->CopyItem(saveWorld->listItem.at(i));
	}

	for (size_t i = 0; i < gameObjects->GetListBoss().size(); i++)
	{
		gameObjects->GetListBoss().at(i)->CopyBoss(saveWorld->listBoss.at(i));
	}
}


