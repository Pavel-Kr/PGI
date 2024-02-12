#include "WindowManager.h"

int main(int argc, char** args) {
	BMPReader reader;
	reader.readFile("CAT256.bmp");
	WindowManager manager;
	if (manager.initWindow("Lab4", reader.getImageWidth(), reader.getImageHeight()) != 0) {
		return 1;
	}
	manager.setBitMap(reader);
	SDL_Event event;
	bool run = true;
	while (run) {
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT)
				run = false;
		}
		manager.render();
	}
	return 0;
}