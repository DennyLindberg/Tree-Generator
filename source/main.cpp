#define USE_MULTITHREADING false

// STL includes
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

// Application includes
#include "opengl/window.h"
#include "opengl/data.h"
#include "opengl/screenshot.h"
#include "core/clock.h"
#include "core/randomization.h"
#include "core/threads.h"


/*
	Program configurations
*/
static const bool SCREEN_VSYNC = false;
static const unsigned int SCREEN_FULLSCREEN = 0;
static const unsigned int SCREEN_WIDTH = 640;
static const unsigned int SCREEN_HEIGHT = 480;
static const float CAMERA_FOV = 90.0f;


UniformRandomGenerator uniformGenerator;
std::string TimeString(double time)
{
	int seconds = int(time);
	int minutes = seconds / 60;
	int hours = minutes / 60;
	double decimals = time - double(seconds);

	return std::to_string(hours) + "h " + std::to_string(minutes % 60) + "m " + std::to_string(seconds % 60) + "." + std::to_string(int(decimals*10.0)) + "s";
}
std::string FpsString(double deltaTime)
{ 
	if (deltaTime == 0)
	{
		return "Inf";
	}
	else
	{
		return std::to_string(int(round(1.0 / deltaTime))); 
	}
}

int main()
{
	OpenGLWindow window(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_FULLSCREEN, SCREEN_VSYNC);
	window.SetTitle("Plant Generation");
	window.SetClearColor(0.0, 0.0, 0.0, 1.0f);

	ApplicationClock clock;

	double lastScreenUpdate = clock.time;
	bool quit = false;
	while (!quit)
	{
		clock.Tick();

		double screenUpdateDelta = clock.time - lastScreenUpdate;
		window.SetTitle("Time: " + TimeString(clock.time) + ", FPS: " + FpsString(screenUpdateDelta));
		lastScreenUpdate = clock.time;

		window.SetClearColor((sinf(float(clock.time))+1.0f)/2.0f, 0.0, 0.0, 1.0f);
		window.Clear();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;
				case SDLK_s:
					TakeScreenshot("screenshot.png", SCREEN_WIDTH, SCREEN_HEIGHT);
					break;
				}
			}
		}

		window.SwapFramebuffer();
	}

	return 0;
}
