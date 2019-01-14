#pragma once
#include "lsystem.h"
#include "canvas.h"
#include "turtle2d.h"
#include "turtle3d.h"
#include "core/randomization.h"

void DrawFractalTree(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawKochCurve(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawSierpinskiTriangle(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawDragonCurve(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawFractalPlant(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawFractalTreeNezumiV1(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawFractalTreeNezumiV2(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawFractalTreeNezumiV3(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawFractalLeaf(std::vector<glm::fvec3>& generatedHull, Canvas2D& canvas, Color color, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);

void GenerateFractalPlant3D(Turtle3D& turtle, UniformRandomGenerator& uniformGenerator, int iterations, float scale = 0.1f);