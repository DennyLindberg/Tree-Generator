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
#include "opengl/grid.h"
#include "core/application.h"
#include "core/clock.h"
#include "core/randomization.h"
#include "core/threads.h"
#include "core/utilities.h"

#include "canvas.h"
#include "turtle.h"
#include "lsystem.h"
#include "examples.h"
#include "input.h"

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
	window.SetClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	GLuint defaultVao = 0;
	glGenVertexArrays(1, &defaultVao);
	glBindVertexArray(defaultVao);

	Camera camera;
	TurntableController turntable(camera);
	turntable.sensitivity = 0.25f;
	turntable.Set(0.0f, 45.0f, 5.0f);

	GLCube cube;
	GLTexturedProgram cubeShader;

	GLGrid grid;
	grid.size = 20.0f;
	grid.gridSpacing = 0.5f;

	double lastScreenUpdate = clock.time;
	bool quit = false;
	bool captureMouse = false;
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
				{
					quit = true;
					break;
				}
				case SDLK_s:
				{
					TakeScreenshot("screenshot.png", WINDOW_WIDTH, WINDOW_HEIGHT);
					break;
				}
				case SDLK_f:
				{
					turntable.SnapToOrigin();
					break;
				}
				}
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				captureMouse = true;
				SDL_ShowCursor(0);
				SDL_SetRelativeMouseMode(SDL_TRUE);
				switch (event.button.button)
				{
				case SDL_BUTTON_RIGHT:
				{
					turntable.inputState = TurntableInputState::Zoom;
					break;
				}
				case SDL_BUTTON_MIDDLE:
				{
					turntable.inputState = TurntableInputState::Translate;
					break;
				}
				case SDL_BUTTON_LEFT:
				default:
				{
					turntable.inputState = TurntableInputState::Rotate;
					break;
				}
				}
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				captureMouse = false;
				SDL_ShowCursor(1);
				SDL_SetRelativeMouseMode(SDL_FALSE);
				break;
			}
			case SDL_MOUSEMOTION:
			{
				if (captureMouse)
				{
					turntable.ApplyMouseInput(-event.motion.xrel, event.motion.yrel);
				}
				break;
			}
			default:
			{
				break;
			}
			}
		}

		window.Clear();

		glm::mat4 mvp = camera.ViewProjectionMatrix();

		cubeShader.UpdateMVP(mvp);
		cubeShader.Use();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		cube.Draw();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		grid.Draw(mvp);
		window.SwapFramebuffer();
	}

	return 0;
}
