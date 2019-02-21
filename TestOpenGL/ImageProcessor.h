#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "Network.h"

#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#define IMGSIZE		5000000
#define BUFSIZE		70000

class ImageProcessor
{
public:
	// Variables
	char						buffer[BUFSIZE];
	char						image[IMGSIZE];
	size_t						imageLength;
	bool						bufInit;
	int							bufPtr;
	int							recv_len;
	bool						stopRecv;
	bool						imageReady;
	std::mutex					mtx;
	std::condition_variable		cv;
	std::thread					TheThread;
	WSASession*					Session;
	UDPSocket*					Socket;

	// Constructor & Deconstructor
	ImageProcessor();
	~ImageProcessor();

	// Methods
	int			init(std::string ipAddr, unsigned short portNum);
	void		LoadImageBytesFromPath(std::string path);
	void		ReceiveBytes();
	void		SignalImageUsed();
	char		GetStreamByte();
};

#endif //IMAGE_PROCESSOR_H