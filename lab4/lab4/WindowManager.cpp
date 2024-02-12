#include "WindowManager.h"

WindowManager::WindowManager()
{
	screen_surface = NULL;
	window = NULL;
}

WindowManager::~WindowManager()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void WindowManager::render()
{
	//SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, 20, 20, 20));
	SDL_UpdateWindowSurface(window);
}

void WindowManager::setBitMap(BMPReader& reader)
{
	unsigned char* pixels = (unsigned char*)screen_surface->pixels;
	for (int i = 0; i < reader.getImageHeight(); i++) {
		for (int j = 0; j < reader.getImageWidth(); j++) {
			// invert y-axis because image is vertically flipped
			RGBQuad color = reader.getPixelColor(j, reader.getImageHeight() - i - 1);
			int index = i * screen_surface->pitch + j * screen_surface->format->BytesPerPixel;
			pixels[index++] = color.blue;
			pixels[index++] = color.green;
			pixels[index++] = color.red;
			pixels[index++] = color.reserved;
		}
	}
}

int WindowManager::initWindow(const char* title, int width, int height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		return 1;
	}

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

	if (window == NULL) {
		return 1;
	}

	screen_surface = SDL_GetWindowSurface(window);

	return 0;
}
