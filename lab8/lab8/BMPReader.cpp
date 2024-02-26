#include "BMPReader.h"
#include <string.h>

bool BMPReader::readFileHeaders(FILE* file)
{
	// Read BITMAPFILEHEADER (always 14 bytes)
	fread(&bm_header, sizeof(bm_header), 1, file);
	// Read BITMAPINFO size
	fread(&info_header.info_size, sizeof(unsigned int), 1, file);
	// Set header version
	switch (info_header.info_size)
	{
	case 12:
		info_header_version = CORE;
		break;
	case 40:
		info_header_version = VERSION_3;
		break;
	case 108:
		info_header_version = VERSION_4;
		break;
	case 124:
		info_header_version = VERSION_5;
		break;
	default:
		printf("Invalid BITMAPINFO header size\n");
		return false;
	}
	// Read BITMAPINFO header itself
	fread(&info_header.pic_width, info_header.info_size - sizeof(unsigned int), 1, file);
	printHeaders();
	return true;
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
	if (info_header.compression != 0 && info_header.compression != 3) {
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

void BMPReader::printFileHeader()
{
	printf("File header:\n");
	printf("\tType = %X, file size = %d, res1 = %d, res2 = %d, bits_offset = %d\n",
		bm_header.type, bm_header.file_size, bm_header.res1, bm_header.res2, bm_header.bits_offset);
}

void BMPReader::printV3Fields()
{
	printf("\tHeader size = %d\n", info_header.info_size);
	printf("\tImage width = %d\n", info_header.pic_width);
	printf("\tImage height = %d\n", info_header.pic_height);
	printf("\tPlanes = %d\n", info_header.planes);
	printf("\tBit count = %d\n", info_header.bit_count);
	printf("\tCompression = %d\n", info_header.compression);
	printf("\tImage size = %d\n", info_header.size_image);
	printf("\tX PPM = %d\n", info_header.x_pixels_per_meter);
	printf("\tY PPM = %d\n", info_header.y_pixels_per_meter);
	printf("\tColors used = %d\n", info_header.colors_used);
	printf("\tColors important = %d\n", info_header.colors_important);
}

void BMPReader::printV4Fields()
{
	printf("\tRed mask = %08X\n", info_header.v4_red_mask);
	printf("\tGreen mask = %08X\n", info_header.v4_green_mask);
	printf("\tBlue mask = %08X\n", info_header.v4_blue_mask);
	printf("\tAlpha mask = %08X\n", info_header.v4_alpha_mask);
	printf("\tCS type = %X\n", info_header.v4_cs_type);
}

void BMPReader::printV5Fields()
{
	printf("\tIntent = %d\n", info_header.v5_intent);
	printf("\tProfile data offset = %d\n", info_header.v5_profile_data);
	printf("\tProfile data length = %d\n", info_header.v5_profile_size);
	printf("\tReserved = %d\n", info_header.v5_reserved);
}

void BMPReader::printHeaders()
{
	printf("\n***************FILE HEADERS***************\n");
	printFileHeader();
	switch (info_header_version)
	{
	case CORE:
		printf("CORE version not supported!\n");
		break;
	case VERSION_3:
		printf("Version 3 info header:\n");
		printV3Fields();
		break;
	case VERSION_4:
		printf("Version 4 info header:\n");
		printV3Fields();
		printV4Fields();
		break;
	case VERSION_5:
		printf("Version 5 info header:\n");
		printV3Fields();
		printV4Fields();
		printV5Fields();
		break;
	default:
		printf("Invalid header version: %d", info_header_version);
		break;
	}
	printf("******************************************\n\n");
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
	recalculateHeaders(new_width, new_height);
}

void BMPReader::addFrameTrueColor(int width)
{
	long new_width = info_header.pic_width + 2 * width;
	long new_height = info_header.pic_height + 2 * width;
	int pixel_size = info_header.bit_count / 8;
	unsigned int new_img_size = new_width * new_height * pixel_size;
	printf("New image size:\n");
	printf("\tWidth = %d\n", new_width);
	printf("\tHeight = %d\n", new_height);
	printf("\tSize = %d\n", new_img_size);
	unsigned char* new_pixels = new unsigned char[new_img_size];
	memset(new_pixels, 0, new_img_size);
	unsigned int offset_new = 0;
	unsigned int offset_old = 0;
	// top edge
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < new_width; j++) {
			for (int k = 0; k < pixel_size; k++) {
				new_pixels[offset_new++] = rand() % 256;
			}
		}
	}
	// copy old pixels to new array
	for (int i = 0; i < info_header.pic_height; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < pixel_size; k++) {
				new_pixels[offset_new++] = rand() % 256;
			}
		}
		for (int j = 0; j < info_header.pic_width; j++) {
			for (int k = 0; k < pixel_size; k++) {
				new_pixels[offset_new++] = pixels[offset_old++];
			}
		}
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < pixel_size; k++) {
				new_pixels[offset_new++] = rand() % 256;
			}
		}
	}
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < new_width; j++) {
			for (int k = 0; k < pixel_size; k++) {
				new_pixels[offset_new++] = rand() % 256;
			}
		}
	}
	delete[] pixels;
	pixels = new_pixels;
	recalculateHeaders(new_width, new_height);
}

