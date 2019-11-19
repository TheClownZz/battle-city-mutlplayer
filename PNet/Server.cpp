#include "Server.h"
namespace PNet
{
	Server * Server::serverPtr = NULL;
	Server::Server(uint8_t _numClient)
	{
		serverPtr = this;
		numClient = _numClient;
		totalReady = 0;
		totalConnect = 0;
		totalUdpConnect = 0;
		for (int i = 0; i < MAX_PlAYER; i++)
		{
			udpClientAddrs[i].isConnect = false;
			packetReceiveID[i] = 0;
		}
		packetSendID = 0;
		isStart = false;

	}
	Server::~Server()
	{
		ServerExitAllThread();
		serverPtr = NULL;
	}
	PResult Server::Create(IPEndpoint ip, int backlog)
	{
		if (CreateUDPSocket(ip) != PResult::P_Success)
			return PResult::P_GenericError;
		return CreateTCPSocket(ip, backlog);
	}
	PResult Server::CreateTCPSocket(IPEndpoint ip, int backlog)
	{
		if (socketListen.Create() == PResult::P_Success)
		{
			socketListen.SetIDNetWork(SERVER_ID);
			std::cout << "TPC Socket successfully created." << std::endl;
			if (socketListen.Listen(ip, backlog) == PResult::P_Success)
			{
				std::cout << "TCP Socket successfully listening on port "
					<< ip.GetPort() << std::endl;
				HANDLE senderThead = CreateThread(NULL, NULL,
					(LPTHREAD_START_ROUTINE)PacketSenderThread, NULL, NULL, NULL);
				HANDLE udpReciveThead = CreateThread(NULL, NULL,
					(LPTHREAD_START_ROUTINE)UDP_PacketReciverThread, NULL, NULL, NULL);
				listThread.push_back(senderThead);
				listThread.push_back(udpReciveThead);
				return PResult::P_Success;
			}
			else
			{
				std::cerr << "Failed to listen on port " << ip.GetPort() << std::endl;
				return PResult::P_GenericError;
			}
		}
		else
		{
			std::cerr << "TCP Socket failed to create." << std::endl;
			return PResult::P_GenericError;
		}
	}
	PResult Server::CreateUDPSocket(IPEndpoint ip)
	{
		if (udpConnection.socket.Create(SocketType::UDP) == PResult::P_Success)
		{
			udpConnection.socket.SetIDNetWork(SERVER_ID);
			if (udpConnection.socket.Bind(ip) == PResult::P_Success)
			{
				std::cout << "UDP Socket successfully created." << std::endl;
				return PResult::P_Success;
			}
			else
			{
				std::cerr << "UDP Socket failed to create." << std::endl;
				return PResult::P_GenericError;
			}

		}
		else
		{
			std::cerr << "UDP Socket failed to create." << std::endl;
			return PResult::P_GenericError;
		}
	}
	PResult Server::ListenForNewConnection()
	{
		Socket newConnection;
		if (socketListen.Accept(newConnection) == PResult::P_Success)
		{
			std::cout << "Client Connected! ID:" << (unsigned)totalConnect << std::endl;
			connections[totalConnect].socket = newConnection;
			connections[totalConnect].socket.SetIDNetWork(SERVER_ID);
			connections[totalConnect].isConnect = true;
			// send id
			Packet p(PacketType::PT_SetID);
			p.write_bits(totalConnect, 3);
			connections[totalConnect].pm.Append(p);
			HANDLE clientHandleThread = CreateThread(NULL, NULL,
				(LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(totalConnect), NULL, NULL);
			listThread.push_back(clientHandleThread);
			totalConnect += 1;
			return PResult::P_Success;
		}
		else
		{
			std::cout << "Failed to accept the client's connection." << std::endl;
			return PResult::P_GenericError;
		}
	}
	void Server::ClientHandlerThread(uint8_t ID)
	{
		if (serverPtr == NULL)
		{
			std::cout << "exit ClientHandlerThread ID:" << (unsigned)ID
				<< " because serverPtr is NULL" << std::endl;
			return;
		}
		while (true)
		{
			Packet packet;
			PResult result = serverPtr->connections[ID].socket.Recv(packet);
			long time = GetTickCount();
			if (result != PResult::P_Success)
				break;
			ProcessPacket(ID, packet, time);
		}
		std::cout << "Lost connection to client ID: " << (unsigned)ID << std::endl;
		serverPtr->connections[ID].isConnect = false;
		serverPtr->udpClientAddrs[ID].isConnect = false;
		closesocket(serverPtr->connections[ID].socket.GetHandle());
		return;
	}
	void Server::PacketSenderThread()
	{
		while (true)
		{
			if (serverPtr == NULL)
			{
				std::cout << "exit PacketSenderThread"
					<< " because serverPtr is NULL" << std::endl;
				return;
			}
			//send udp
			while (serverPtr->udpConnection.pm.HasPendingPackets())
			{
				Packet p = serverPtr->udpConnection.pm.Retrieve();
				for (int i = 0; i < serverPtr->totalConnect; i++)
				{
					if (!serverPtr->udpClientAddrs[i].isConnect)
						continue;
					sockaddr_in addr = serverPtr->udpClientAddrs[i].addr;
					if (serverPtr->udpConnection.socket.SendTo
					(p, addr, sizeof(addr)) != PResult::P_Success)
					{
						std::cout << "Failed to send UDP packet to Client id: "
							<< i << std::endl;
					}
				}
				p._buffer.clear(); //Clean up buffer from the packet p
			}

			// SEND TCP
			for (int i = 0; i < serverPtr->totalConnect; i++)
			{
				if (!serverPtr->connections[i].isConnect)
					continue;
				PacketManager butlletPm;
				int num = 0;
				while (serverPtr->connections[i].pm.HasPendingPackets())
				{
					Packet p = serverPtr->connections[i].pm.Retrieve();
					//gui lap tuc neu # PT_Bullet
					if (p.GetPacketType() != PacketType::PT_Bullet && p.GetPacketType() != PacketType::PT_Bullet_Shoot
						&& p.GetPacketType() != PacketType::PT_Bullet_Shoot)
					{
						if (serverPtr->connections[i].socket.Send(p) != PResult::P_Success)
						{
							std::cout << "Failed to send TCP packet to ID: " << i << std::endl;
						}
						p._buffer.clear(); //Clean up buffer from the packet p
					}
					else// gop cac packet shoot lai
					{
						butlletPm.Append(p);
						num++;
					}
				}
				if (num > 0)// merge cac PT_Bullet thanh 1
				{
					Packet packet(PacketType::PT_Bullet);
					packet.write_bits(num, 6);
					while (butlletPm.HasPendingPackets())
					{
						Packet p = butlletPm.Retrieve();
						int bitIndex = NUM_BIT_PACKET;
						uint8_t tankID;
						long timeSend;
						int x, y, direct;
						bool isShoot;
						{// read info
							isShoot = p.read_bits(bitIndex, 1);
							tankID = p.read_bits(bitIndex, 3);
							timeSend = p.read_bits(bitIndex, sizeof(long) * 8);
							if (isShoot)
							{
								x = p.read_bits(bitIndex, 12);
								y = p.read_bits(bitIndex, 12);
								direct = p.read_bits(bitIndex, 2);
							}
						}

						// write info
						{
							packet.write_bits(isShoot, 1);
							packet.write_bits(tankID, 3);
							packet.write_bits(timeSend, sizeof(long) * 8);
							if (isShoot)
							{
								packet.write_bits(x, 12);
								packet.write_bits(y, 12);
								packet.write_bits(direct, 2);
							}
						}
						p._buffer.clear();

					}
					// gui PT_Bullet
					{
						if (serverPtr->connections[i].socket.Send(packet) != PResult::P_Success)
						{
							std::cout << "Failed to send TCP packet to ID: " << i << std::endl;
						}
						packet._buffer.clear(); //Clean up buffer from the packet p
					}
				}
			}
			Sleep(1);
		}
	}
	void Server::UDP_PacketReciverThread()
	{
		while (true)
		{
			if (serverPtr == NULL)
			{
				std::cout << "exit UDP_PacketReciverThread"
					<< " because serverPtr is NULL" << std::endl;
				return;
			}

			sockaddr_in addr;
			int addrlen = sizeof(addr);
			Packet packet;
			PResult result = serverPtr->udpConnection.socket.RecvFrom(packet, (sockaddr*)&addr, &addrlen);
			long time = GetTickCount();
			if (result != PResult::P_Success)
				break;
			ProcessPacket(packet, time, addr);
		}
	}
	bool Server::ProcessPacket(uint8_t ID, Packet & packet, long timeRecive)
	{
		if (serverPtr == NULL)
		{
			std::cout << "exit ProcessPacket"
				<< " because serverPtr is NULL" << std::endl;
			return false;
		}
		switch (packet.GetPacketType())
		{
		case PacketType::PT_Time:
		{
			int bitIndex = NUM_BIT_PACKET;
			long t1;
			t1 = packet.read_bits(bitIndex, sizeof(long) * 8);
			Packet timePacket(PacketType::PT_Time);
			long timeSend = GetTickCount();
			timePacket.write_bits(t1, sizeof(long) * 8);
			timePacket.write_bits(timeRecive, sizeof(long) * 8);
			timePacket.write_bits(timeSend, sizeof(long) * 8);
			std::cout << "send time to client, t2:" << timeRecive << " t3:" << timeSend << std::endl;
			std::cout << "t1:" << t1 << std::endl;
			serverPtr->connections[ID].pm.Append(timePacket);
			break;
		}
		case PacketType::PT_Ready:
		{
			serverPtr->totalReady++;
			if (serverPtr->totalReady == serverPtr->numClient)
			{
				// send start game
				serverPtr->isStart = true;
				Packet p(PacketType::PT_Start);
				p.write_bits(serverPtr->totalConnect, 3);
				serverPtr->gamePacket.Append(p);
			}
			break;
		}
		default:
			return false;
		}

		return true;
	}
	bool Server::ProcessPacket(Packet & packet, long timeRecive, sockaddr_in from)
	{
		if (serverPtr == NULL)
		{
			std::cout << "exit ProcessPacket"
				<< " because serverPtr is NULL" << std::endl;
			return false;
		}
		switch (packet.GetPacketType())
		{
		case PacketType::PT_ConnectUdp:
		{
			int bitIndex = NUM_BIT_PACKET;
			uint8_t _id;
			_id = packet.read_bits(bitIndex, 2);
			std::cout << "PT_ConnectUdp client id:" << (unsigned)_id << std::endl;
			serverPtr->udpClientAddrs[_id].addr = from;
			serverPtr->udpClientAddrs[_id].isConnect = true;
			Packet p(PacketType::PT_ConnectUdp);
			serverPtr->connections[_id].pm.Append(p);
			break;
		}
		case PacketType::PT_Input:
		{
			int bitIndex = NUM_BIT_PACKET;
			uint8_t clientID = packet.read_bits(bitIndex, 2);
			uint16_t packetID = packet.read_bits(bitIndex, sizeof(uint16_t) * 8);
			if (serverPtr->packetReceiveID[clientID] > packetID)
			{
				std::cout << "igone PT_Input id:" << packetID << std::endl;
				break;
			}
			long timeSend = packet.read_bits(bitIndex, sizeof(long) * 8);
			long current = GetTickCount();
			if (current - timeSend >= Max_Ping)
			{
				std::cout << "igone PT_Input id:" << packetID << "ping: " << current - timeSend << std::endl;
				break;
			}
			serverPtr->packetReceiveID[clientID] = packetID;
			Packet p(PacketType::PT_Input);
			p._buffer = packet._buffer;

			serverPtr->gamePacket.Append(p);
			break;

		}
		default:
			return false;
		}
		return true;
	}
	void Server::ServerExitAllThread()
	{
		for (size_t i = 0; i < listThread.size(); i++)
		{
			DWORD exitCode;

			if (GetExitCodeThread(listThread.at(i), &exitCode) != 0)
			{
				ExitThread(exitCode);
				printf("Thread closed");

				if (CloseHandle(listThread.at(i)))
				{
					std::cout << "Handle closed at:" << i << std::endl;
				}
			}
		}
		listThread.clear();
	}
}
