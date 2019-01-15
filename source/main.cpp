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

using BranchVector = std::vector<std::vector<Bone<FractalTree3DProps>*>>;
void BuildBranches(BranchVector& branches, Bone<FractalTree3DProps>* bone)
{
	using TBone = Bone<FractalTree3DProps>;
	using BoneVector = std::vector<TBone*>;

	branches.clear();
	branches.shrink_to_fit();
	branches.push_back({ bone });

	int activeIndex = 0;
	while (activeIndex < branches.size())
	{
		BoneVector& currentBranch = branches[activeIndex];
		TBone* firstBone = currentBranch[0];

		BoneVector potentials{ firstBone };
		TBone* lastChild = firstBone->lastChild;
		while (lastChild)
		{
			currentBranch.push_back(lastChild);
			potentials.push_back({ lastChild });
			lastChild = lastChild->lastChild;
		}

		for (TBone* p : potentials)
		{
			TBone* sibling = p->previousSibling;
			while (sibling)
			{
				branches.push_back({ sibling });
				sibling = sibling->previousSibling;
			}
		}

		activeIndex++;
	}
}

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

	GLProgram defaultShader, lineShader, leafShader;
	ShaderManager shaderManager;
	shaderManager.InitializeFolder(contentFolder);
	shaderManager.LoadLiveShader(defaultShader, L"basic_vertex.glsl", L"basic_fragment.glsl");
	shaderManager.LoadLiveShader(leafShader, L"basic_vertex.glsl", L"leaf_fragment.glsl");
	shaderManager.LoadShader(lineShader, L"line_vertex.glsl", L"line_fragment.glsl");

	/*
		Build mesh using Turtle
	*/
	Turtle3D turtle;
	GLLine skeletonLines;
	GenerateFractalTree3D(uniformGenerator, 10,
		[&skeletonLines](Bone<FractalTree3DProps>* bone) -> void
	{
		using TBone = Bone<FractalTree3DProps>;

		std::function<void(TBone*)> scaleBones = [](TBone* bone) -> void
		{
			float scale = 2.0f;
			bone->transform.position *= scale;
			bone->length *= scale;
		};
		bone->ForEach(scaleBones);

		BranchVector branches;
		BuildBranches(branches, bone);

		// TODO: Replace AddLine calls and build mesh instead
		for (int i = 0; i < branches.size(); ++i)
		{
			auto& branch = branches[i];
			for (TBone* b : branch)
			{
				skeletonLines.AddLine(
					b->transform.position,
					b->tipPosition(),
					glm::fvec4(0.0f, 1.0f, 0.0f, 1.0f)
				);

				skeletonLines.AddLine(
					b->transform.position,
					b->transform.position + b->transform.sideDirection*b->transform.properties.thickness,
					glm::fvec4(1.0f, 0.0f, 0.0f, 1.0f)
				);
			}
		}
	});
	skeletonLines.SendToGPU();

	/*
		Build leaf texture using turtle graphics
	*/
	int leafTextureSize = 128;
	Color leafFillColor{ 0,200,0,0 };
	Color leafLineColor{ 0,100,0,255 };

	Canvas2D leafCanvas{leafTextureSize, leafTextureSize};
	std::shared_ptr<GLTexture> leafTexture = leafCanvas.GetTexture();
	leafCanvas.Fill(leafFillColor);
	std::vector<glm::fvec3> leafHull;
	DrawFractalLeaf(leafHull, leafCanvas, leafLineColor, 6, 1.0f, glm::fvec2(leafTextureSize * 0.5, leafTextureSize), 90);

	glm::fvec2 previous = leafHull[0];
	for (int i = 1; i < leafHull.size(); i++)
	{
		leafCanvas.DrawLine(leafHull[i - 1], leafHull[i], leafLineColor);
	}
	leafCanvas.DrawLine(leafHull.back(), leafHull[0], leafLineColor);

	/*
		Create leaf mesh by converting turtle graphics to vertices and UV coordinates.
	*/
	GLTriangleMesh leafMesh;
	glm::fvec3 leafNormal{ 0.0f, 0.0f, 1.0f };

	// Normalize leafHull dimensions to [0, 1.0]
	for (auto& h : leafHull)
	{
		h = h / float(leafTextureSize);
	}
	for (glm::fvec3& p : leafHull)
	{
		leafMesh.AddVertex(
			{ p.x-0.5f, 1.0f-p.y, p.z}, // convert to world coordinate system
			leafNormal,
			{ 1.0f, 0.0f, 0.0f, 1.0f }, // vertex color
			{ p.x, p.y, 0.0f, 0.0f }	// texture coordinate
		);
	}
	for (int i = 1; i < leafHull.size()-1; i++)
	{
		leafMesh.DefineNewTriangle(0, i, i+1);
	}
	leafMesh.SendToGPU();

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

		leafTexture->UseForDrawing();
		leafShader.UpdateMVP(projection * leafMesh.transform.ModelMatrix());
		leafShader.Use();
		leafMesh.Draw();

		lineShader.UpdateMVP(projection);
		lineShader.Use();
		skeletonLines.Draw();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		grid.Draw(projection);

		glClear(GL_DEPTH_BUFFER_BIT);
		leafCanvas.RenderToScreen();
		window.SwapFramebuffer();
	}

	exit(0);
}
