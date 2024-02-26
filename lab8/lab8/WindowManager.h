#pragma once
#include <SDL.h>
#include "BMPReader.h"

class WindowManager
{
	SDL_Surface* screen_surface;
	SDL_Window* window;
public:
	WindowManager();
	int initWindow(const char* title, int width, int height);
	void setBitMap(BMPReader& reader);
	void render();
	~WindowManager();
};

