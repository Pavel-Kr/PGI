#pragma once
#include <stdio.h>

#include "BaseReader.h"

#define PCX_ID 0x0A

#pragma pack(1)
typedef struct {
	unsigned char id;
	unsigned char version;
	unsigned char encoding;
	unsigned char bits_per_pixel;
	unsigned short min_x;
	unsigned short min_y;
	unsigned short max_x;
	unsigned short max_y;
	unsigned short horizontal_dpi;
	unsigned short vertical_dpi;
	unsigned char palette[48];
	unsigned char reserved1;
	unsigned char planes;
	unsigned short bytes_per_line;
	unsigned short palette_mode;
	unsigned short src_h_res;
	unsigned short src_v_res;
	unsigned char reserved2[54];
} PCXFileHeader;

class PCXReader : public BaseReader
{
	PCXFileHeader header;
	unsigned char* pixels;
	RGBTriple* palette;

	bool readHeader(FILE* file);
	void printHeader();
	bool readPixels(FILE* file);
public:
	PCXReader();
	bool readFile(const char* path) override;
	long getImageWidth();
	long getImageHeight();
	RGBQuad getPixelColor(int x, int y);
	~PCXReader();
};

