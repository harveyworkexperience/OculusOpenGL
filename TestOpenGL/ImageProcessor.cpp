#include "ImageProcessor.h"
#include <fstream>

ImageProcessor::ImageProcessor() 
{
	stop = false;
	imageReady = false;
};

ImageProcessor::~ImageProcessor() {};

void ImageProcessor::LoadImageBytesFromPath(std::string path) 
{
	std::ifstream infile(path);

	//get length of file
	infile.seekg(0, infile.end);
	imageLength = infile.tellg();
	infile.seekg(0, infile.beg);

	// don't overflow the buffer!
	if (imageLength > sizeof(buffer))
	{
		imageLength = sizeof(buffer);
	}

	//read file
	infile.read(buffer, imageLength);
}

char* ImageProcessor::GetImage()
{
	return buffer;
}

void ImageProcessor::ReceiveBytes()
{
	while (stop)
	{
		int foundJPGHeader = 0;
		char b;

	}
}