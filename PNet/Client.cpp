#include "Client.h"
namespace PNet
{
	long Client::timeDifference = 0;
	Client * Client::clientPtr = NULL;
	Client::Client()
	{
		clientPtr = this;
		numPlayer = 0;
		id = UNDEFINE_ID;
		timeDifference = 0;
		udpConnection.isConnect = false;
		packetSendID = 0;
		packetReceiveID = 0;
		//HANDLE timeThread= CreateThread(NULL, NULL, 
		//	(LPTHREAD_START_ROUTINE)ThreadHandleTime,NULL, NULL, NULL); 
	}

	Client::~Client()
	{
		ClientExitAllThread();
		clientPtr = NULL;
	}

	PResult Client::Create()
	{
		if(udpConnection.socket.Create(SocketType::UDP)== PResult::P_Success)
			std::cout << "UDP Socket successfully created." << std::endl;
		else
			return PResult::P_GenericError;

		if (tcpConnection.socket.Create() == PResult::P_Success)
		{
			std::cout << "TCP Socket successfully created." << std::endl;
			return PResult::P_Success;
		}
		return PResult::P_GenericError;
	}
	PResult Client::Connect(IPEndpoint ip)
	{
		serverIp = ip;
		if (tcpConnection.socket.Connect(ip) == PResult::P_Success)
		{
			std::cout << "Successfully connected to server!" << std::endl;
			tcpConnection.isConnect = true;
			HANDLE tpcReciveThead, udpReciveThead, senderThead;
			tpcReciveThead=CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)TCP_PacketReciverThread, NULL, NULL, NULL);
			udpReciveThead =CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)UDP_PacketReciverThread, NULL, NULL, NULL);
			senderThead = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PacketSenderThread, NULL, NULL, NULL);
			
			listThread.push_back(tpcReciveThead);
			listThread.push_back(udpReciveThead);
			listThread.push_back(senderThead);

			Packet timePacket(PacketType::PT_Time);
			uint32_t t1 = GetTickCount();
			timePacket.write_bits(t1, sizeof(long) * 8);
			int bitIndex = NUM_BIT_PACKET;
			uint32_t t2 = timePacket.read_bits(bitIndex, sizeof(long) * 8);
			tcpConnection.pm.Append(timePacket);
			return PResult::P_Success;
		}
		std::cout << "fail to connect to server!" << std::endl;
		return PResult::P_GenericError;
	}
	void Client::ClientExitAllThread()
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
	void Client::TCP_PacketReciverThread()
	{
		while (true)
		{
			if (clientPtr == NULL)
			{
				std::cout << "exit TCP_PacketReciverThread  because clientPtr is NULL"
					<< std::endl;
				return;
			}

			Packet packet;
			PResult result = clientPtr->tcpConnection.socket.Recv(packet);
			long time = GetTickCount();
			if (result != PResult::P_Success)
				break;
			ProcessPacket(packet, time, clientPtr->tcpConnection);
			Sleep(5);
		}
		std::cout << "Lost connection to Server: " << std::endl;
		clientPtr->tcpConnection.isConnect = false;
		closesocket(clientPtr->tcpConnection.socket.GetHandle());
		return;
	}
	void Client::UDP_PacketReciverThread()
	{
		while (true)
		{
			if (clientPtr == NULL)
			{
				std::cout << "exit UDP_PacketReciverThread  because clientPtr is NULL"
					<< std::endl;
				return;
			}

			if (clientPtr->udpConnection.isConnect)
			{
				sockaddr_in addr;
				int addrlen = sizeof(addr);
				Packet packet;
				PResult result;
				if (clientPtr->udpConnection.socket.GetHandle() != INVALID_SOCKET)
					result = clientPtr->udpConnection.socket.RecvFrom
					(packet, (sockaddr*)&addr, &addrlen);
				else
				{
					std::cout << "INVALID_SOCKET" << std::endl;
					break;
				}
				long time = GetTickCount();
				if (result != PResult::P_Success)
				{
					std::cout << "fail to recive udp packet:" << WSAGetLastError() << std::endl;
				}
				ProcessPacket(packet, time, clientPtr->udpConnection);
			}
			Sleep(5);
		}
	}
	void Client::PacketSenderThread()
	{
		while (true)
		{
			if (clientPtr == NULL)
			{
				std::cout << "exit PacketSenderThread  because clientPtr is NULL"
					<< std::endl;
				return;
			}

			if (!clientPtr->tcpConnection.isConnect)
				continue;
			//// send udp packet
			while (clientPtr->udpConnection.pm.HasPendingPackets())
			{
				sockaddr_in addr = clientPtr->serverIp.GetSockaddrIPv4();
				Packet p = clientPtr->udpConnection.pm.Retrieve();
				if(clientPtr->udpConnection.socket.SendTo(p,addr,sizeof(addr)) != PResult::P_Success)
					std::cout << "Failed to send UDP packet to server: " << std::endl;
				p._buffer.clear(); //Clean up buffer from the packet p
			}
			//// send tcp packet
			while (clientPtr->tcpConnection.pm.HasPendingPackets())
			{
				Packet p = clientPtr->tcpConnection.pm.Retrieve();
				if (clientPtr->tcpConnection.socket.Send(p) != PResult::P_Success)
				{
					std::cout << "Failed to send TCP packet to server: " << std::endl;
				}
				p._buffer.clear(); //Clean up buffer from the packet p
			}
			Sleep(5);
		}
	}
	bool Client::ProcessPacket(Packet & packet, long timeRecive, Connection &connection)
	{
		if (clientPtr == NULL)
		{
			std::cout << "exit ProcessPacket  because clientPtr is NULL"
				<< std::endl;
			return false;
		}
		switch (packet.GetPacketType())
		{
		case PacketType::PT_Time:
		{
			int bitIndex = NUM_BIT_PACKET;
			long t1, t2, t3, t4;
			t4 = timeRecive;
			t1 = packet.read_bits(bitIndex, sizeof(long) * 8);
			t2 = packet.read_bits(bitIndex, sizeof(long) * 8);
			t3 = packet.read_bits(bitIndex, sizeof(long) * 8);
			long lag1, lag2;
			lag1 = My_ABS(t2 - t1);
			lag2 = My_ABS(t4 - t3);
			if (My_ABS(lag1 - lag2) > Min_Time_Ping)
			{
				std::cout << "Sync time dont exact, request server again" << timeDifference << std::endl;
				Packet timePacket(PacketType::PT_Time);
				long t1 = GetTickCount();
				timePacket.write_bits(t1, sizeof(long) * 8);
				connection.pm.Append(timePacket);
			}
			else
			{
				timeDifference = ((t2 - t1) + (t3 - t4)) / 2;
				std::cout << "receve time from sever, t2:" << t2 << " t3:" << t3 << std::endl;
				std::cout << "t1:" << t1 << " t4:" << t4 << std::endl;
				std::cout << "different time:" << timeDifference << std::endl;
			}
			std::cout << "PT_Time:" << My_ABS(lag1 - lag2) << std::endl;
			break;
		}
		case PacketType::PT_ConnectUdp:
		{
			clientPtr->udpConnection.isConnect = true;
			break;
		}
		case PacketType::PT_SetID:
		{
			int bitIndex = NUM_BIT_PACKET;
			uint8_t _id;
			_id = packet.read_bits(bitIndex, 3);
			clientPtr->id = _id;
			std::cout << (unsigned)"PT_SetID:" << clientPtr->id << std::endl;
			break;
		}
		case PacketType::PT_Start:
		{
			int bitIndex = NUM_BIT_PACKET;
			uint8_t _numPlayer;
			_numPlayer = packet.read_bits(bitIndex, 3);
			clientPtr->numPlayer = _numPlayer;
			Packet p(PacketType::PT_Start);
			p._buffer = packet._buffer;
			clientPtr->gamePacket.Append(p);
			break;
		}
		case PacketType::PT_TankProperties:
		{
			if (clientPtr->numPlayer <= 0)
				break;
			int bitIndex = NUM_BIT_PACKET;
			uint16_t packetID;
			packetID = packet.read_bits(bitIndex, sizeof(uint16_t) * 8);
			if (clientPtr->packetReceiveID > packetID)
			{
				std::cout << "igone PT_TankProperties id:" << packetID << std::endl;
				break;
			}
			clientPtr->packetReceiveID = packetID;
			Packet p(PacketType::PT_TankProperties);
			p._buffer = packet._buffer;
			clientPtr->gamePacket.Append(p);
			break;
		}
		case PacketType::PT_Reset:
		{
			std::cout << "PT_Reset" << std::endl;
			Packet p(PacketType::PT_Reset);
			p._buffer = packet._buffer;
			clientPtr->gamePacket.Append(p);
			break;
		}
		case PacketType::PT_UpdateMap:
		{
			Packet p(PacketType::PT_UpdateMap);
			p._buffer = packet._buffer;
			clientPtr->gamePacket.Append(p);
			break;
		}
		case PacketType::PT_Shoot:
		{
			Packet p(PacketType::PT_Shoot);
			p._buffer = packet._buffer;
			clientPtr->gamePacket.Append(p);
			break;
		}
		case PacketType::PT_Item:
		{
			Packet p(PacketType::PT_Item);
			p._buffer = packet._buffer;
			clientPtr->gamePacket.Append(p);
		}
		default:
			return false;
		}
		return true;
	}
	void Client::ThreadHandleTime()
	{
		while (true)
		{
			Sleep(500);
			/*long time = GetTickCount();
			std::cout << "Time:" << time + timeDifference << std::endl;*/
		}
	}
}
