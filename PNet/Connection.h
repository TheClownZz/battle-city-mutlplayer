#pragma once
#include"Socket.h"
#include"PacketManager.h"
namespace PNet
{
	struct Connection
	{
		bool isConnect;
		Socket socket;
		PacketManager pm; //Packet Manager for outgoing data for this connection
	};
	struct udpAddr
	{
		bool isConnect;
		sockaddr_in addr;
	};
}