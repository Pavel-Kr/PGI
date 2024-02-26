#include "BMPReader.h"

int main() {
	BMPReader normal;
	normal.readFile("_сarib_TC.bmp");
	normal.encode_file("top_secret.txt", PERCENT_25);
	normal.writeFile("normal_image.bmp");

	BMPReader encoded;
	encoded.readFile("normal_image.bmp");
	encoded.decode_to_file("secret_revealed.txt");
}
