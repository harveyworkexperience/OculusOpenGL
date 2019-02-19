#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

class ImageProcessor
{
public:
	// Variables
	char		buffer[5000000];
	char		image[5000000];
	size_t		imageLength;
	bool		stop;
	bool		imageReady;

	// Constructor & Deconstructor
	ImageProcessor();
	~ImageProcessor();

	// Methods
	void		LoadImageBytesFromPath(std::string path);
	void		ReceiveBytes();
	int			initReceiver();
	char*		GetImage();
};

#endif //IMAGE_PROCESSOR_H