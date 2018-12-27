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
#include "opengl/mesh.h"
#include "opengl/texture.h"
#include "opengl/program.h"
#include "opengl/screenshot.h"
#include "core/clock.h"
#include "core/randomization.h"
#include "core/threads.h"
#include "core/utilities.h"

/*
	Program configurations
*/
static const bool SCREEN_VSYNC = false;
static const unsigned int SCREEN_FULLSCREEN = 0;
static const unsigned int SCREEN_WIDTH = 640;
static const unsigned int SCREEN_HEIGHT = 480;
static const float CAMERA_FOV = 90.0f;

/*
	Application
*/
int main()
{
	UniformRandomGenerator uniformGenerator;
	ApplicationClock clock;

	OpenGLWindow window(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_FULLSCREEN, SCREEN_VSYNC);
	window.SetTitle("Plant Generation");
	window.SetClearColor(0.0, 0.0, 0.0, 1.0f);

	GLuint defaultVao = 0;
	glGenVertexArrays(1, &defaultVao);
	glBindVertexArray(defaultVao);

	GLTexture texture{int(SCREEN_WIDTH), int(SCREEN_HEIGHT)};
	texture.FillDebug();
	texture.SendToGPU();
	texture.UseForDrawing();
	GLQuad fullscreenQuad{};

	GLTexturedProgram shaderProgram{};

	double lastScreenUpdate = clock.time;
	bool quit = false;
	while (!quit)
	{
		clock.Tick();
		window.SetTitle("Time: " + TimeString(clock.time) + ", FPS: " + FpsString(clock.deltaTime));

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

		window.SetClearColor((sinf(float(clock.time))+1.0f)/2.0f, 0.0, 0.0, 1.0f);
		window.Clear();
		shaderProgram.Use();
		fullscreenQuad.Draw();
		window.SwapFramebuffer();
	}

	return 0;
}
