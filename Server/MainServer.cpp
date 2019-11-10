#pragma once
#include <PNet\IncludeMe.h>
#include <iostream>
#include<PNet\Server.h>
#include<PNet\Constants.h>
#include <Windows.h>
#include <TankFrameWork\Code\Game.h>
#include <TankFrameWork\Code\GameDefine.h>

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
		game.CreateObject(numPlayer, SERVER_ID);
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
		input.isShoot = packet.read_bits(bitIndex, 1);
		input.isLeft = packet.read_bits(bitIndex, 1);
		input.isRight = packet.read_bits(bitIndex, 1);
		input.isUp = packet.read_bits(bitIndex, 1);
		input.isDown = packet.read_bits(bitIndex, 1);;
		game.HandleInput(input, clientID, timeSend);
		break;
	}
	default:
		break;
	}
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
int WINAPI WinMain(HINSTANCE Hins, HINSTANCE HIns, LPTSTR a, int c)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	if (Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized." << std::endl;
		int numClient;
		std::cout << "Number player in room:";
		std::cin >> numClient;
		if (numClient > MAX_PlAYER)
			numClient = MAX_PlAYER;
		Server myServer(numClient);
	/*	char *serverIP = new char[20];
		std::cout << "Nhap ip server:";
		std::cin >> serverIP;*/
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
							while (Server::serverPtr->gamePacket.HasPendingPackets())
							{
								Packet p = Server::serverPtr->gamePacket.Retrieve();
								ProcessPacket(p, game);
							}
							startTime = endTime;
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

		}
		else
			cout << "cant create server " << endl;
	}
	Network::Shutdown();
	system("pause");
	return 0;
}