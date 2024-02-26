#include "WindowManager.h"
#include "PCXReader.h"

int main(int argc, char **argv) {
	PCXReader reader;
	reader.readFile("CAT256.pcx");
	/*BMPReader reader;
	if (!reader.readFile("_сarib_TC.bmp")) {
		return 1;
	}
	WindowManager manager;
	if (manager.initWindow("Lab8", reader.getImageWidth(), reader.getImageHeight()) != 0) {
		return 1;
	}
	manager.setBitMap(reader);
	SDL_Event event;
	bool run = true;
	while (run)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT) {
				run = false;
			}
		}
		manager.render();
	}*/
	return 0;
}