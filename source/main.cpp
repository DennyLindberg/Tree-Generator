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


#include "canvas.h"
#include "turtle3d.h"
#include "lsystem.h"
#include "examples.h"
#include "input.h"
#include "shadermanager.h"

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
	turntable.position = glm::vec3{0.0f, 3.0f, 0.0f};
	turntable.sensitivity = 0.25f;
	turntable.Set(-45.0f, 25.0f, 7.0f);

	GLTexture defaultTexture{contentFolder / "default.png"};
	defaultTexture.UseForDrawing();

	GLGrid grid;
	grid.size = 20.0f;
	grid.gridSpacing = 0.5f;

	GLProgram defaultShader, lineShader;
	ShaderManager shaderManager;
	shaderManager.InitializeFolder(contentFolder);
	shaderManager.LoadLiveShader(defaultShader, L"basic_vertex.glsl", L"basic_fragment.glsl");
	shaderManager.LoadShader(lineShader, L"line_vertex.glsl", L"line_fragment.glsl");

	/*
		Build mesh using Turtle
	*/
	float scale = 0.05f;
	Turtle3D turtle;
	GLLine skeletonLines;
	GenerateFractalPlant3D(turtle, uniformGenerator, 8, 0.05f);
	turtle.BonesToGLLines(skeletonLines, glm::fvec4(0.0f, 1.0f, 0.0f, 1.0f), glm::fvec4(1.0f, 0.0f, 0.0f, 1.0f));

	/*
		Build leaf mesh and texture
	*/
	int leafTextureSize = 128;
	Color leafFillColor{ 0,200,0,255 };
	Color leafLineColor{ 0,100,0,255 };

	Canvas2D canvas{leafTextureSize, leafTextureSize};
	canvas.Fill(leafFillColor);
	std::vector<glm::fvec3> leafHull;
	DrawFractalLeaf(leafHull, canvas, leafLineColor, 6, 1.0f, glm::fvec2(leafTextureSize * 0.5, leafTextureSize), 90);

	glm::fvec2 previous = leafHull[0];
	for (int i = 1; i < leafHull.size(); i++)
	{
		canvas.DrawLine(leafHull[i - 1], leafHull[i], leafLineColor);
	}
	canvas.DrawLine(leafHull.back(), leafHull[0], leafLineColor);

	// Normalize leafHull dimensions to [0, 1.0]
	for (auto& h : leafHull)
	{
		h = h / float(leafTextureSize);
	}
	std::sort(leafHull.begin(), leafHull.end(), [](const glm::vec3 &v1, const glm::vec3 &v2)
	{
		return v1.y < v2.y;
	});

	/*
		Polygon mesh test
	*/
	GLTriangleMesh mesh;
	glm::fvec3 planeNormal{ 0.0f, 0.0f, 1.0f };
	mesh.AddVertex(
		{ 0.0f, 0.0f, 0.0f }, planeNormal,
		{ 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }
	);
	mesh.AddVertex(
		{ 1.0f, 0.0f, 0.0f }, planeNormal,
		{ 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 0.0f }
	);
	mesh.AddVertex(
		{ 1.0f, 1.0f, 0.0f }, planeNormal,
		{ 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 0.0f }
	);
	mesh.AddVertex(
		{ 0.0f, 1.0f, 0.0f }, planeNormal,
		{ 1.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }
	);
	mesh.DefineNewTriangle(0, 1, 2);
	mesh.DefineNewTriangle(2, 3, 0);
	mesh.SendToGPU();


	/*
		Main application loop
	*/
	bool quit = false;
	bool captureMouse = false;
	bool renderWireframe = false;
	while (!quit)
	{
		clock.Tick();
		SetThreadedTime(clock.time);
		window.SetTitle("Time: " + TimeString(clock.time) + ", FPS: " + FpsString(clock.deltaTime));
		shaderManager.CheckLiveShaders();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			quit = (event.type == SDL_QUIT) || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE);
			
			if (quit)
			{
				break;
			}

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

		glm::mat4 projection = camera.ViewProjectionMatrix();

		mesh.transform.scale = glm::fvec3{ float(sin(clock.time*2.0)) };

		mesh.transform.position.x = float(sin(clock.time));
		mesh.transform.position.y = float(sin(clock.time));

		mesh.transform.rotation.x = float(clock.time*90.0);
		mesh.transform.rotation.y = float(clock.time*180.0);
		mesh.transform.rotation.z = float(clock.time*360.0);

		defaultTexture.UseForDrawing();
		defaultShader.UpdateMVP(projection * mesh.transform.ModelMatrix());
		defaultShader.Use();
		mesh.Draw();

		lineShader.UpdateMVP(projection * mesh.transform.ModelMatrix());
		lineShader.Use();
		skeletonLines.Draw();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		grid.Draw(projection);
		canvas.RenderToScreen();
		window.SwapFramebuffer();
	}

	exit(0);
}
