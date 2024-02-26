#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <cstdlib>
#include <ctime>

#include "BaseReader.h"

#define ENCODED 17

#pragma pack(1)
typedef struct {
    unsigned short type;
    unsigned int file_size;
    unsigned short res1;
    unsigned short res2;
    unsigned int bits_offset;
} bitmapfileheader;

typedef struct {
    // Version 3 fields
    unsigned int info_size;
    long pic_width;
    long pic_height;
    unsigned short planes;
    unsigned short bit_count;
    unsigned int compression;
    unsigned int size_image;
    long x_pixels_per_meter;
    long y_pixels_per_meter;
    unsigned int colors_used;
    unsigned int colors_important;
    // Version 4 fields
    unsigned int v4_red_mask;
    unsigned int v4_green_mask;
    unsigned int v4_blue_mask;
    unsigned int v4_alpha_mask;
    unsigned int v4_cs_type;
    unsigned char v4_endpoints[36];
    unsigned int v4_gamma_red;
    unsigned int v4_gamma_green;
    unsigned int v4_gamma_blue;
    // Version 5 fields
    unsigned int v5_intent;
    unsigned int v5_profile_data;
    unsigned int v5_profile_size;
    unsigned int v5_reserved;
} bitmapinfoheader;

enum version_t {
    CORE,
    VERSION_3,
    VERSION_4,
    VERSION_5
};

enum percent_t {
    PERCENT_25 = 25,
    PERCENT_50 = 50,
    PERCENT_75 = 75
};

class BMPReader : BaseReader
{
    bitmapfileheader bm_header;
    bitmapinfoheader info_header;
    version_t info_header_version;
    RGBQuad* palette;
    int palette_size;
    unsigned char* pixels;
    bool img_loaded;

    bool readFileHeaders(FILE* file);
    bool readPalette(FILE* file);
    bool readPixels(FILE* file);
    long calcPaddedWidth(int width);
    unsigned int calcImageSize(int width, int height);
    void printFileHeader();
    void printV3Fields();
    void printV4Fields();
    void printV5Fields();
    void printHeaders();
    void addFramePalette(int width);
    void addFrameTrueColor(int width);
    void recalculateHeaders(int new_width, int new_height);
    bool isTrueColor();
public:
    BMPReader();
    bool readFile(const char* path);
    void writeFile(const char* path);
    void convertToGrayscale();
    void addFrame(int width = 15);
    void rotate90Degrees();
    long getImageWidth();
    long getImageHeight();
    RGBQuad getPixelColor(int x, int y);
    void scale(double factor);
    void addLogo(BMPReader& logo_reader, int x = 0, int y = 0, float transparency = 0.5);
    void encode_file(const char* path, percent_t encoding_size);
    void decode_to_file(const char* path);
    ~BMPReader();
};

