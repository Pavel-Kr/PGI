#include "BMPReader.h"
#include <string.h>

void BMPReader::readFileHeaders(FILE* file)
{
	fread(&bm_header, sizeof(bm_header), 1, file);
	fread(&info_header, sizeof(info_header), 1, file);
	printHeaders();
}

bool BMPReader::readPalette(FILE* file)
{
	if (info_header.bit_count != 8) {
		printf("Only 8-bit BMP images are supported!");
		return false;
	}
	if (info_header.colors_used == 0)
		palette_size = 256;
	else
		palette_size = info_header.colors_used;
	palette = new RGBQuad[palette_size];
	//printf("Palette:\n");
	for (int i = 0; i < palette_size; i++) {
		fread(&palette[i], sizeof(RGBQuad), 1, file);
		//printf("0x%02X%02X%02X%02X\n", palette[i].blue, palette[i].green, palette[i].red, palette[i].reserved);
	}
	printf("Read %d colors\n", palette_size);
	return true;
}

bool BMPReader::readPixels(FILE* file)
{
	if (info_header.compression != 0) {
		printf("Only uncompressed BMP files supported!");
		return false;
	}
	img_size = info_header.size_image;
	if (img_size == 0) {
		printf("Image size = 0");
		return false;
	}
	pixels = new unsigned char[img_size];
	fseek(file, bm_header.bits_offset, SEEK_SET);
	fread(pixels, 1, img_size, file);
	//printf("First 10 bytes of pixels:\n");
	for (int i = 0; i < 10; i++) {
		//printf("%02X ", pixels[i]);
	}
	printf("Read %d pixels\n", img_size);
	return true;
}

long BMPReader::calcPaddedWidth(int width)
{
	int padded_width = width;
	if (padded_width % 4 != 0) {
		padded_width += 4 - (padded_width % 4);
	}
	return padded_width;
}

unsigned int BMPReader::calcImageSize(int width, int height)
{
	int padded_width = calcPaddedWidth(width);
	printf("Width = %d, padded width = %d\n", width, padded_width);
	int bytes_per_pixel = info_header.bit_count / 8;
	return padded_width * height * bytes_per_pixel;
}

void BMPReader::printHeaders()
{
	printf("Type = %X, file size = %d, res1 = %d, res2 = %d, bits_offset = %d\n",
		bm_header.type, bm_header.file_size, bm_header.res1, bm_header.res2, bm_header.bits_offset);
	printf("Header size = %d\n", info_header.info_size);
	printf("Image width = %d\n", info_header.pic_width);
	printf("Image height = %d\n", info_header.pic_height);
	printf("Planes = %d\n", info_header.planes);
	printf("Bit count = %d\n", info_header.bit_count);
	printf("Compression = %d\n", info_header.compression);
	printf("Image size = %d\n", info_header.size_image);
	printf("X PPM = %d\n", info_header.x_pixels_per_meter);
	printf("Y PPM = %d\n", info_header.y_pixels_per_meter);
	printf("Colors used = %d\n", info_header.colors_used);
	printf("Colors important = %d\n", info_header.colors_important);
}

void BMPReader::addFramePalette(int width)
{
	int color = 69;
	long new_width = info_header.pic_width + 2 * width;
	long new_height = info_header.pic_height + 2 * width;
	unsigned int new_img_size = calcImageSize(new_width, new_height);
	printf("New image size:\n");
	printf("\tWidth = %d\n", new_width);
	printf("\tHeight = %d\n", new_height);
	printf("\tSize = %d\n", new_img_size);
	unsigned char* new_pixels = new unsigned char[new_img_size];
	long padded_width = calcPaddedWidth(new_width);
	memset(new_pixels, 0, new_img_size);
	unsigned int offset_new = 0;
	unsigned int offset_old = 0;
	// top edge
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < new_width; j++) {
			color = rand() % 256;
			new_pixels[offset_new++] = color;
		}
		while (offset_new % 4 != 0)
			offset_new++;
	}
	// copy old pixels to new array
	for (int i = 0; i < info_header.pic_height; i++) {
		for (int j = 0; j < width; j++) {
			color = rand() % 256;
			new_pixels[offset_new++] = color;
		}
		for (int j = 0; j < info_header.pic_width; j++) {
			new_pixels[offset_new++] = pixels[offset_old++];
		}
		for (int j = 0; j < width; j++) {
			color = rand() % 256;
			new_pixels[offset_new++] = color;
		}
		while (offset_new % 4 != 0)
			offset_new++;
		while (offset_old % 4 != 0)
			offset_old++;
	}
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < new_width; j++) {
			color = rand() % 256;
			new_pixels[offset_new++] = color;
		}
		while (offset_new % 4 != 0)
			offset_new++;
	}
	delete[] pixels;
	pixels = new_pixels;
	info_header.pic_width = new_width;
	info_header.pic_height = new_height;
	bm_header.file_size += new_img_size - info_header.size_image;
	info_header.size_image = new_img_size;
	img_size = new_img_size;
}

