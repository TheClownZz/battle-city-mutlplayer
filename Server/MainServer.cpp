#pragma once
#include <PNet\IncludeMe.h>
#include <iostream>
#include<PNet\Server.h>
#include<PNet\Constants.h>
#include <Windows.h>
#include <TankFrameWork\Code\Game.h>
#include <TankFrameWork\Code\GameDefine.h>
#include"SaveWorld.h"
#pragma warning(disable : 4996)
#pragma comment(lib, "Ws2_32.lib")
using namespace PNet;
using namespace Define;
#define NUM_SAVE_WORLD 4
vector<InputState> unHandleInput;
vector<SaveWorld*> listSaveWorld;
bool isStart = false;
void ProcessPacket(Packet & packet, Game &game)
{
	switch (packet.GetPacketType())
	{
	case PacketType::PT_Start:
	{
		isStart = true;
		int bitIndex = NUM_BIT_PACKET;
		uint8_t numPlayer;
		numPlayer = packet.read_bits(bitIndex, 3);
		game.CreateObject(numPlayer, SERVER_ID);
		long time = GetTickCount();
		for (int i = 0; i < NUM_SAVE_WORLD; i++)
		{
			SaveWorld *saveWorld = new SaveWorld(&game, time, 0);
			listSaveWorld.push_back(saveWorld);
		}
		break;
	}
	case PacketType::PT_Reset:
	{
		long time = GetTickCount();
		for (size_t i = 0; i < listSaveWorld.size(); i++)
		{
			listSaveWorld.at(i)->UpdateState(&game, time, 0);
		}
		break;
	}
	case PacketType::PT_Input:
	{
		int bitIndex = NUM_BIT_PACKET;
		uint8_t clientID;
		uint16_t packetID;
		InputState input;
		long timeSend;
		clientID = packet.read_bits(bitIndex, 2);
		packetID = packet.read_bits(bitIndex, sizeof(uint16_t) * 8);
		timeSend = packet.read_bits(bitIndex, sizeof(long) * 8);

		input.clientID = clientID;
		input.isShoot = packet.read_bits(bitIndex, 1);
		input.isLeft = packet.read_bits(bitIndex, 1);
		input.isRight = packet.read_bits(bitIndex, 1);
		input.isUp = packet.read_bits(bitIndex, 1);
		input.isDown = packet.read_bits(bitIndex, 1);
		input.timeSend = timeSend;

		unHandleInput.push_back(input);
		//game.HandleInput(input);
		break;
	}
	default:
		break;
	}
}

