#pragma once
#include "lsystem.h"
#include "canvas.h"
#include "turtle.h"

void DrawFractalTree(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawKochCurve(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawSierpinskiTriangle(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawDragonCurve(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);
void DrawFractalPlant(Canvas2D& canvas, int iterations, float scale = 10.0f, glm::fvec2 origin = glm::fvec2{ 0.0f }, float startAngle = 90);