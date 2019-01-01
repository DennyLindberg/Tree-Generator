#define USE_MULTITHREADING false

// STL includes
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <thread>

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

namespace fs = std::filesystem;
fs::path contentFolder = fs::current_path().parent_path() / "content";

/*
	File modify listener
*/
bool closeThread = false;
void ListenToFolderChange()
{
	auto notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE;
	HANDLE hDir = CreateFile(
		contentFolder.c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);

	if (hDir == INVALID_HANDLE_VALUE)
	{
		printf("\r\nInvalid handle");
		return;
	}

	BYTE buffer[4096];
	DWORD dwBytesReturned = 0;
	while (!closeThread)
	{
		bool retrievedChanges = ReadDirectoryChangesW(
			hDir,
			buffer, sizeof(buffer),
			FALSE,
			notifyFilter,
			&dwBytesReturned, NULL, NULL
		);

		if (!retrievedChanges)
		{
			printf("\r\nFile listener failed...");
			break;
		}

		BYTE* p = buffer;
		for (;;)
		{
			FILE_NOTIFY_INFORMATION* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(p);

			if (info->Action == FILE_ACTION_MODIFIED)
			{
				int stringLength = info->FileNameLength / sizeof(WCHAR);
				std::wstring filename = std::wstring((WCHAR*)&info->FileName, stringLength);
				wprintf(L"\r\nChanged: %Ls", filename.c_str());
			}

			if (!info->NextEntryOffset) break;
			p += info->NextEntryOffset;
		}
	}
}

/*
	Application
*/
int main()
{
	std::thread fileChangeThread = std::thread(ListenToFolderChange);

	//
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
	bool renderWireframe = false;
	while (!quit)
	{
		clock.Tick();
		window.SetTitle("Time: " + TimeString(clock.time) + ", FPS: " + FpsString(clock.deltaTime));

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

		cubeShader.UpdateMVP(mvp);
		cubeShader.Use();
		cube.Draw();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		grid.Draw(mvp);
		window.SwapFramebuffer();
	}

	closeThread = true;
	if (fileChangeThread.joinable())
	{
		fileChangeThread.join();
	}

	return 0;
}
