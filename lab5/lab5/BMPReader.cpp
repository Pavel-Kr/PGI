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
	for (int i = 0; i < palette_size; i++) {
		fread(&palette[i], sizeof(RGBQuad), 1, file);
	}
	printf("Read %d colors\n", palette_size);
	return true;
}

bool BMPReader::readPixels(FILE* file)
{
	if (info_header.compression != 0) {
		printf("Only uncompressed BMP files supported!\n");
		return false;
	}
	int img_size = info_header.size_image;
	if (img_size == 0) {
		printf("Image size = 0\n");
		return false;
	}
	unsigned char* buffer = new unsigned char[img_size];
	int pixel_size = info_header.bit_count / 8;
	pixels = new unsigned char[info_header.pic_width * info_header.pic_height * pixel_size];
	fseek(file, bm_header.bits_offset, SEEK_SET);
	fread(buffer, 1, img_size, file);
	printf("Read %d bytes\n", img_size);
	int byte_width = info_header.pic_width * pixel_size;
	int padded_width = calcPaddedWidth(byte_width);
	for (int i = 0; i < info_header.pic_height; i++) {
		for (int j = 0; j < info_header.pic_width; j++) {
			int to = i * byte_width + j * pixel_size;
			int from = i * padded_width + j * pixel_size;
			for (int k = 0; k < pixel_size; k++) {
				pixels[to + k] = buffer[from + k];
			}
		}
	}
	printf("Read %d pixels\n", info_header.pic_width * info_header.pic_height);
	delete[] buffer;
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
	unsigned int new_img_size = new_width * new_height;
	printf("New image size:\n");
	printf("\tWidth = %d\n", new_width);
	printf("\tHeight = %d\n", new_height);
	printf("\tSize = %d\n", new_img_size);
	unsigned char* new_pixels = new unsigned char[new_img_size];
	unsigned int new_byte_size = calcImageSize(new_width, new_height);
	memset(new_pixels, 0, new_img_size);
	unsigned int offset_new = 0;
	unsigned int offset_old = 0;
	// top edge
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < new_width; j++) {
			color = rand() % 256;
			new_pixels[offset_new++] = color;
		}
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
	}
	// bottom edge
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < new_width; j++) {
			color = rand() % 256;
			new_pixels[offset_new++] = color;
		}
	}
	delete[] pixels;
	pixels = new_pixels;
	info_header.pic_width = new_width;
	info_header.pic_height = new_height;
	bm_header.file_size += new_byte_size - info_header.size_image;
	info_header.size_image = new_byte_size;
}

void BMPReader::addFrameTrueColor(int width)
{
	RGBQuad color;
	long new_width = info_header.pic_width + 2 * width;
	long new_height = info_header.pic_height + 2 * width;
	int pixel_size = info_header.bit_count / 8;
	unsigned int new_img_size = new_width * new_height * pixel_size;
	printf("New image size:\n");
	printf("\tWidth = %d\n", new_width);
	printf("\tHeight = %d\n", new_height);
	printf("\tSize = %d\n", new_img_size);
	unsigned char* new_pixels = new unsigned char[new_img_size];
	unsigned int new_byte_size = calcImageSize(new_width, new_height);
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
	}
	delete[] pixels;
	pixels = new_pixels;
	info_header.pic_width = new_width;
	info_header.pic_height = new_height;
	bm_header.file_size += new_byte_size - info_header.size_image;
	info_header.size_image = new_byte_size;
}

