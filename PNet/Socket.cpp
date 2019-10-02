#include "Socket.h"
#include <assert.h>
#include <iostream>
namespace PNet
{
	Socket::Socket(IPVersion ipversion, SocketHandle handle, SocketType _type, uint8_t _idNetwork)
		:ipversion(ipversion), handle(handle), type(_type), idNetWork(_idNetwork)
	{
		assert(ipversion == IPVersion::IPv4);
	}

	PResult Socket::Create(SocketType _type)
	{
		assert(ipversion == IPVersion::IPv4);
		
		type = _type;
		
		if (handle != INVALID_SOCKET)
		{
			return PResult::P_GenericError;
		}
		
		if (type == SocketType::TCP)
			handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		else
			handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (handle == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		if (type == SocketType::TCP)
		{
			if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != PResult::P_Success)
			{
				return PResult::P_GenericError;
			}
		}

		return PResult::P_Success;
	}

	PResult Socket::Close()
	{
		if (handle == INVALID_SOCKET)
		{
			return PResult::P_GenericError;
		}

		int result = closesocket(handle);
		if (result != 0) //if error occurred while trying to close socket
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		handle = INVALID_SOCKET;
		return PResult::P_Success;
	}

	PResult Socket::Bind(IPEndpoint endpoint)
	{
		sockaddr_in addr = endpoint.GetSockaddrIPv4();
		int result = bind(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if (result != 0) //if an error occurred
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::Listen(IPEndpoint endpoint, int backlog)
	{
		if (Bind(endpoint) != PResult::P_Success)
		{
			return PResult::P_GenericError;
		}

		int result = listen(handle, backlog);
		if (result != 0) //If an error occurred
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::Accept(Socket & outSocket)
	{
		assert(type == SocketType::TCP);
		sockaddr_in addr = {};
		int len = sizeof(sockaddr_in);
		SocketHandle acceptedConnectionHandle = accept(handle, (sockaddr*)(&addr), &len);
		if (acceptedConnectionHandle == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		IPEndpoint newConnectionEndpoint((sockaddr*)&addr);
		std::cout << "New connection accepted!" << std::endl;
		newConnectionEndpoint.Print();
		outSocket = Socket(IPVersion::IPv4, acceptedConnectionHandle);
		return PResult::P_Success;
	}

	PResult Socket::Connect(IPEndpoint endpoint)
	{
		assert(type == SocketType::TCP);
		sockaddr_in addr = endpoint.GetSockaddrIPv4();
		int result = connect(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if (result != 0) //if an error occurred
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	PResult Socket::Send(const void * data, int numberOfBytes, int & bytesSent)
	{
		assert(type == SocketType::TCP);

		bytesSent = send(handle, (const char*)data, numberOfBytes, NULL);

		if (bytesSent == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::SendTo(const void * data, int numberOfBytes, int & bytesSent, sockaddr_in to, int tolen)
	{

		bytesSent = sendto(handle, (const char*)data, numberOfBytes, NULL, (sockaddr*)&to, tolen);

		if (bytesSent == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::Recv(void * destination, int numberOfBytes, int & bytesReceived)
	{
		assert(type == SocketType::TCP);

		bytesReceived = recv(handle, (char*)destination, numberOfBytes, NULL);

		if (bytesReceived == 0) //If connection was gracefully closed
		{
			return PResult::P_GenericError;
		}

		if (bytesReceived == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::RecvFrom(void * destination, int numberOfBytes, int & bytesReceived,  sockaddr * from, int * fromlen)
	{

		bytesReceived = recvfrom(handle, (char*)destination, numberOfBytes, NULL, from, fromlen);

		if (bytesReceived == 0) //If connection was gracefully closed
		{
			return PResult::P_GenericError;
		}

		if (bytesReceived == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}

	PResult Socket::SendAll(const void * data, int numberOfBytes)
	{
		assert(type == SocketType::TCP);
		int totalBytesSent = 0;

		while (totalBytesSent < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesSent;
			int bytesSent = 0;
			char * bufferOffset = (char*)data + totalBytesSent;
			PResult result = Send(bufferOffset, bytesRemaining, bytesSent);
			if (result != PResult::P_Success)
			{
				return PResult::P_GenericError;
			}
			totalBytesSent += bytesSent;
		}

		return PResult::P_Success;
	}

	PResult Socket::SendAllTo(const void * data, int numberOfBytes, sockaddr_in to, int tolen)
	{
		int totalBytesSent = 0;

		while (totalBytesSent < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesSent;
			int bytesSent = 0;
			char * bufferOffset = (char*)data + totalBytesSent;
			PResult result = SendTo(bufferOffset, bytesRemaining, bytesSent, to, tolen);
			if (result != PResult::P_Success)
			{
				return PResult::P_GenericError;
			}
			totalBytesSent += bytesSent;
		}

		return PResult::P_Success;
	}

	PResult Socket::RecvAll(void * destination, int numberOfBytes)
	{
		assert(type == SocketType::TCP);

		int totalBytesReceived = 0;

		while (totalBytesReceived < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesReceived;
			int bytesReceived = 0;
			char * bufferOffset = (char*)destination + totalBytesReceived;
			PResult result = Recv(bufferOffset, bytesRemaining, bytesReceived);
			if (result != PResult::P_Success)
			{
				return PResult::P_GenericError;
			}
			totalBytesReceived += bytesReceived;
		}

		return PResult::P_Success;
	}

	PResult Socket::RecvAllFrom(void * destination, int numberOfBytes, sockaddr * from, int * fromlen)
	{
		int totalBytesReceived = 0;

		while (totalBytesReceived < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesReceived;
			int bytesReceived = 0;
			char * bufferOffset = (char*)destination + totalBytesReceived;
			PResult result = RecvFrom(bufferOffset, bytesRemaining, bytesReceived, from, fromlen);
			if (result != PResult::P_Success)
			{
				return PResult::P_GenericError;
			}
			totalBytesReceived += bytesReceived;
		}

		return PResult::P_Success;
	}

	PResult Socket::Send(Packet & packet)
	{
		assert(type == SocketType::TCP);

		uint8_t encodedPacketSize = (packet._buffer.size());
		PResult result = SendAll(&encodedPacketSize, sizeof(uint8_t));
		if (result != PResult::P_Success)
			return PResult::P_GenericError;

		result = SendAll(packet._buffer.data(), packet._buffer.size());
		if (result != PResult::P_Success)
			return PResult::P_GenericError;

		return PResult::P_Success;
	}

	PResult Socket::SendTo(Packet & packet, sockaddr_in to, int tolen)
	{
		int bufferSize;
		if (idNetWork == SERVER_ID)
			bufferSize = SERVER_BUFFER_SIZE;
		else
			bufferSize = CLIENT_BUFFER_SIZE;
		PResult result = SendAllTo(packet._buffer.data(), bufferSize, to, tolen);
		if (result != PResult::P_Success)
			return PResult::P_GenericError;
		return PResult::P_Success;
	}

	PResult Socket::Recv(Packet & packet)
	{
		assert(type == SocketType::TCP);


		uint8_t bufferSize = 0;
		PResult result = RecvAll(&bufferSize, sizeof(uint8_t));
		if (result != PResult::P_Success)
			return PResult::P_GenericError;

		if (bufferSize > PNet::g_MaxPacketSize)
			return PResult::P_GenericError;

		packet._buffer.resize(bufferSize);
		result = RecvAll(&packet._buffer[0], bufferSize);
		if (result != PResult::P_Success)
			return PResult::P_GenericError;

		return PResult::P_Success;
	}

	PResult Socket::RecvFrom(Packet & packet, sockaddr * from, int * fromlen)
	{
		int bufferSize;
		if (idNetWork != SERVER_ID)
			bufferSize = SERVER_BUFFER_SIZE;
		else
			bufferSize = CLIENT_BUFFER_SIZE;
		packet._buffer.resize(bufferSize);
		PResult result = RecvAllFrom(&packet._buffer[0], bufferSize, from, fromlen);
		if (result != PResult::P_Success)
			return PResult::P_GenericError;
		return PResult::P_Success;

	}


	SocketHandle Socket::GetHandle()
	{
		return handle;
	}

	IPVersion Socket::GetIPVersion()
	{
		return ipversion;
	}
	PResult Socket::SetSocketOption(SocketOption option, BOOL value)
	{
		int result = 0;
		switch (option)
		{
		case SocketOption::TCP_NoDelay:
			result = setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
			break;
		default:
			return PResult::P_GenericError;
		}

		if (result != 0) //If an error occurred
		{
			int error = WSAGetLastError();
			return PResult::P_GenericError;
		}

		return PResult::P_Success;
	}
}