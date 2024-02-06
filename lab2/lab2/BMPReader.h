#pragma once
#define _CRT_SECURE_NO_WARNINGS
#pragma pack(1)
#include <stdio.h>
#include <cstdlib>
#include <ctime>
typedef struct {
    unsigned short type;
    unsigned int file_size;
    unsigned short res1;
    unsigned short res2;
    unsigned int bits_offset;
} bitmapfileheader;

typedef struct {
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
} bitmapinfoheader;

typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char reserved;
} RGBQuad;

class BMPReader
{
    bitmapfileheader bm_header;
    bitmapinfoheader info_header;
    RGBQuad* palette;
    int palette_size;
    unsigned char* pixels;
    unsigned int img_size;
    bool img_loaded;

    void readFileHeaders(FILE* file);
    bool readPalette(FILE* file);
    bool readPixels(FILE* file);
    long calcPaddedWidth(int width);
    unsigned int calcImageSize(int width, int height);
    void printHeaders();
    void addFramePalette(int width);
    void addFrameTrueColor(int width);
public:
    BMPReader();
    bool readFile(const char* path);
    void writeFile(const char* path);
    void convertToGrayscale();
    void addFrame(int width = 15);
    ~BMPReader();
};

