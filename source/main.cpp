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
	GLProgram liveReloadShader;
	GLProgram lineShader;
	std::string fragment, vertex;
	if (LoadText(contentFolder/"basic_fragment.glsl", fragment) && LoadText(contentFolder/"basic_vertex.glsl", vertex))
	{
		liveReloadShader.LoadFragmentShader(fragment);
		liveReloadShader.LoadVertexShader(vertex);
		liveReloadShader.CompileAndLink();
	}
	if (LoadText(contentFolder / "line_fragment.glsl", fragment) && LoadText(contentFolder / "line_vertex.glsl", vertex))
	{
		lineShader.LoadFragmentShader(fragment);
		lineShader.LoadVertexShader(vertex);
		lineShader.CompileAndLink();
	}

	GLGrid grid;
	grid.size = 20.0f;
	grid.gridSpacing = 0.5f;

	FileListener fileListener{ contentFolder };
	fileListener.Bind(
		L"basic_fragment.glsl", 
		[&liveReloadShader](fs::path filePath) -> void {
			wprintf(L"\r\nFragment shader changed: %Ls\r\n", filePath.c_str());
			std::string content;
			if (LoadText(filePath, content))
			{
				printf("\r\n=======\r\n%s\r\n=======\r\n\r\n", content.c_str());
				liveReloadShader.LoadFragmentShader(content);
				liveReloadShader.CompileAndLink();
			}
			else
			{
				printf("\r\nFailed to read file");
			}
		}
	);
	fileListener.Bind(
		L"basic_vertex.glsl",
		[&liveReloadShader](fs::path filePath) -> void {
			wprintf(L"\r\nVertex shader changed: %Ls\r\n", filePath.c_str());
			std::string content;
			if (LoadText(filePath, content))
			{
				printf("\r\n=======\r\n%s\r\n=======\r\n\r\n", content.c_str());
				liveReloadShader.LoadVertexShader(content);
				liveReloadShader.CompileAndLink();
			}
			else
			{
				printf("\r\nFailed to read file");
			}
		}
	);
	fileListener.StartThread();


	/*
		Build mesh using Turtle
	*/
	struct TreeContext
	{
		int x;
	};
	using Turtle = Turtle3D<TreeContext>;
	using TurtleState = TurtleState3D<TreeContext>;

	float scale = 0.1f;
	Turtle turtle;
	LSystemString fractalTree;
	fractalTree.axiom = "0";
	fractalTree.productionRules['0'] = "1[0]0";
	fractalTree.productionRules['1'] = "11";

	turtle.actions['0'] = [scale, &uniformGenerator](Turtle& t, int repetitions) {
		float forwardGrowth = 0.0f;
		while (--repetitions >= 0)
		{
			forwardGrowth += uniformGenerator.RandomFloat();
		}

		t.MoveForward(scale*forwardGrowth);
	};
	turtle.actions['1'] = turtle.actions['0'];
	turtle.actions['['] = [scale, &uniformGenerator](Turtle& t, int repetitions) {
		t.PushState();
		t.Rotate(180.0f*uniformGenerator.RandomFloat(0.1f, 1.0f), 
				 45.0f*uniformGenerator.RandomFloat(0.2f, 1.0f));
	};
	turtle.actions[']'] = [scale, &uniformGenerator](Turtle& t, int repetitions) {
		t.PopState();
		t.Rotate(-180.0f*uniformGenerator.RandomFloat(0.1f, 1.0f),
				  45.0f*uniformGenerator.RandomFloat(0.2f, 1.0f));
	};

	std::string growth = fractalTree.RunProduction(8);
	turtle.GenerateSkeleton(growth);

	GLLine skeleton;
	float counter = 0;
	turtle.ForEachBone([&counter, &skeleton](std::pair<TurtleState, TurtleState>& bone) -> void {
		skeleton.AddLine(bone.first.position, bone.second.position, glm::fvec4(0.0f, 1.0f, 0.0f, 1.0f));
		skeleton.AddLine(bone.first.position, bone.first.position+bone.second.sideDirection*0.1f, glm::fvec4(1.0f, 0.0f, 0.0f, 1.0f));
	});
	skeleton.SendToGPU();

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

		//liveReloadShader.UpdateMVP(mvp);
		//liveReloadShader.Use();
		//cube.Draw();

		lineShader.UpdateMVP(mvp);
		lineShader.Use();
		skeleton.Draw();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		grid.Draw(mvp);
		window.SwapFramebuffer();
	}

	return 0;
}
