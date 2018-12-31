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
#include "opengl/camera.h"
#include "opengl/mesh.h"
#include "opengl/texture.h"
#include "opengl/program.h"
#include "opengl/screenshot.h"
#include "core/application.h"
#include "core/clock.h"
#include "core/randomization.h"
#include "core/threads.h"
#include "core/utilities.h"

#include "canvas.h"
#include "turtle.h"
#include "lsystem.h"
#include "examples.h"

/*
	Program configurations
*/
static const bool WINDOW_VSYNC = false;
static const int WINDOW_FULLSCREEN = 0;
static const int WINDOW_WIDTH = 640;
static const int WINDOW_HEIGHT = 480;
static const float CAMERA_FOV = 90.0f;
static const float WINDOW_RATIO = WINDOW_WIDTH / float(WINDOW_HEIGHT);

/*
	Application
*/
int main()
{
	InitializeApplication(ApplicationSettings{
		WINDOW_VSYNC, WINDOW_FULLSCREEN, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_RATIO
	});

	UniformRandomGenerator uniformGenerator;
	ApplicationClock clock;

	OpenGLWindow window;
	window.SetTitle("Plant Generation");
	window.SetClearColor(0.0, 0.0, 0.0, 1.0f);

	GLuint defaultVao = 0;
	glGenVertexArrays(1, &defaultVao);
	glBindVertexArray(defaultVao);

	Camera camera;
	camera.position = {0.0f, 0.0f, 5.0f};

	GLCube cube;
	GLTexturedProgram cubeShader;

	double lastScreenUpdate = clock.time;
	bool quit = false;
	int startX = 0;
	int startY = 0;
	while (!quit)
	{
		clock.Tick();
		window.SetTitle("Time: " + TimeString(clock.time) + ", FPS: " + FpsString(clock.deltaTime));

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
			{
				quit = true;
				break;
			}
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;
				case SDLK_s:
					TakeScreenshot("screenshot.png", WINDOW_WIDTH, WINDOW_HEIGHT);
					break;
				}
			}
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEMOTION:
			{
				//SDL_MouseButtonEvent& b = event.button;
				//if (b.button == SDL_BUTTON_LEFT) 
				//{
				//	int mouseWindowX = 0;
				//	int mouseWindowY = 0;
				//	SDL_GetMouseState(&mouseWindowX, &mouseWindowY);

				//	int canvasMouseX = mouseWindowX - 50;
				//	int canvasMouseY = mouseWindowY - 50;
				//	canvas.DrawLine(glm::ivec2{ startX, startY }, glm::ivec2{ canvasMouseX, canvasMouseY }, Color{ 0,0,0,255 });
				//	
				//	startX = canvasMouseX;
				//	startY = canvasMouseY;
				//}
			}
			default:
			{

			}
			}
		}

		window.SetClearColor((sinf(float(clock.time))+1.0f)/2.0f, 0.0, 0.0, 1.0f);
		window.Clear();

		glm::mat4 model = glm::mat4(1.0f);
		camera.position = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(45.0f) * float(clock.deltaTime), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camera.position, 1.0f));
		cubeShader.UpdateMVP(camera.ViewProjectionMatrix() * model);
		cubeShader.Use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		cube.Draw();

		window.SwapFramebuffer();
	}

	return 0;
}
