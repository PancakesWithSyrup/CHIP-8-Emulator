#pragma once
#include "SDL.h"

class Window {
public:
	Window(char const* title, const int windowHeight, const int windowWidth, int textureWidth, int textureHeight);
	~Window();

	void update(void const* buffer, int pitch);
	bool processInput(uint8_t* keys);
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
};

