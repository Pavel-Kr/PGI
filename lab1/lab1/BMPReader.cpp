#include "BMPReader.h"

void BMPReader::readFileHeaders(FILE* file)
{
	fread(&bm_header, sizeof(bm_header), 1, file);
	printf("Type = %X, file size = %d, res1 = %d, res2 = %d, bits_offset = %d\n",
		bm_header.type, bm_header.file_size, bm_header.res1, bm_header.res2, bm_header.bits_offset);
	fread(&info_header, sizeof(info_header), 1, file);
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
	if (!readPalette(file)) {
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

BMPReader::~BMPReader()
{
	if (palette != NULL)
		delete[] palette;
	if (pixels != NULL)
		delete[] pixels;
}
