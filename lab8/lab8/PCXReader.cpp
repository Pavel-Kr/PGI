#define _CRT_SECURE_NO_WARNINGS
#include "PCXReader.h"

bool PCXReader::readHeader(FILE* file)
{
	fread(&header, sizeof(header), 1, file);
	printHeader();
	// Sanity checks
	if (header.id != PCX_ID) {
		printf("This is not a PCX file!\n");
		return false;
	}
	if (header.encoding != 1) {
		printf("Uncompressed PCX files not supported!\n");
		return false;
	}
	if (header.bits_per_pixel != 8) {
		printf("Only 8-bit planes are supported!\n");
		return false;
	}
	return true;
}

void PCXReader::printHeader()
{
	printf("\n***************FILE HEADERS***************\n");
	printf("\t Format: PCX\n");
	printf("\t Format ID: %02X\n", header.id);
	printf("\t Version: %d\n", header.version);
	printf("\t Encoding: %d\n", header.encoding);
	printf("\t Bits per plane: %d\n", header.bits_per_pixel);
	printf("\t Min X: %d\n", header.min_x);
	printf("\t Max X: %d\n", header.max_x);
	printf("\t Min Y: %d\n", header.min_y);
	printf("\t Max Y: %d\n", header.max_y);
	printf("\t Horizontal DPI: %d\n", header.horizontal_dpi);
	printf("\t Vertical DPI: %d\n", header.vertical_dpi);
	printf("\t Planes: %d\n", header.planes);
	printf("\t Bytes per line: %d\n", header.bytes_per_line);
	printf("\t Palette mode: %d\n", header.palette_mode);
	printf("\t Horizontal source resolution: %d\n", header.src_h_res);
	printf("\t Vertical source resolution: %d\n", header.src_v_res);
	printf("******************************************\n\n");
}

bool PCXReader::readPixels(FILE* file)
{
	long image_width = getImageWidth();
	long image_height = getImageHeight();
	unsigned int scan_line_length = header.bytes_per_line * header.planes;
	int padding_size = scan_line_length - image_width;
	unsigned int size = scan_line_length * image_height;
	pixels = new unsigned char[size];
	if (pixels == NULL) {
		printf("Failed to allocate %d bytes of memory for pixels\n", size);
		return false;
	}
	switch (header.planes)
	{
	case 1:
	{
		unsigned int pixels_offset = 0;
		for (int i = 0; i < image_height; i++) {
			// Decode single line
			int index = 0;
			while (index < image_width)
			{
				unsigned char byte;
				fread(&byte, 1, 1, file);
				// Check if it is run length
				if ((byte & 0xC0) == 0xC0) {
					int run_length = byte & 0x3F;
					fread(&byte, 1, 1, file);
					for (int j = 0; j < run_length; j++) {
						pixels[pixels_offset++] = byte;
						index++;
					}
				}
				else {
					pixels[pixels_offset++] = byte;
					index++;
				}
			}
		}
		break;
	}
	default:
		printf("%d planes format not supported\n", header.planes);
		return false;
	}
	return true;
}

PCXReader::PCXReader()
{
	pixels = NULL;
	palette = NULL;
}

bool PCXReader::readFile(const char* path)
{
	printf("Try to read file %s\n", path);
	FILE* file = fopen(path, "rb");
	if (file == NULL) {
		printf("Failed to open file %s\n", path);
		return false;
	}
	if (!readHeader(file)) {
		return false;
	}
	if (!readPixels(file)) {
		return false;
	}
	return true;
}

long PCXReader::getImageWidth()
{
	return header.max_x - header.min_x + 1;
}

long PCXReader::getImageHeight()
{
	return header.max_y - header.min_y + 1;
}

RGBQuad PCXReader::getPixelColor(int x, int y)
{
	return RGBQuad();
}

PCXReader::~PCXReader()
{
	if (pixels != NULL) {
		delete[] pixels;
	}
	if (palette != NULL) {
		delete[] palette;
	}
}
