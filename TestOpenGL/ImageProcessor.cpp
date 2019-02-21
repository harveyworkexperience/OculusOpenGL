#include "ImageProcessor.h"
#include <fstream>
#include <limits.h>

ImageProcessor::ImageProcessor() 
{
	stopRecv = true;
	bufInit = false;
	bufPtr = 0;
	imageReady = false;
	recv_len = 0;
}

ImageProcessor::~ImageProcessor() 
{
	if (!stopRecv) {
		SignalImageUsed();
		stopRecv = true;
		TheThread.join();
		delete Socket;
		delete Session;
	}
}

int ImageProcessor::init(std::string ipAddr, unsigned short portNum)
{
	char buf[100];
	try
	{
		Session = new WSASession();
		Socket = new UDPSocket();
		Socket->SendTo(ipAddr, 11000, "Send me stuff!", 14);
		Socket->RecvFrom(buf, 100);
		std::cout << buf << std::endl;
	}
	catch (std::system_error& e)
	{
		std::cout << e.what();
		return -1;
	}

	stopRecv = false;
	TheThread = std::thread([this] { this->ReceiveBytes(); });

	return 0;
}

void ImageProcessor::LoadImageBytesFromPath(std::string path) 
{
	std::ifstream infile(path);

	//get length of file
	infile.seekg(0, infile.end);
	imageLength = infile.tellg();
	infile.seekg(0, infile.beg);

	// don't overflow the buffer!
	if (imageLength > sizeof(image))
	{
		imageLength = sizeof(image);
	}

	//read file
	infile.read(image, imageLength);
}

void ImageProcessor::SignalImageUsed()
{
	if (imageReady) {
		imageReady = false;
		cv.notify_one();
	}
}

void ImageProcessor::ReceiveBytes()
{
	std::cout << "CHAR_MIN = " << CHAR_MIN << std::endl;
	std::cout << "Thread started ..." << std::endl;
	while (!stopRecv)
	{
		// Looking for JPEG headers
		imageReady = false;
		int foundJPGHeader = 0;
		char byte;
		//std::cout << 1 << std::endl;
		while (!stopRecv)
		{
			//std::cout << 2 << std::endl;
			byte = (char)GetStreamByte();
			if (stopRecv)
				return;
			if (byte == (char)0xff)
				foundJPGHeader = 1;
			else if (byte == (char)0xd8 && foundJPGHeader == 1)
				break;
			else
				foundJPGHeader = 0;
		}

		if (stopRecv)
			return;

		//std::cout << 3 << std::endl;

		// Initialising new image
		int byteCount = 0;
		memset(image, 0, IMGSIZE);
		image[byteCount++] = (char)0xff;
		image[byteCount++] = (char)0xd8;

		// Building image
		
		//std::cout << 4 << std::endl;

		int endFlag = 0;
		while (byteCount < IMGSIZE && !stopRecv)
		{
			//std::cout << 5 << " " << stopRecv << std::endl;
			char tempByte = GetStreamByte();
			if (stopRecv) 
				return;
			image[byteCount++] = tempByte;
			if (tempByte == (char)0xff && endFlag == 0)
				endFlag++;
			else if (tempByte == (char)0xd9 && endFlag == 1)
				break;
			else
				endFlag = 0;
		}

		//std::cout << 6 << std::endl;

		// Finished building image
		image[byteCount - 2] = (char)0xff;
		image[byteCount - 1] = (char)0xd9;
		imageLength = byteCount;
		imageReady = true;
		
		// Waiting for signal to modify image
		std::unique_lock<std::mutex> lck(mtx);
		cv.wait(lck);

		//std::cout << 8 << std::endl;
	}
}

char ImageProcessor::GetStreamByte()
{
	// Updating buffer
	if (!bufInit || bufPtr >= recv_len)
	{
		memset(buffer, 0, BUFSIZE);
		recv_len = 0;
		// buffer has to be larger than receive packet
		try {
			recv_len = Socket->RecvFrom(buffer, BUFSIZE);
		}
		catch (std::exception &ex) {
			std::cout << ex.what() << std::endl;
			stopRecv = true;
		}
		bufInit = true;
		bufPtr = 0;
	}
	return buffer[bufPtr++];
}