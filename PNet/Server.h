#pragma once
#include"Socket.h"
#include"IPEndpoint.h"
#include"Connection.h"
#include"PacketManager.h"
#include<iostream>
#include<vector>
namespace PNet
{
	class Server
	{
	public:
		Server(uint8_t numClient);
		~Server();
		PResult Create(IPEndpoint ip, int backlog = 5);
		PResult CreateTCPSocket(IPEndpoint ip, int backlog);
		PResult CreateUDPSocket(IPEndpoint ip);
		PResult ListenForNewConnection();
		void ServerExitAllThread();
	private:
		static void ClientHandlerThread(uint8_t ID);
		static void PacketSenderThread();
		static void UDP_PacketReciverThread();
		static bool ProcessPacket(uint8_t ID, Packet & packet, long timeRecive);
		static bool ProcessPacket(Packet & packet, long timeRecive, sockaddr_in from);
	private:
		uint8_t totalReady;
		uint8_t numClient;
		std::vector<HANDLE> listThread;
		bool isStart;
	public :
		Socket socketListen;
		static  Server * serverPtr;
		uint16_t packetReceiveID[MAX_PlAYER];
		uint16_t packetSendID;
		Connection connections[MAX_PlAYER];
		udpAddr udpClientAddrs[MAX_PlAYER];
		Connection udpConnection;
		PacketManager gamePacket;
		uint8_t totalConnect;
		uint8_t totalUdpConnect;
	};
}