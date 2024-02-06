#include "BMPReader.h"

int main() {
    BMPReader reader;
    reader.readFile("CAT256.bmp");
    reader.convertToGrayscale();
    reader.writeFile("GreyCat.bmp");
}
