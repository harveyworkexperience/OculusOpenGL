#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <string>

#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#define SIZE 500

class UDPClient
{
public:
	WSADATA w;											/* Used to open Windows connection */
	unsigned short port_number;							/* The port number to use */
	SOCKET sd;											/* The socket descriptor */
	int server_length;									/* Length of server struct */
	char send_buffer[SIZE] = "GET BYTE STREAM\r\n";		/* Data to send */
	struct hostent *hp;									/* Information about the server */
	struct sockaddr_in server;							/* Information about the server */
	struct sockaddr_in client;							/* Information about the client */
	int a1, a2, a3, a4;									/* Server address components in xxx.xxx.xxx.xxx form */
	int b1, b2, b3, b4;									/* Client address components in xxx.xxx.xxx.xxx form */
	char host_name[256];								/* Host name of this computer */
	bool ready;											/* Used to check if init was called */
	bool sock_opened;									/* Flag to check if socket is currently opened */

	UDPClient();
	~UDPClient();
	int init(std::string serverAddress, unsigned short portNumber);
	int udpsendto(const char* msg, int msglen);
	int udprecvfrom(char* buffer, int buflen);
};

#endif //UDP_CLIENT_H