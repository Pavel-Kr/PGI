#include "BMPReader.h"

int main() {
	BMPReader picture, logo;
	picture.readFile("_сarib_TC.bmp");
	logo.readFile("logo.bmp");
	logo.scale(0.7);
	int x = 20;
	int y = picture.getImageHeight() - logo.getImageHeight() - 20;
	picture.addLogo(logo, x, y, 0.3);
	picture.writeFile("SibGuchiFish.bmp");
}