void BMPReader::addFrameTrueColor(int width)
{
	RGBQuad color;
	long new_width = info_header.pic_width + 2 * width;
	long new_height = info_header.pic_height + 2 * width;
	unsigned int new_img_size = calcImageSize(new_width, new_height);
	printf("New image size:\n");
	printf("\tWidth = %d\n", new_width);
	printf("\tHeight = %d\n", new_height);
	printf("\tSize = %d\n", new_img_size);
	unsigned char* new_pixels = new unsigned char[new_img_size];
	long padded_width = calcPaddedWidth(new_width);
	memset(new_pixels, 0, new_img_size);
	unsigned int offset_new = 0;
	unsigned int offset_old = 0;
	// top edge
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < new_width; j++) {
			color.red = rand() % 256;
			color.green = rand() % 256;
			color.blue = rand() % 256;
			new_pixels[offset_new++] = color.blue;
			new_pixels[offset_new++] = color.green;
			new_pixels[offset_new++] = color.red;
		}
		while (offset_new % 4 != 0)
			offset_new++;
	}
	// copy old pixels to new array
	for (int i = 0; i < info_header.pic_height; i++) {
		for (int j = 0; j < width; j++) {
			color.red = rand() % 256;
			color.green = rand() % 256;
			color.blue = rand() % 256;
			new_pixels[offset_new++] = color.blue;
			new_pixels[offset_new++] = color.green;
			new_pixels[offset_new++] = color.red;
		}
		for (int j = 0; j < info_header.pic_width; j++) {
			new_pixels[offset_new++] = pixels[offset_old++];
			new_pixels[offset_new++] = pixels[offset_old++];
			new_pixels[offset_new++] = pixels[offset_old++];
		}
		for (int j = 0; j < width; j++) {
			color.red = rand() % 256;
			color.green = rand() % 256;
			color.blue = rand() % 256;
			new_pixels[offset_new++] = color.blue;
			new_pixels[offset_new++] = color.green;
			new_pixels[offset_new++] = color.red;
		}
		while (offset_new % 4 != 0)
			offset_new++;
		while (offset_old % 4 != 0)
			offset_old++;
	}
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < new_width; j++) {
			color.red = rand() % 256;
			color.green = rand() % 256;
			color.blue = rand() % 256;
			new_pixels[offset_new++] = color.blue;
			new_pixels[offset_new++] = color.green;
			new_pixels[offset_new++] = color.red;
		}
		while (offset_new % 4 != 0)
			offset_new++;
	}
	delete[] pixels;
	pixels = new_pixels;
	info_header.pic_width = new_width;
	info_header.pic_height = new_height;
	bm_header.file_size += new_img_size - info_header.size_image;
	info_header.size_image = new_img_size;
	img_size = new_img_size;
}

BMPReader::BMPReader()
{
	palette = NULL;
	pixels = NULL;
	palette_size = 0;
	img_size = 0;
	img_loaded = false;
}

bool BMPReader::readFile(const char* path)
{
	FILE* file;
	file = fopen(path, "rb");
	readFileHeaders(file);
	switch (info_header.bit_count)
	{
		case 8:
			printf("8-bit format\n");
			// colors stored in palette
			if (!readPalette(file)) {
				return false;
			}
			break;
		case 24:
			printf("24-bit format\n");
			break;
		default:
			printf("Only 8-bit abd 24-bit formats are supported!\n");
			return false;
	}
	if (!readPixels(file)) {
		return false;
	}
	fclose(file);
	img_loaded = true;
	return true;
}

void BMPReader::writeFile(const char* path)
{
	if (!img_loaded) {
		printf("Nothing to write!");
		return;
	}
	FILE* file = fopen(path, "wb");
	fwrite(&bm_header, sizeof(bm_header), 1, file);
	fwrite(&info_header, sizeof(info_header), 1, file);
	for (int i = 0; i < palette_size; i++) {
		fwrite(&palette[i], sizeof(RGBQuad), 1, file);
	}
	fseek(file, bm_header.bits_offset, SEEK_SET);
	fwrite(pixels, 1, img_size, file);
	fclose(file);
}

void BMPReader::convertToGrayscale()
{
	for (int i = 0; i < palette_size; i++) {
		int average = (palette[i].blue + palette[i].green + palette[i].red) / 3;
		palette[i].blue = palette[i].green = palette[i].red = average;
	}
}

void BMPReader::addFrame(int width)
{
	switch (info_header.bit_count)
	{
	case 8:
		addFramePalette(width);
		break;
	case 24:
		addFrameTrueColor(width);
		break;
	default:
		printf("Only 8-bit adn 24-bit formats are supported!\n");
		return;
	}
	printHeaders();
}

BMPReader::~BMPReader()
{
	if (palette != NULL)
		delete[] palette;
	if (pixels != NULL)
		delete[] pixels;
}
