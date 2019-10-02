#pragma once
#include "SocketHandle.h"
#include "PResult.h"
#include "IPVersion.h"
#include "SocketOption.h"
#include "IPEndpoint.h"
#include "Constants.h"
#include "Packet.h"
#include "SocketType.h"
namespace PNet
{
	class Socket
	{
	public:
		Socket(IPVersion ipversion = IPVersion::IPv4,
			SocketHandle handle = INVALID_SOCKET,
			SocketType type = SocketType::TCP, uint8_t idNetwork = UNDEFINE_ID);
		PResult Create(SocketType type = SocketType::TCP);
		PResult Close();
		PResult Bind(IPEndpoint endpoint);
		PResult Listen(IPEndpoint endpoint, int backlog = 5);
		PResult Accept(Socket & outSocket);
		PResult Connect(IPEndpoint endpoint);
		PResult Send(const void * data, int numberOfBytes, int & bytesSent);
		PResult SendTo(const void * data, int numberOfBytes, int & bytesSent, sockaddr_in to, int tolen);
		PResult Recv(void * destination, int numberOfBytes, int & bytesReceived);
		PResult RecvFrom(void * destination, int numberOfBytes, int & bytesReceived,  sockaddr *from, int *fromlen);
		PResult SendAll(const void * data, int numberOfBytes);
		PResult SendAllTo(const void * data, int numberOfBytes, sockaddr_in to, int tolen);
		PResult RecvAll(void * destination, int numberOfBytes);
		PResult RecvAllFrom(void * destination, int numberOfBytes, sockaddr *from, int *fromlen);
		PResult Send(Packet & packet);
		PResult SendTo(Packet & packet, sockaddr_in to, int tolen);
		PResult Recv(Packet & packet);
		PResult RecvFrom(Packet & packet, sockaddr *from, int *fromlen);
		SocketHandle GetHandle();
		IPVersion GetIPVersion();
		void SetIDNetWork(uint8_t id) { idNetWork = id; }
	private:
		PResult SetSocketOption(SocketOption option, BOOL value);
		IPVersion ipversion = IPVersion::IPv4;
		SocketHandle handle = INVALID_SOCKET;
		SocketType type = SocketType::TCP;
		uint8_t idNetWork;
	};
}