void BMPReader::recalculateHeaders(int new_width, int new_height)
{
	unsigned int new_byte_size = calcImageSize(new_width, new_height);
	info_header.pic_width = new_width;
	info_header.pic_height = new_height;
	bm_header.file_size += new_byte_size - info_header.size_image;
	info_header.size_image = new_byte_size;
}

bool BMPReader::isTrueColor()
{
	return info_header.bit_count >= 24;
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
	printf("Read image from file %s\n", path);
	FILE* file;
	file = fopen(path, "rb");
	if (!readFileHeaders(file)) {
		printf("Couldn't read BMP headers\n");
		return false;
	}
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
		case 32:
			printf("32-bit format\n");
			break;
		default:
			printf("Only 8-bit, 24-bit and 32-bit formats are supported!\n");
			return false;
	}
	if (!readPixels(file)) {
		return false;
	}
	fclose(file);
	img_loaded = true;
	printf("Image loaded from %s\n\n", path);
	return true;
}

void BMPReader::writeFile(const char* path)
{
	printf("Write image to file %s\n", path);
	if (!img_loaded) {
		printf("Nothing to write!\n");
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
	printf("Image saved to %s\n\n", path);
}

void BMPReader::convertToGrayscale()
{
	printf("Convert to grayscale\n");
	for (int i = 0; i < palette_size; i++) {
		int average = (palette[i].blue + palette[i].green + palette[i].red) / 3;
		palette[i].blue = palette[i].green = palette[i].red = average;
	}
}

void BMPReader::addFrame(int width)
{
	printf("Add frame with width = %d\n", width);
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
	printf("Rotate image by 90 degrees\n");
	if (!img_loaded) {
		printf("Image not loaded!\n");
		return;
	}
	int new_width = info_header.pic_height;
	int new_height = info_header.pic_width;
	int pixel_size = info_header.bit_count / 8;
	int new_img_size = new_width * new_height * pixel_size;
	unsigned char* new_pixels = new unsigned char[new_img_size];
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
	recalculateHeaders(new_width, new_height);
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
	printf("Scale image by %.3f\n", factor);
	long new_width = info_header.pic_width * factor;
	long new_height = info_header.pic_height * factor;
	int pixel_size = info_header.bit_count / 8;
	unsigned int new_img_size = new_width * new_height * pixel_size;
	printf("New image size:\n");
	printf("\tWidth = %d\n", new_width);
	printf("\tHeight = %d\n", new_height);
	printf("\tSize = %d\n", new_img_size);
	unsigned char* new_pixels = new unsigned char[new_img_size];
	int byte_width_old = info_header.pic_width * pixel_size;
	int byte_width_new = new_width * pixel_size;
	memset(new_pixels, 0, new_img_size);
	for (int i = 0; i < new_height; i++) {
		for (int j = 0; j < new_width; j++) {
			int from = (int)(i / factor) * byte_width_old + (int)(j / factor) * pixel_size;
			int to = i * byte_width_new + j * pixel_size;
			for (int k = 0; k < pixel_size; k++) {
				new_pixels[to + k] = pixels[from + k];
			}
		}
	}
	delete[] pixels;
	pixels = new_pixels;
	recalculateHeaders(new_width, new_height);
}

void BMPReader::addLogo(BMPReader& logo_reader, int x, int y, float transparency)
{
	printf("Add logo on x = %d, y = %d with transparency set to %.2f\n", x, y, transparency);
	if (logo_reader.info_header.pic_width > info_header.pic_width ||
		logo_reader.info_header.pic_height > info_header.pic_height) {
		printf("Logo size is bigger than this picture size\n");
		return;
	}
	if (x + logo_reader.info_header.pic_width > info_header.pic_width ||
		y + logo_reader.info_header.pic_height > info_header.pic_height) {
		printf("Cannot add logo on x = %d, y = %d, not enough space\n", x, y);
		return;
	}
	if (!isTrueColor()) {
		printf("Logo can be added only to TrueColor BMPs\n");
		return;
	}
	if (!logo_reader.isTrueColor()) {
		printf("Logo must be in TrueColor format\n");
		return;
	}
	if (transparency < 0.1 || transparency > 0.9) {
		printf("Transparency must be in range [0.1; 0.9]\n");
		return;
	}
	int pixel_size = info_header.bit_count / 8;
	int pixel_size_logo = logo_reader.info_header.bit_count / 8;
	int pixel_size_min = pixel_size < pixel_size_logo ? pixel_size : pixel_size_logo;
	int byte_width = info_header.pic_width * pixel_size;
	int byte_width_logo = logo_reader.info_header.pic_width * pixel_size_logo;
	for (int i = 0; i < logo_reader.info_header.pic_height; i++) {
		for (int j = 0; j < logo_reader.info_header.pic_width; j++) {
			int from = i * byte_width_logo + j * pixel_size_logo;
			int to = (i + y) * byte_width + (j + x) * pixel_size;
			for (int k = 0; k < pixel_size_min; k++) {
				float factor = (float)logo_reader.pixels[from + 3] / 255 * (1 - transparency);
				int pic_channel = pixels[to + k] * (1 - factor);
				int logo_channel = logo_reader.pixels[from + k] * (factor);
				pixels[to + k] = pic_channel + logo_channel;
			}
		}
	}
}

unsigned char replace_bits(unsigned char dest, unsigned char src, unsigned char mask) {
	unsigned char res = dest & ~mask;
	src &= mask;
	res |= src;
	return res;
}

void BMPReader::encode_file(const char* path, percent_t encoding_size)
{
	printf("Encode file %s into image\n", path);
	FILE* file = fopen(path, "rb");
	if (file == NULL) {
		printf("Failed to open file %s\n", path);
		return;
	}
	int bytes_data, mask;
	int bytes_image = 4;
	unsigned int buf_size = info_header.size_image * encoding_size / 100;
	unsigned char* buffer = new unsigned char[buf_size];
	unsigned int data_size = fread(buffer, 1, buf_size, file);
	printf("Encode %d bytes of data\n", data_size);
	unsigned int pixels_offset = 0;
	if (data_size == buf_size && !feof(file)) {
		printf("%d bytes of data will be encoded in image, but file has more data, so this data will be lost."
			"Do you wish to continue? y/n [n]: ", data_size);
		char choice;
		scanf_s("%c", &choice);
		if (choice != 'y')
			goto EXIT_LABEL;
	}
	// Bits of data will be encoded in image like this:
	// xxxxxx11 xxxxxx11 xxxxxx11 xxxxxx11 - 25%
	// xxxx1122 xxxx1122 xxxx1122 xxxx1122 - 50%
	// xx112233 xx112233 xx112233 xx112233 - 75%
	// where x - bits of image, numbers - bits of corresponding bytes of data
	switch (encoding_size)
	{
	case PERCENT_25:
		bytes_data = 1;
		mask = 0x03;
		break;
	case PERCENT_50:
		bytes_data = 2;
		mask = 0x0f;
		break;
	case PERCENT_75:
		bytes_data = 3;
		mask = 0x3f;
		break;
	default:
		printf("Invalid encoding size: %d\n", encoding_size);
		goto EXIT_LABEL;
	}
	// Encode actual data size
	for (int i = 0; i < sizeof(data_size); i++) {
		unsigned char byte = (data_size >> (i * 8)) & 0xff;
		for (int j = 0; j < 4; j++) {
			pixels[pixels_offset] = replace_bits(pixels[pixels_offset], byte >> (j * 2), 0x03);
			pixels_offset++;
		}
	}
	// Encode data in least significant bits of image
	for (int i = 0; i < data_size; i += bytes_data) {
		unsigned char* bytes = &buffer[i];
		for (int j = 0; j < bytes_image; j++) {
			unsigned char bits = 0;
			for (int k = 0; k < bytes_data; k++) {
				bits = bits << 2 | (bytes[k] >> (j * 2)) & 0x03;
			}
			pixels[pixels_offset] = replace_bits(pixels[pixels_offset], bits, mask);
			pixels_offset++;
		}
	}
	bm_header.res1 = ENCODED;
	bm_header.res2 = encoding_size;
EXIT_LABEL:
	delete[] buffer;
	fclose(file);
}

void BMPReader::decode_to_file(const char* path)
{
	printf("Try decode information to %s\n", path);
	if (bm_header.res1 != ENCODED) {
		printf("Nothing encoded here\n");
		return;
	}
	FILE* file = fopen(path, "wb");
	if (file == NULL) {
		printf("Failed to open file %s\n", path);
		return;
	}
	unsigned short encoding_size = bm_header.res2;
	unsigned int data_size = 0;
	unsigned int pixels_offset = 0;
	unsigned char* buffer;
	int bytes_data, mask;
	int bytes_image = 4;
	switch (encoding_size)
	{
	case 25:
		bytes_data = 1;
		mask = 0x03;
		break;
	case 50:
		bytes_data = 2;
		mask = 0x0f;
		break;
	case 75:
		bytes_data = 3;
		mask = 0x3f;
		break;
	default:
		printf("Invalid encoding size: %d\n", encoding_size);
		goto EXIT_LABEL;
	}
	// Decode data size
	for (int i = 0; i < sizeof(data_size); i++) {
		unsigned int byte = 0;
		unsigned char mask = 0x03;
		for (int j = 0; j < 4; j++) {
			unsigned char bits = pixels[pixels_offset++] & mask;
			byte |= bits << (j * 2);
		}
		data_size |= byte << (i * 8);
	}
	printf("Restored data_size = %d\n", data_size);
	buffer = new unsigned char[data_size];
	memset(buffer, 0, data_size);
	// Each 3 bytes of data encoded in 4 bytes of image, 2 bits per each byte, 6 total
	for (int i = 0; i < data_size; i += bytes_data) {
		unsigned char* bytes = &buffer[i];
		//unsigned char mask = 0x3f;
		for (int j = 0; j < bytes_image; j++) {
			unsigned char bits = pixels[pixels_offset] & mask;
			for (int k = 0; k < bytes_data; k++) {
				unsigned char bits_k = (bits >> (bytes_data - k - 1) * 2) & 0x03;
				if (i + k < data_size) {
					bytes[k] |= bits_k << (j * 2);
				}
			}
			pixels_offset++;
		}
	}
	fwrite(buffer, 1, data_size, file);
	delete[] buffer;
EXIT_LABEL:
	fclose(file);
}

BMPReader::~BMPReader()
{
	if (palette != NULL)
		delete[] palette;
	if (pixels != NULL)
		delete[] pixels;
}
