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

void GenerateNewTree(GLLine& skeletonLines, GLTriangleMesh& branchMeshes, UniformRandomGenerator& uniformGenerator, int treeIterations=10, int treeSubdivisions=3)
{
	skeletonLines.Clear();
	branchMeshes.Clear();

	GenerateFractalTree3D(
		uniformGenerator,
		treeIterations,
		treeSubdivisions,
		[&skeletonLines, &branchMeshes, treeIterations, treeSubdivisions](Bone<FractalTree3DProps>* root, std::vector<FractalBranch>& branches) -> void
	{
		if (!root) return;
		using TBone = Bone<FractalTree3DProps>;

		float trunkThickness = 0.2f * powf(1.2f, float(treeIterations));
		float branchScalar = 0.6f;										// how the branch thickness relates to the parent
		float depthScalar = powf(0.6f, 1.0f / float(treeSubdivisions));	// how much the branch shrinks in thickness the farther from the root it goes (the pow is to counter the subdiv growth)
		const int trunkCylinderDivisions = 32;
		for (int b = 0; b < branches.size(); b++)
		{
			int cylinderDivisions = int(trunkCylinderDivisions / pow(2, branches[b].depth));
			cylinderDivisions = (cylinderDivisions < 4) ? 6 : cylinderDivisions;

			GLTriangleMesh newBranchMesh{ false };

			/*
				Vertex 
				Positions, Normals, Texture Coordinates
			*/
			// Create vertex rings around each bone
			auto& branchNodes = branches[b].nodes;
			float rootLength = branchNodes[0]->length;
			float texU = 0.0f; // Texture coordinate along branch, it varies depending on the bone length and must be tracked
			for (int depth = 0; depth < branchNodes.size(); depth++)
			{
				auto& bone = branchNodes[depth];
				float thickness = trunkThickness * powf(branchScalar, float(branches[b].depth)) * powf(depthScalar, float(bone->nodeDepth));
				float circumference = 2.0f*PI_f*thickness;
				texU += bone->length / circumference;

				glm::fvec3 localX = bone->transform.sideDirection;
				glm::fvec3 localY = bone->transform.forwardDirection;

				// Make the branch root blend into its parent a bit. (this makes the branches appear less angular)
				auto& t = bone->transform;
				glm::fvec3 position = t.position;
				if (depth < (treeSubdivisions-1) && branchNodes[0]->parent)
				{
					auto& parent = branchNodes[0]->parent;
					float blendAlpha = depth / float(treeSubdivisions);

					glm::fvec3 u = parent->transform.forwardDirection;
					glm::fvec3 v = bone->transform.position - parent->transform.position;
					float length = glm::length(v);
					v /= length;
					glm::fvec3 projectionOnParent = parent->transform.position + glm::dot(u, v) * u * length * blendAlpha;

					position = glm::mix(projectionOnParent, t.position, 0.5f + 0.5f*blendAlpha);
					thickness = glm::mix(thickness/branchScalar, thickness, 0.4f + 0.6f*blendAlpha);

					// Blend orientation of cylinder ring to give a spline
					auto& parentForward = parent->transform.forwardDirection;
					auto& boneForward = bone->transform.forwardDirection;
					localY = glm::normalize(glm::mix(parentForward, boneForward, blendAlpha));
					glm::fvec3 rotationVector = glm::normalize(glm::cross(boneForward, localY));
					float angle = glm::acos(glm::dot(boneForward, localY));
					localX = glm::rotate(glm::mat4(1.0f), angle, rotationVector) * glm::fvec4(localX, 0.0f);
				}

				// Generate the cylinder ring
				float angleStep = 360.0f / float(cylinderDivisions);
				for (int i = 0; i < cylinderDivisions; i++)
				{
					float angle = angleStep * i;
					glm::mat4 rot = glm::rotate(glm::mat4{ 1.0f }, glm::radians(angle), localY);
					glm::fvec3 normal = rot * glm::fvec4(localX, 0.0f);

					newBranchMesh.AddVertex(
						position + normal * thickness,
						normal,
						glm::fvec4{ 1.0f },
						glm::fvec4{ texU, i/float(cylinderDivisions), 1.0f, 1.0f }
					);
				}

				// Add extra set of vertices for the UV seam
				newBranchMesh.AddVertex(
					position + localX * thickness,
					localX,
					glm::fvec4{ 1.0f },
					glm::fvec4{ texU, 1.0f, 1.0f, 1.0f }
				);
			}

			// Add tip for branch
			auto& lastBone = branchNodes.back();
			newBranchMesh.AddVertex(
				lastBone->tipPosition(),
				lastBone->transform.forwardDirection,
				glm::fvec4{ 1.0f },
				glm::fvec4{ texU+lastBone->length, 0.5f, 1.0f, 1.0f }
			);



			/*
				Triangle Indices
			*/
			// Generate indices for cylinders
			int ringStep = cylinderDivisions + 1; // +1 because of UV seam
			for (int depth = 1; depth < branchNodes.size(); depth++)
			{
				int uStart = depth * ringStep;
				int lStart = uStart - ringStep;

				for (int i = 0; i < cylinderDivisions; i++)
				{
					int u = uStart + i;
					int l = lStart + i;

					newBranchMesh.DefineNewTriangle(l, l + 1, u + 1);
					newBranchMesh.DefineNewTriangle(u + 1, u, l);
				}
			}

			// Generate indices for tip
			int tipIndex = int(newBranchMesh.positions.size()) - 1;
			int lastRing = ringStep * (int(branchNodes.size()) - 1);
			for (int i = 1; i < ringStep; i++)
			{
				int ringId = lastRing + i;
				newBranchMesh.DefineNewTriangle(ringId - 1, ringId, tipIndex);
			}

			branchMeshes.AppendMesh(newBranchMesh);
		}
	});

	branchMeshes.SendToGPU();
	skeletonLines.SendToGPU();
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
	GLTexture treeBarkTexture{contentFolder / "opengameart_org_bark-1024-colcor.png"};
	defaultTexture.UseForDrawing();

	GLGrid grid;
	grid.size = 20.0f;
	grid.gridSpacing = 0.5f;

	GLProgram defaultShader, lineShader, leafShader, phongShader;
	ShaderManager shaderManager;
	shaderManager.InitializeFolder(contentFolder);
	shaderManager.LoadLiveShader(defaultShader, L"basic_vertex.glsl", L"basic_fragment.glsl");
	shaderManager.LoadLiveShader(leafShader, L"basic_vertex.glsl", L"leaf_fragment.glsl");
	shaderManager.LoadLiveShader(phongShader, L"phong_vertex.glsl", L"phong_fragment.glsl");
	shaderManager.LoadShader(lineShader, L"line_vertex.glsl", L"line_fragment.glsl");

	phongShader.Use();
	phongShader.SetUniformVec4("lightColor", glm::fvec4{1.0f, 1.0f, 1.0f, 1.0f});

	/*
		Build mesh using Turtle
	*/
	GLLine skeletonLines;
	GLTriangleMesh branchMeshes;
	auto GenerateRandomTree = [&]() {
		printf("\r\nGenerating");
		GenerateNewTree(skeletonLines, branchMeshes, uniformGenerator, 10, 3);
	};
	GenerateRandomTree();


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
				else if (key == SDLK_g) GenerateRandomTree();
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


		// Tree branches
		branchMeshes.transform.scale = glm::vec3(0.5f);

		phongShader.Use();
		glm::vec3 lightPos = glm::vec3(999999.0f);
		phongShader.SetUniformVec3("cameraPosition", camera.GetPosition());
		phongShader.SetUniformVec3("lightPosition", lightPos);
		phongShader.UpdateMVP(projection * branchMeshes.transform.ModelMatrix());
		treeBarkTexture.UseForDrawing();
		branchMeshes.Draw();

		//lineShader.UpdateMVP(projection);
		//lineShader.Use();
		//skeletonLines.Draw();


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		grid.Draw(projection);

		glClear(GL_DEPTH_BUFFER_BIT);
		leafCanvas.RenderToScreen();
		window.SwapFramebuffer();
	}

	exit(0);
}