int UpdateWorldInput(long currentTime, Game &game)
{
	int lastWorldUpdate = NUM_SAVE_WORLD;// save world cuoi cung dc update input
	// Bo input vao game world hien tai
	while (!unHandleInput.empty())
	{
		if (currentTime - unHandleInput.at(unHandleInput.size() - 1).timeSend <= Min_Ping)
		{
			InputState input;
			input = unHandleInput.back();
			unHandleInput.pop_back();
			game.listInput.push_back(input);
		}
		else
			break;
	}
	// Bo input vao save world hien tai
	while (!unHandleInput.empty())
	{
		InputState input;
		input = unHandleInput.back();
		unHandleInput.pop_back();
		for (size_t i = listSaveWorld.size() - 1; i >= 0; i--)
		{
			if (listSaveWorld.at(i)->time - input.timeSend <= Min_Ping)
			{
				listSaveWorld.at(i)->listInput.push_back(input);
				if (i < lastWorldUpdate)
					lastWorldUpdate = i;
				long ping = currentTime - input.timeSend;
				cout << "lastWorldUpdate:" << lastWorldUpdate << " ping:" << ping << endl;
				cout << "ping save world:" << listSaveWorld.at(i)->time - input.timeSend << endl;
				cout << "diferent :" << currentTime - listSaveWorld.at(i)->time << endl;
				cout << listSaveWorld.at(i)->time << " " << input.timeSend << endl;
				break;
			}
		}
	}
	return lastWorldUpdate;

}
void RollBack(long currentTime, Game &game, float gameTime)
{
	//Bo input vao game world theo time
	int lastWorldUpdate = UpdateWorldInput(currentTime, game);

	// save lai cac save world  khong rollback
	for (int i = 0; i < lastWorldUpdate; i++)
	{
		if (i == NUM_SAVE_WORLD - 1)
			break;
		listSaveWorld.at(i)->UpdateState(listSaveWorld.at(i + 1));
		listSaveWorld.at(i)->gameTime = listSaveWorld.at(i + 1)->gameTime;
		listSaveWorld.at(i)->time = listSaveWorld.at(i + 1)->time;
	}
	//rollback
	if (lastWorldUpdate < NUM_SAVE_WORLD)
	{
		cout << "gameTime:" << gameTime << endl;
		cout << "roll back tu save world thu " << lastWorldUpdate << endl;
		for (size_t i = lastWorldUpdate; i < listSaveWorld.size() - 1; i++)
		{
			listSaveWorld.at(i)->UpdateSaveWorld(listSaveWorld.at(i + 1));
			listSaveWorld.at(i + 1)->UpdateState(listSaveWorld.at(i));
		}
		listSaveWorld.at(listSaveWorld.size() - 1)->UpdateSaveWorld();

		game.UpdateState(listSaveWorld.at(listSaveWorld.size() - 1));
	}

	//save lai cac world duoc rollback
	for (int i = lastWorldUpdate; i < listSaveWorld.size() - 1; i++)
	{
		listSaveWorld.at(i)->UpdateState(listSaveWorld.at(i + 1));
		listSaveWorld.at(i)->gameTime = listSaveWorld.at(i + 1)->gameTime;
		listSaveWorld.at(i)->time = listSaveWorld.at(i + 1)->time;
	}
	//save last game
	listSaveWorld.at(listSaveWorld.size() - 1)->UpdateState(&game, currentTime, gameTime);
}
void ListenClient(int numClient)
{
	for (int i = 0; i < numClient; i++)
	{
		Server::serverPtr->ListenForNewConnection();
	}
	std::cout << "all client has connect" << std::endl;
	std::cout << "Start Game..." << std::endl;
}
void SortInputPacket()
{
	std::sort(unHandleInput.begin(), unHandleInput.end());
}
int WINAPI WinMain(HINSTANCE Hins, HINSTANCE HIns, LPTSTR a, int c)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	if (Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized." << std::endl;
		int numClient;
		std::cout << "Number player in room (max:4):";
		std::cin >> numClient;
		if (numClient > MAX_PlAYER)
			numClient = MAX_PlAYER;
		Server myServer(numClient);
		if (myServer.Create(IPEndpoint(NULL, MY_PORT, false)) == PResult::P_Success)
		{
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ListenClient,
				(LPVOID)numClient, NULL, NULL);
			// xu ly game o day 
			{
				Game game(Hins, Define::WinWidth, Define::WinHeight, (char*)"Tank Server", SERVER_ID);
				if (!game.InitWD())
				{
					MessageBox(NULL, "Can't load Init Game", "Error", MB_OK);
					return 0;
				}
				//Load Data 
				game.InitDT();
				MSG Msg;
				ZeroMemory(&Msg, sizeof(MSG));
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				LARGE_INTEGER startTime;
				QueryPerformanceCounter(&startTime);
				LARGE_INTEGER endTime;
				float frame_rate = 1000.0f / Define::FPSGame;
				float gameTime = 0;
				while (Msg.message != WM_QUIT)
				{
					if (PeekMessage(&Msg, NULL, NULL, NULL, PM_REMOVE))
					{
						TranslateMessage(&Msg);
						DispatchMessage(&Msg);
					}
					else
					{
						QueryPerformanceCounter(&endTime);
						gameTime = (endTime.QuadPart - startTime.QuadPart) * (1000.0f / Frequency.QuadPart);
						if (gameTime >= frame_rate)
						{
							startTime = endTime;
							long current = GetTickCount();

							while (Server::serverPtr->gamePacket.HasPendingPackets())
							{
								Packet p = Server::serverPtr->gamePacket.Retrieve();
								ProcessPacket(p, game);
							}

							SortInputPacket();
							if (isStart)
							{
								RollBack(current, game, gameTime);
							}
							game.HandleInput();
							game.Update(gameTime);
							//game.Render();
							game.ClearInput();
						}
						else
						{
							Sleep(frame_rate - gameTime);
						}
					}
				}
			}

		}
		else
			cout << "cant create server " << endl;
	}
	Network::Shutdown();
	system("pause");
	return 0;
}