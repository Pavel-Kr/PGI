#pragma once

typedef struct {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char reserved;
} RGBQuad;

typedef struct {
	unsigned char blue;
	unsigned char red;
	unsigned char green;
} RGBTriple;

class BaseReader
{
public:
	virtual bool readFile(const char* path) = 0;
	virtual long getImageWidth() = 0;
	virtual long getImageHeight() = 0;
	virtual RGBQuad getPixelColor(int x, int y) = 0;
};