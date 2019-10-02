#pragma once
#include <PNet\IncludeMe.h>
#include <iostream>
#include"PNet\Server.h"
#include"PNet\Constants.h"
#include <Windows.h>
#include "TankFrameWork\Code\Game.h"
#include "TankFrameWork\Code\GameDefine.h"

#pragma comment(lib, "Ws2_32.lib")
using namespace PNet;
//using namespace Define;
int main()
{
	if (Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized." << std::endl;
		int numClient;
		std::cout << "Nhap so luong client:";
		std::cin >> numClient;
		if (numClient > MAX_PlAYER)
			numClient = MAX_PlAYER;
		Server myServer;
		if (myServer.Create(IPEndpoint("0.0.0.0", MY_PORT)) == PResult::P_Success)
		{
			for (int i = 0; i < numClient; i++)
				myServer.ListenForNewConnection();
			std::cout << "all client has connect" << std::endl;
			//// xu ly game o day 
			while (true)
			{
				Sleep(5);
			}
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;
}