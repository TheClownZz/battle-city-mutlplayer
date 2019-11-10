#pragma once
#include <PNet\IncludeMe.h>
#include <iostream>
#include<PNet\Client.h>
#include<PNet\Constants.h>
#include <Windows.h>
#include <TankFrameWork\Code\Game.h>
#include <TankFrameWork\Code\GameDefine.h>
#include <fstream>

#pragma warning(disable: 4996)
#pragma comment(lib, "Ws2_32.lib")
using namespace PNet;
using namespace Define;

void ProcessPacket(Packet & packet, Game &game)
{
	switch (packet.GetPacketType())
	{
	case PacketType::PT_Start:
	{
		int bitIndex = NUM_BIT_PACKET;
		uint8_t numPlayer;
		numPlayer = packet.read_bits(bitIndex, 3);
		int listLevel[4];
		for (int i = 0; i < NUM_ENEMY; i++)
		{
			listLevel[i] = packet.read_bits(bitIndex, 4);
		}

		game.CreateObject(numPlayer, Client::clientPtr->id, listLevel);
		break;
	}
	case PacketType::PT_TankProperties:
	{
		int bitIndex = NUM_BIT_PACKET;
		uint16_t packetID;
		long timeSend;
		packetID = packet.read_bits(bitIndex, sizeof(uint16_t) * 8);
		timeSend = packet.read_bits(bitIndex, sizeof(long) * 8);
		TankProperties listProperties[4 + NUM_ENEMY];
		for (int i = 0; i < Client::clientPtr->numPlayer + NUM_ENEMY; i++)
		{
			uint8_t state, bState;
			int x, y, direct, bx, by, bDirect;
			bool isBlock;
			x = packet.read_bits(bitIndex, 12);
			y = packet.read_bits(bitIndex, 12);
			state = packet.read_bits(bitIndex, 4);
			isBlock = packet.read_bits(bitIndex, 1);
			direct = packet.read_bits(bitIndex, 2);

			listProperties[i].x = (float)x / 10;
			listProperties[i].y = (float)y / 10;
			listProperties[i].state = (Statetank)state;
			listProperties[i].isBlock = isBlock;
			listProperties[i].direct = direct;

			/*bx = packet.read_bits(bitIndex, 12);
			by = packet.read_bits(bitIndex, 12);
			bState = packet.read_bits(bitIndex, 1);
			bDirect = packet.read_bits(bitIndex, 2);

			listProperties[i].bullet.x = (float)bx / 10;
			listProperties[i].bullet.y = (float)by / 10;
			listProperties[i].bullet.state = bState;
			listProperties[i].bullet.direct = bDirect;*/
		}
		game.UpdateTankProperties(timeSend, listProperties);
		break;
	}
	case PacketType::PT_Shoot:
	{
		int bitIndex = NUM_BIT_PACKET;
		int num = packet.read_bits(bitIndex, 6);
		for (int i = 0; i < num; i++)
		{
			uint8_t tankID;
			BulletProperties bulletP;
			int x, y;
			tankID = packet.read_bits(bitIndex, 3);
			bulletP.timeSend = packet.read_bits(bitIndex, sizeof(long) * 8);
			x = packet.read_bits(bitIndex, 12);
			y = packet.read_bits(bitIndex, 12);
			bulletP.x = (float)x / 10;
			bulletP.y = (float)y / 10;
			bulletP.direct = packet.read_bits(bitIndex, 2);
			game.Shoot(tankID, bulletP);
		}
		break;
	}
	case PacketType::PT_Reset:
	{
		int bitIndex = NUM_BIT_PACKET;
		int team = packet.read_bits(bitIndex, 1);
		bool isBossDead = packet.read_bits(bitIndex, 1);
		long timeSend = packet.read_bits(bitIndex, sizeof(long) * 8);
		game.SetEndGame(timeSend, team, isBossDead);
		break;
	}
	case PacketType::PT_UpdateMap:
	{
		int bitIndex = NUM_BIT_PACKET;
		int num = packet.read_bits(bitIndex, 8);
		int stageIndex = packet.read_bits(bitIndex, 6);
		long timeSend = packet.read_bits(bitIndex, sizeof(long) * 8);
		vector<TileInfo> serverMap;
		for (int i = 0; i < num; i++)
		{
			TileInfo ti;
			ti.x = packet.read_bits(bitIndex, 6);
			ti.y = packet.read_bits(bitIndex, 5);
			ti.value = packet.read_bits(bitIndex, 3);
			ti.timeSave = timeSend;
			serverMap.push_back(ti);
		}
		game.UpdateMap(serverMap, timeSend, stageIndex);
		break;
	}
	case PacketType::PT_Item:
	{
		int bitIndex = NUM_BIT_PACKET;
		bool isShow = packet.read_bits(bitIndex, 1);
		if (isShow)
		{
			long timeSend = packet.read_bits(bitIndex, sizeof(long) * 8);
			int type = packet.read_bits(bitIndex, 2);
			int x = packet.read_bits(bitIndex, 8);
			int y = packet.read_bits(bitIndex, 8);
			game.ShowItem(timeSend, x, y, type);
		}
		else
		{
			long timeSend = packet.read_bits(bitIndex, sizeof(long) * 8);
			int type = packet.read_bits(bitIndex, 2);
			int playerID = packet.read_bits(bitIndex, 3);
			game.EatItem(timeSend, playerID, type);
		}
		break;
	}
	default:
		break;
	}
}

string ReadServerIp()
{
	string item_name;
	ifstream nameFileout;
	nameFileout.open("ipconfig.txt");
	std::getline(nameFileout, item_name);
	std::cout << "ReadServerIp:" << item_name << std::endl;
	nameFileout.close();
	return  item_name;
}

int WINAPI WinMain(HINSTANCE Hins, HINSTANCE HIns, LPTSTR a, int c)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);

	if (Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized." << std::endl;
		Client myClient;
		if (myClient.Create() == PResult::P_Success)
		{
			char serverIP[20];
			strcpy(serverIP, ReadServerIp().c_str());
			if (myClient.Connect(IPEndpoint(serverIP, MY_PORT)) == PResult::P_Success)
			{
				while (!myClient.udpConnection.isConnect)
				{
					if (myClient.id != UNDEFINE_ID)
					{
						std::cout << "send id:" << (unsigned)myClient.id << std::endl;
						Packet connectPacket(PacketType::PT_ConnectUdp);
						connectPacket.write_bits(myClient.id, 2);
						myClient.udpConnection.pm.Append(connectPacket);
						/////
					}
					Sleep(500);
				}

				//// xu ly game o day 
				Game game(Hins, Define::WinWidth, Define::WinHeight, (char*)"Tank Client");
				if (!game.InitWD())
				{
					MessageBox(NULL, "Can't load Init Game", "Error", MB_OK);
					return 0;
				}
				//Load Data 
				game.InitDT();
				Packet p(PacketType::PT_Ready);
				myClient.tcpConnection.pm.Append(p);
				std::cout << "Waiting..." << std::endl;

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
							while (Client::clientPtr->gamePacket.HasPendingPackets())
							{
								Packet p = Client::clientPtr->gamePacket.Retrieve();
								ProcessPacket(p, game);
							}
							game.Update(gameTime);
							game.Render();
						}
						else
						{
							Sleep(frame_rate - gameTime);
						}
					}
				}
			}
			else
				std::cout << "Cant connect to server ip:" << serverIP << std::endl;

		}
		else
			std::cout << "Fail to create client" << std::endl;
	}
	else
		cout << "Fail to initialized Winsock api" << endl;
	Network::Shutdown();
	std::system("pause");
	return 0;
}