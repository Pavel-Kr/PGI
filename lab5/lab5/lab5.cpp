#include "BMPReader.h"

int main()
{
    BMPReader reader;
    reader.readFile("CAT256.bmp");
    //reader.readFile("_сarib_TC.bmp");
    reader.scale(2);
    reader.writeFile("ScaledCat.bmp");
}
