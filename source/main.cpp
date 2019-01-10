#define USE_MULTITHREADING false

// STL includes
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <functional>

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
#include "core/filelistener.h"


#include "canvas.h"
#include "turtle3d.h"
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

namespace fs = std::filesystem;

/*
	Application
*/
int main()
{
	fs::path contentFolder = fs::current_path().parent_path() / "content";
	InitializeApplication(ApplicationSettings{
		WINDOW_VSYNC, WINDOW_FULLSCREEN, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_RATIO, contentFolder
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

	GLTexture defaultTexture{contentFolder / "default.png"};
	defaultTexture.UseForDrawing();

	GLCube cube;
	GLProgram liveShader;
	std::string fragment, vertex;
	if (LoadText(contentFolder/"basic_fragment.glsl", fragment) && LoadText(contentFolder/"basic_vertex.glsl", vertex))
	{
		liveShader.LoadFragmentShader(fragment);
		liveShader.LoadVertexShader(vertex);
		liveShader.CompileAndLink();
	}

	GLGrid grid;
	grid.size = 20.0f;
	grid.gridSpacing = 0.5f;

	FileListener fileListener{ contentFolder };
	fileListener.Bind(
		L"basic_fragment.glsl", 
		[&liveShader](fs::path filePath) -> void {
			wprintf(L"\r\nFragment shader changed: %Ls\r\n", filePath.c_str());
			std::string content;
			if (LoadText(filePath, content))
			{
				printf("\r\n=======\r\n%s\r\n=======\r\n\r\n", content.c_str());
				liveShader.LoadFragmentShader(content);
				liveShader.CompileAndLink();
			}
			else
			{
				printf("\r\nFailed to read file");
			}
		}
	);
	fileListener.Bind(
		L"basic_vertex.glsl",
		[&liveShader](fs::path filePath) -> void {
			wprintf(L"\r\nVertex shader changed: %Ls\r\n", filePath.c_str());
			std::string content;
			if (LoadText(filePath, content))
			{
				printf("\r\n=======\r\n%s\r\n=======\r\n\r\n", content.c_str());
				liveShader.LoadVertexShader(content);
				liveShader.CompileAndLink();
			}
			else
			{
				printf("\r\nFailed to read file");
			}
		}
	);
	fileListener.StartThread();

	/*
		Main application loop
	*/
	double lastScreenUpdate = clock.time;
	bool quit = false;
	bool captureMouse = false;
	bool renderWireframe = false;
	while (!quit)
	{
		clock.Tick();
		SetThreadedTime(clock.time);
		window.SetTitle("Time: " + TimeString(clock.time) + ", FPS: " + FpsString(clock.deltaTime));
		fileListener.ProcessCallbacksOnMainThread();


		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			quit = (event.type == SDL_QUIT) || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE);
			
			if (event.type == SDL_KEYDOWN)
			{
				auto key = event.key.keysym.sym;

				if		(key == SDLK_4) renderWireframe = true;
				else if (key == SDLK_5) renderWireframe = false;
				else if (key == SDLK_s) TakeScreenshot("screenshot.png", WINDOW_WIDTH, WINDOW_HEIGHT);
				else if (key == SDLK_f) turntable.SnapToOrigin();
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				captureMouse = true;
				SDL_ShowCursor(0);
				SDL_SetRelativeMouseMode(SDL_TRUE);

				auto button = event.button.button;
				     if (button == SDL_BUTTON_LEFT)   turntable.inputState = TurntableInputState::Rotate;
				else if (button == SDL_BUTTON_MIDDLE) turntable.inputState = TurntableInputState::Translate;
				else if (button == SDL_BUTTON_RIGHT)  turntable.inputState = TurntableInputState::Zoom;
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				captureMouse = false;
				SDL_ShowCursor(1);
				SDL_SetRelativeMouseMode(SDL_FALSE);
			}
			else if (event.type == SDL_MOUSEMOTION && captureMouse)
			{
				turntable.ApplyMouseInput(-event.motion.xrel, event.motion.yrel);
			}
		}

		window.Clear();
		glPolygonMode(GL_FRONT_AND_BACK, (renderWireframe? GL_LINE : GL_FILL));

		glm::mat4 mvp = camera.ViewProjectionMatrix();

		liveShader.UpdateMVP(mvp);
		liveShader.Use();
		cube.Draw();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		grid.Draw(mvp);
		window.SwapFramebuffer();
	}

	return 0;
}
