#pragma once
#include "canvas.h"
#include "core/math.h"
#include <map>
#include <stack>
#include <functional>

struct TurtleState2D
{
	glm::fvec2 position;
	float angle;
};

class Turtle2D
{
public:
	glm::fvec2 position;
	float angle;

	std::stack<TurtleState2D> turtleStack;
	std::map<char, std::function<void(Turtle2D&, Canvas2D&)>> actions;

	Turtle2D() = default;
	~Turtle2D() = default;

	void Clear();
	void Draw(Canvas2D& canvas, std::string& symbols, glm::fvec2 startPosition, float startAngle);
	void PushState();
	void PopState();
	glm::fvec2 GetDirection();
	void Rotate(float degrees);
};