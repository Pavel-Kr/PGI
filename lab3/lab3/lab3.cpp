#include "BMPReader.h"

int main()
{
    srand(time(0));
    BMPReader reader;
    reader.readFile("_сarib_TC.bmp");
    reader.rotate90Degrees();
    reader.writeFile("RotatedFish.bmp");
}