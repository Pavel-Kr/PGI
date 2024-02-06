#include "BMPReader.h"

int main()
{
    srand(time(0));
    BMPReader reader;
    reader.readFile("_сarib_TC.bmp");
    reader.addFrame();
    reader.writeFile("FrameFish.bmp");
}
