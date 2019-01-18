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
static const bool WINDOW_VSYNC = true;
static const int WINDOW_FULLSCREEN = 0;
static const int WINDOW_WIDTH = 1280;
static const int WINDOW_HEIGHT = 720;
static const float CAMERA_FOV = 90.0f;
static const float WINDOW_RATIO = WINDOW_WIDTH / float(WINDOW_HEIGHT);
static const int FPS_LIMIT = 0;

namespace fs = std::filesystem;

void GenerateNewTree(GLLine& skeletonLines, GLTriangleMesh& branchMeshes, GLTriangleMesh& crownLeavesMeshes, const GLTriangleMesh& leafMesh, UniformRandomGenerator& uniformGenerator, int treeIterations=10, int treeSubdivisions=3)
{
	skeletonLines.Clear();
	branchMeshes.Clear();
	crownLeavesMeshes.Clear();

	/*
		Tree branch propertes
	*/
	float trunkThickness = 0.5f * powf(1.3f, float(treeIterations));
	float branchScalar = 0.4f;											// how the branch thickness relates to the parent
	float depthScalar = powf(0.75f, 1.0f / float(treeSubdivisions));	// how much the branch shrinks in thickness the farther from the root it goes (the pow is to counter the subdiv growth)
	const int trunkCylinderDivisions = 32;

	/*
		Leaf generation properties
	*/
	float leafMinScale = 0.25f;
	float leafMaxScale = 1.5f;
	float growthCurve = treeIterations / (1.0f + float(treeIterations));
	float pruningChance = growthCurve * 2.0f - 1.0f;// Random chance to remove a leaf (chance increases by the number of iterations)

	int leavesPerBranch = 25 - int(20 * (growthCurve * 2.0f - 1.0f));
	leavesPerBranch = (leavesPerBranch == 0) ? 1 : leavesPerBranch;
	


	/*
		Helper functions
	*/
	auto& getBranchThickness = [&](int branchDepth, int nodeDepth) -> float
	{
		return trunkThickness * powf(branchScalar, float(branchDepth)) * powf(depthScalar, float(nodeDepth));
	};

	auto& getCylinderDivisions = [&](int branchDepth) -> int
	{
		int cylinderDivisions = int(trunkCylinderDivisions / pow(2, branchDepth));
		return (cylinderDivisions < 4) ? 6 : cylinderDivisions;
	};


	GenerateFractalTree3D(
		uniformGenerator,
		treeIterations,
		treeSubdivisions,
		[&](Bone<FractalTree3DProps>* root, std::vector<FractalBranch>& branches) -> void
	{
		if (!root) return;
		using TBone = Bone<FractalTree3DProps>;

		for (int b = 0; b < branches.size(); b++)
		{
			int cylinderDivisions = getCylinderDivisions(branches[b].depth);
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
				float thickness = getBranchThickness(branches[b].depth, bone->nodeDepth);
				float circumference = 2.0f*PI_f*thickness;
				texU += bone->length / circumference;

				glm::fvec3 localX = bone->transform.sideDirection;
				glm::fvec3 localY = bone->transform.forwardDirection;

				// Make the branch root blend into its parent a bit. (this makes the branches appear less angular)
				auto& t = bone->transform;
				glm::fvec3 position = t.position;
				if (depth < (treeSubdivisions - 1) && branchNodes[0]->parent)
				{
					auto& parent = branchNodes[0]->parent;
					float blendAlpha = depth / float(treeSubdivisions);

					glm::fvec3 u = parent->transform.forwardDirection;
					glm::fvec3 v = bone->transform.position - parent->transform.position;
					float length = glm::length(v);
					v /= length;
					glm::fvec3 projectionOnParent = parent->transform.position + glm::dot(u, v) * u * length * blendAlpha;

					position = glm::mix(projectionOnParent, t.position, 0.5f + 0.5f*blendAlpha);
					thickness = glm::mix(thickness / branchScalar, thickness, 0.4f + 0.6f*blendAlpha);

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
						glm::fvec4{ texU, i / float(cylinderDivisions), 1.0f, 1.0f }
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
				glm::fvec4{ texU + lastBone->length, 0.5f, 1.0f, 1.0f }
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




		/*
			Generate leaves
		*/
		int maxBranchDepth = 0;
		for (auto& branch : branches)
		{
			maxBranchDepth = (branch.depth > maxBranchDepth) ? branch.depth : maxBranchDepth;
		}

		int startDepth = maxBranchDepth - 2;
		startDepth = (startDepth > 2) ? startDepth : 2;

		for (auto& branch : branches)
		{
			if (branch.depth < startDepth) continue;

			auto& branchNodes = branch.nodes;
			int lastIndex = int(branchNodes.size() - 1);
			int startIndex = int(round(0.25f * lastIndex));
			for (int i = startIndex; i <= lastIndex; ++i)
			{
				auto& leafNode = branchNodes[i];

				glm::fvec3 nodeBegin = leafNode->transform.position;
				glm::fvec3 nodeEnd = leafNode->tipPosition();
				glm::fvec3 nodeDirection = leafNode->transform.forwardDirection;
				glm::fvec3 nodeNormal = leafNode->transform.sideDirection;

				float thickness = getBranchThickness(branch.depth, leafNode->nodeDepth);
				float circumference = 2.0f*PI_f*thickness;

				int leafId = leavesPerBranch;
				float stepSize = leafNode->length / leavesPerBranch;
				glm::fvec3 position, direction, normal;
				while (leafId > 0)
				{
					leafId--;
					if (uniformGenerator.RandomFloat() < pruningChance) continue;

					// Compute the leaf placement
					position = nodeBegin + nodeDirection*(stepSize*leafId + uniformGenerator.RandomFloat(0.0f, stepSize/2.0f));	// spread along branch
					float angle = uniformGenerator.RandomFloat(0.0f, 2.0f*PI_f);
					direction = glm::rotate(glm::mat4{ 1.0f }, angle, nodeDirection) * glm::fvec4{ nodeNormal, 1.0f };				// random direction
					position += direction * thickness;																				// push leaf so that it starts on the branch and not inside it
					direction = glm::normalize(glm::mix(direction, nodeDirection, uniformGenerator.RandomFloat(0.3f, 0.8f)));		// blend how much the leaf is angled along the branch
					angle = uniformGenerator.RandomFloat(0.0f, 22.0f* PI_f);
					normal = glm::rotate(glm::mat4{ 1.0f }, angle, direction) * glm::fvec4{ nodeDirection, 1.0f };					// random twist
				
					// Insert the leaf
					crownLeavesMeshes.AppendMeshTransformed(
						leafMesh,
						glm::inverse(glm::lookAt(position, position - direction, -normal)) * glm::scale(glm::mat4{ 1.0f }, glm::fvec3{ uniformGenerator.RandomFloat(leafMinScale, leafMaxScale) })
					);
				}

				// Put a leaf at the tip of the branch
				if (i == lastIndex)
				{
					crownLeavesMeshes.AppendMeshTransformed(
						leafMesh,
						glm::inverse(glm::lookAt(nodeEnd, nodeEnd - nodeDirection, -nodeNormal)) * glm::scale(glm::mat4{ 1.0f }, glm::fvec3{ uniformGenerator.RandomFloat(leafMinScale, leafMaxScale) })
					);
				}

				// Debug orientation lines
				//skeletonLines.AddLine(nodeEnd, nodeEnd + 0.2f*nodeDirection, glm::fvec4(0.0f, 1.0f, 0.0f, 1.0f));
				//skeletonLines.AddLine(nodeEnd, nodeEnd + 0.2f*nodeNormal, glm::fvec4(1.0f, 0.0f, 0.0f, 1.0f));
			}

		}
	});

	branchMeshes.SendToGPU();
	crownLeavesMeshes.SendToGPU();
	skeletonLines.SendToGPU();

	printf("\r\n\tDone! LeavesPerBranch: %d, Pruning: %.2f\r\n", leavesPerBranch, pruningChance);
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

printf(R"(
====================================================================
	
	L-system Tree Generator.

	Controls:
		Mouse controls the camera. (L: Rotate, M: Move, R: Zoom)

		G:				Generate new tree with current settings
		Up arrow:		Increase L-system iterations (bigger tree)
		Down arrow:		Decrease L-system iterations (smaller tree)
		Left arrow:		Decrease branch divisions
		Right arrow:	Increase branch divisions

	Please note that iterations greater than 6 takes a long time.
	The application will not refresh during generations and will
	appear to "hang".

====================================================================
)");

	GLuint defaultVao = 0;
	glGenVertexArrays(1, &defaultVao);
	glBindVertexArray(defaultVao);

	Camera camera;
	TurntableController turntable(camera);
	turntable.position = glm::vec3{0.0f, 7.0f, 0.0f};
	turntable.sensitivity = 0.25f;
	turntable.Set(-25.0f, 15.0f, 15.0f);

	GLTexture defaultTexture{contentFolder / "default.png"};
	GLTexture treeBarkTexture{contentFolder / "opengameart_org_bark-1024-colcor.png"};
	defaultTexture.UseForDrawing();

	GLQuad backgroundQuad;

	GLGrid grid;
	grid.size = 20.0f;
	grid.gridSpacing = 0.5f;

	GLProgram defaultShader, lineShader, treeShader, leafShader, phongShader, backgroundShader;
	ShaderManager shaderManager;
	shaderManager.InitializeFolder(contentFolder);
	shaderManager.LoadShader(defaultShader, L"basic_vertex.glsl", L"basic_fragment.glsl");
	shaderManager.LoadShader(leafShader, L"leaf_vertex.glsl", L"leaf_fragment.glsl");
	shaderManager.LoadShader(phongShader, L"phong_vertex.glsl", L"phong_fragment.glsl");
	shaderManager.LoadShader(treeShader, L"tree_vertex.glsl", L"tree_fragment.glsl");
	shaderManager.LoadShader(lineShader, L"line_vertex.glsl", L"line_fragment.glsl");
	shaderManager.LoadShader(backgroundShader, L"background_vertex.glsl", L"background_fragment.glsl");

	phongShader.Use(); phongShader.SetUniformVec4("lightColor", glm::fvec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	treeShader.Use(); treeShader.SetUniformVec4("lightColor", glm::fvec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	leafShader.Use(); leafShader.SetUniformVec4("lightColor", glm::fvec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	

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
			{ p.x - 0.5f, p.z, 1.0f - p.y}, // convert to world coordinate system (tip points towards x+, face towards z+)
			leafNormal,
			{ 1.0f, 0.0f, 0.0f, 1.0f }, // vertex color
			{ p.x, p.y, 0.0f, 0.0f }	// texture coordinate
		);
	}
	for (int i = 1; i < leafHull.size()-1; i++)
	{
		leafMesh.DefineNewTriangle(0, i, i+1);
	}
	leafMesh.ApplyMatrix(glm::scale(glm::mat4{ 1.0f }, glm::fvec3{ 0.5f }));
	leafMesh.SendToGPU();


	/*
		Build tree mesh
	*/
	GLLine skeletonLines, coordinateReferenceLines;
	GLTriangleMesh branchMeshes, crownLeavesMeshes;
	auto GenerateRandomTree = [&](int iterations = 5, int subdivisions = 3) {
		printf("\r\nGenerating for %d iterations and %d subdivisions", iterations, subdivisions);
		GenerateNewTree(skeletonLines, branchMeshes, crownLeavesMeshes, leafMesh, uniformGenerator, iterations, subdivisions);
	};
	GenerateRandomTree();

	/*
		Coordinate system reference lines
	*/
	glm::fvec3 orientationReferencePoint = glm::fvec3{ 0.0f, 0.0f, 0.0f };
	glm::fvec3 x = glm::fvec3{ 1.0f, 0.0f, 0.0f };
	glm::fvec3 y = glm::fvec3{ 0.0f, 1.0f, 0.0f };
	glm::fvec3 z = glm::fvec3{ 0.0f, 0.0f, 1.0f };
	coordinateReferenceLines.AddLine(orientationReferencePoint, orientationReferencePoint + x, glm::fvec4(x, 1.0f));
	coordinateReferenceLines.AddLine(orientationReferencePoint, orientationReferencePoint + y, glm::fvec4(y, 1.0f));
	coordinateReferenceLines.AddLine(orientationReferencePoint, orientationReferencePoint + z, glm::fvec4(z, 1.0f));
	coordinateReferenceLines.SendToGPU();

	/*
		Main application loop
	*/
	bool quit = false;
	bool captureMouse = false;
	bool renderWireframe = false;
	int treeIterations = 5;
	int treeSubdivisions = 3;
	double lastUpdate = 0.0;
	double deltaTime = 0.0;
	double fpsDelta = (FPS_LIMIT == 0) ? 0.0 : (1.0 / FPS_LIMIT);
	while (!quit)
	{
		clock.Tick();
		SetThreadedTime(clock.time);

		if (WINDOW_VSYNC || FPS_LIMIT == 0)
		{
			deltaTime = clock.deltaTime;
			lastUpdate = clock.time;
		}
		else
		{
			deltaTime = clock.time - lastUpdate;
			if (deltaTime < fpsDelta) continue;
			lastUpdate = clock.time;
		}

		window.SetTitle("Time: " + TimeString(clock.time) + ", FPS: " + FpsString(deltaTime));
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
				else if (key == SDLK_g) GenerateRandomTree(treeIterations, treeSubdivisions);
				else if (key == SDLK_UP) { GenerateRandomTree(++treeIterations, treeSubdivisions);}
				else if (key == SDLK_DOWN) { treeIterations = (treeIterations <= 1) ? 1 : treeIterations - 1; GenerateRandomTree(treeIterations, treeSubdivisions); }
				else if (key == SDLK_LEFT) { treeSubdivisions = (treeSubdivisions <= 1) ? 1 : treeSubdivisions - 1; GenerateRandomTree(treeIterations, treeSubdivisions); }
				else if (key == SDLK_RIGHT) { GenerateRandomTree(treeIterations, ++treeSubdivisions); }
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

		backgroundShader.Use();
		backgroundQuad.Draw();
		glClear(GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, (renderWireframe? GL_LINE : GL_FILL));

		glm::vec3 lightPos = glm::vec3(999999.0f);
		glm::mat4 projection = camera.ViewProjectionMatrix();
		glm::mat4 mvp = projection * branchMeshes.transform.ModelMatrix();

		// Leaves
		leafTexture->UseForDrawing();
		leafShader.Use();
		leafShader.SetUniformFloat("sssBacksideAmount", 0.75f);
		leafShader.SetUniformFloat("time", float(clock.time));
		leafShader.SetUniformVec3("cameraPosition", camera.GetPosition());
		leafShader.SetUniformVec3("lightPosition", lightPos);
		leafShader.UpdateMVP(mvp);
		crownLeavesMeshes.Draw();

		// Tree branches
		treeBarkTexture.UseForDrawing();
		treeShader.Use();
		treeShader.SetUniformVec3("cameraPosition", camera.GetPosition());
		treeShader.SetUniformVec3("lightPosition", lightPos);
		treeShader.UpdateMVP(mvp);
		branchMeshes.Draw();


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		grid.Draw(projection);

		glClear(GL_DEPTH_BUFFER_BIT);
		lineShader.UpdateMVP(projection);
		lineShader.Use();
		coordinateReferenceLines.Draw();
		leafCanvas.RenderToScreen();

		window.SwapFramebuffer();
	}

	exit(0);
}
