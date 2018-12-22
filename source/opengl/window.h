#pragma once

#include "SDL2/SDL.h"
#undef main

#include <string>

class OpenGLWindow
{
protected:
	SDL_GLContext maincontext;
	SDL_Window* window = nullptr;

	int screenWidth = 640;
	int screenHeight = 480;
	int fullscreen = 0;
	bool vsyncEnabled = false;

public:
	OpenGLWindow(int width, int height, bool fullscreenEnabled, bool vsync)
		: screenWidth { width}, screenHeight{ height }, 
		  fullscreen(fullscreenEnabled), 
		  vsyncEnabled{ vsync }
	{
		Initialize();
	}

	~OpenGLWindow() = default;

	void SetTitle(std::string newCaption);
	void SwapFramebuffer();
	void SetClearColor(float r, float g, float b, float a);
	void Clear();

protected:
	void Initialize();
};
