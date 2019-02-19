#include "UDPClient.h"
#include <sstream>

UDPClient::UDPClient()
{
	ready = false;
}

UDPClient::~UDPClient()
{
	closesocket(sd);
	WSACleanup();
};

int UDPClient::init(std::string serverAddress, unsigned short portNumber)
{
	std::istringstream ss(serverAddress);
	std::string token;
	int count = 0;
	int* a_arr[4] = { &a1,&a2,&a3,&a4 };
	while (std::getline(ss, token, '.')) {
		*a_arr[count] = std::stoi(token);
		count++;
	}
	if (count != 4)
		return -1;

	port_number = portNumber;

	/* Open windows connection */
	if (WSAStartup(0x0101, &w) != 0)
	{
		fprintf(stderr, "Could not open Windows connection.\n");
		return -1;
	}

	/* Open a datagram socket */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd == INVALID_SOCKET)
	{
		fprintf(stderr, "Could not create socket.\n");
		WSACleanup();
		return -1;
	}

	/* Clear out server struct */
	memset((void *)&server, '\0', sizeof(struct sockaddr_in));

	/* Set family and port */
	server.sin_family = AF_INET;
	server.sin_port = htons(port_number);

	/* Set server address */
	server.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)a1;
	server.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)a2;
	server.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)a3;
	server.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)a4;

	/* Clear out client struct */
	memset((void *)&client, '\0', sizeof(struct sockaddr_in));

	/* Set family and port */
	client.sin_family = AF_INET;
	client.sin_port = htons(0);

	/* Get host name of this computer */
	gethostname(host_name, sizeof(host_name));
	hp = gethostbyname(host_name);

	/* Check for NULL pointer */
	if (hp == NULL)
	{
		fprintf(stderr, "Could not get host name.\n");
		closesocket(sd);
		WSACleanup();
		return -1;
	}

	/* Assign the address */
	client.sin_addr.S_un.S_un_b.s_b1 = hp->h_addr_list[0][0];
	client.sin_addr.S_un.S_un_b.s_b2 = hp->h_addr_list[0][1];
	client.sin_addr.S_un.S_un_b.s_b3 = hp->h_addr_list[0][2];
	client.sin_addr.S_un.S_un_b.s_b4 = hp->h_addr_list[0][3];

	/* Bind local address to socket */
	if (bind(sd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, "Cannot bind address to socket.\n");
		closesocket(sd);
		WSACleanup();
		return -1;
	}

	server_length = sizeof(struct sockaddr_in);
	ready = true;
	return 0;
}

int UDPClient::udpsendto(const char* msg, int msglen)
{
	if (!ready)
		return -1;

	int sent_len = 0;

	/* Tranmsit data to get time */
	server_length = sizeof(struct sockaddr_in);
	if ((sent_len = sendto(sd, msg, msglen, 0, (struct sockaddr *)&server, server_length)) == -1)
	{
		fprintf(stderr, "Error transmitting data.\n");
		closesocket(sd);
		WSACleanup();
		return -1;
	}
	return sent_len;
}

int UDPClient::udprecvfrom(char* buffer, int buflen)
{
	if (!ready)
		return -1;

	int received_len = 0;

	/* Receive time */
	if ((received_len = recvfrom(sd, buffer, buflen, 0, (struct sockaddr *)&server, &server_length)) < 0)
	{
		fprintf(stderr, "Error receiving data.\n");
		closesocket(sd);
		WSACleanup();
		return -1;
	}
	return received_len;
}