#pragma once
#include"Socket.h"
#include"IPEndpoint.h"
#include"Connection.h"
#include"PacketManager.h"
#include<iostream>
#include<mutex>
#include<vector>
namespace PNet
{
	class Client {
	public:
		Client();
		~Client();
		PResult Create();
		PResult Connect(IPEndpoint ip);
		void ClientExitAllThread();
	private:
		static void TCP_PacketReciverThread();
		static void UDP_PacketReciverThread();
		static void PacketSenderThread();
		static bool ProcessPacket(Packet & packet, long timeRecive, Connection &connection);
	private:
		IPEndpoint serverIp;
		std::vector<HANDLE> listThread;
	public:
		uint8_t numPlayer;
		uint16_t packetReceiveID;
		uint16_t packetSendID;
		Connection udpConnection;
		Connection tcpConnection;
		PacketManager gamePacket;
		uint8_t id;
		static Client *clientPtr;
		static long timeDifference;
	};
}