BMPReader::BMPReader()
{
	palette = NULL;
	pixels = NULL;
	palette_size = 0;
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
	printf("enter writeFile()\n");
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
	unsigned char* buffer = new unsigned char[info_header.size_image];
	int pixel_size = info_header.bit_count / 8;
	int byte_width = info_header.pic_width * pixel_size;
	int padded_width = calcPaddedWidth(byte_width);
	for (int i = 0; i < info_header.pic_height; i++) {
		for (int j = 0; j < info_header.pic_width; j++) {
			int from = i * byte_width + j * pixel_size;
			int to = i * padded_width + j * pixel_size;
			for (int k = 0; k < pixel_size; k++) {
				buffer[to + k] = pixels[from + k];
			}
		}
	}
	fseek(file, bm_header.bits_offset, SEEK_SET);
	fwrite(buffer, 1, info_header.size_image, file);
	fclose(file);
	printf("exit writeFile()\n");
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
	if (!img_loaded) {
		printf("Image not loaded!\n");
		return;
	}
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

void BMPReader::rotate90Degrees()
{
	if (!img_loaded) {
		printf("Image not loaded!\n");
		return;
	}
	int new_width = info_header.pic_height;
	int new_height = info_header.pic_width;
	int pixel_size = info_header.bit_count / 8;
	int new_img_size = new_width * new_height * pixel_size;
	unsigned char* new_pixels = new unsigned char[new_img_size];
	unsigned int new_byte_size = calcImageSize(new_width, new_height);
	memset(new_pixels, 0, new_img_size);
	printf("New image size:\n");
	printf("\tWidth = %d\n", new_width);
	printf("\tHeight = %d\n", new_height);
	printf("\tSize = %d\n", new_img_size);
	int byte_width_new = new_width * pixel_size;
	int byte_width_old = info_header.pic_width * pixel_size;
	for (int i = 0; i < new_height; i++) {
		for (int j = 0; j < new_width; j++) {
			int to = i * byte_width_new + j * pixel_size;
			int from = (info_header.pic_height - j - 1) * byte_width_old + i * pixel_size;
			for (int k = 0; k < pixel_size; k++) {
				new_pixels[to + k] = pixels[from + k];
			}
		}
	}
	delete[] pixels;
	pixels = new_pixels;
	info_header.pic_width = new_width;
	info_header.pic_height = new_height;
	bm_header.file_size += new_byte_size - info_header.size_image;
	info_header.size_image = new_byte_size;
	printHeaders();
}

long BMPReader::getImageWidth()
{
	return info_header.pic_width;
}

long BMPReader::getImageHeight()
{
	return info_header.pic_height;
}

RGBQuad BMPReader::getPixelColor(int x, int y)
{
	RGBQuad color;
	int stride = info_header.pic_width * info_header.bit_count / 8;
	switch (info_header.bit_count)
	{
	case 8:
	{
		int index = y * stride + x;
		int color_index = pixels[index];
		color = palette[color_index];
		break;
	}
	case 24:
	{
		int index = y * stride + x * 3;
		color.blue = pixels[index++];
		color.green = pixels[index++];
		color.red = pixels[index++];
		break;
	}
	default:
		break;
	}
	return color;
}

void BMPReader::scale(double factor)
{
	switch (info_header.bit_count)
	{
	case 8:
	{
		long new_width = info_header.pic_width * factor;
		long new_height = info_header.pic_height * factor;
		unsigned int new_img_size = new_width * new_height;
		printf("New image size:\n");
		printf("\tWidth = %d\n", new_width);
		printf("\tHeight = %d\n", new_height);
		printf("\tSize = %d\n", new_img_size);
		unsigned char* new_pixels = new unsigned char[new_img_size];
		unsigned int new_byte_size = calcImageSize(new_width, new_height);
		memset(new_pixels, 0, new_img_size);
		for (int i = 0; i < new_height; i++) {
			for (int j = 0; j < new_width; j++) {
				int from = (int)(i / factor) * info_header.pic_width + (int)(j / factor);
				int to = i * new_width + j;
				new_pixels[to] = pixels[from];
			}
		}
		delete[] pixels;
		pixels = new_pixels;
		info_header.pic_width = new_width;
		info_header.pic_height = new_height;
		bm_header.file_size += new_byte_size - info_header.size_image;
		info_header.size_image = new_byte_size;
		break;
	}
	case 24: 
	{
		printf("24-bit image scaling not supported!\n");
		return;
	}
	default:
		break;
	}
}

BMPReader::~BMPReader()
{
	if (palette != NULL)
		delete[] palette;
	if (pixels != NULL)
		delete[] pixels;
}
