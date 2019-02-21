// Code from: https://stackoverflow.com/questions/14665543/how-do-i-receive-udp-packets-with-winsock-in-c
// Couldn't get my own UDP library to work and was running out of time so using this one currently.
// Mine had annoying corruptions with the received packet for some reason.

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <system_error>
#include <string>
#include <iostream>

#pragma once

class WSASession
{
public:
	WSASession()
	{
		int ret = WSAStartup(MAKEWORD(2, 2), &data);
		if (ret != 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "WSAStartup Failed");
	}
	~WSASession()
	{
		WSACleanup();
	}
private:
	WSAData data;
};

class UDPSocket
{
public:
	UDPSocket()
	{
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET)
			throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
	}
	~UDPSocket()
	{
		closesocket(sock);
	}

	void SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags = 0)
	{
		sockaddr_in add;
		add.sin_family = AF_INET;
		InetPton(AF_INET, PCSTR(address.c_str()), &add.sin_addr.s_addr);
		add.sin_port = htons(port);
		int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
	}
	void SendTo(sockaddr_in& address, const char* buffer, int len, int flags = 0)
	{
		int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&address), sizeof(address));
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
	}
	int RecvFrom(char* buffer, int len, int flags = 0)
	{
		sockaddr_in from;
		int size = sizeof(from);
		int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&from), &size);
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");

		// make the buffer zero terminated
		buffer[ret] = 0;
		return ret;
	}
	int RecvFrom(const std::string& address, unsigned short port, char* buffer, int len, int flags = 0)
	{
		sockaddr_in add;
		add.sin_family = AF_INET;
		InetPton(AF_INET, PCSTR(address.c_str()), &add.sin_addr.s_addr);
		int size = sizeof(add);
		int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&add), &size);
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");

		// make the buffer zero terminated
		buffer[ret] = 0;
		return ret;
	}
	void Bind(unsigned short port)
	{
		sockaddr_in add;
		add.sin_family = AF_INET;
		add.sin_addr.s_addr = htonl(INADDR_ANY);
		add.sin_port = htons(port);

		int ret = bind(sock, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
	}

private:
	SOCKET sock;
};