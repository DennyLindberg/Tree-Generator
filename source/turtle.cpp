#include "turtle.h"

void Turtle2D::Clear()
{
	turtleStack = std::stack<TurtleState>();
}

void Turtle2D::Draw(Canvas2D& canvas, std::string& symbols, glm::fvec2 startPosition, float startAngle)
{
	if (turtleStack.size() != 0)
	{
		Clear();
	}
	position = startPosition;
	angle = startAngle;

	for (char& c : symbols)
	{
		if (actions.count(c))
		{
			actions[c](*this, canvas);
		}
	}
}

void Turtle2D::PushState()
{
	turtleStack.push(TurtleState{ position, angle });
}

void Turtle2D::PopState()
{
	TurtleState oldState = turtleStack.top();
	position = oldState.position;
	angle = oldState.angle;
	turtleStack.pop();
}

glm::fvec2 Turtle2D::GetDirection()
{
	// Angle is flipped so that the system is right handed
	float radians = -angle * PI_f / 180.0f;
	return glm::fvec2{
		cosf(radians),
		sinf(radians)
	};
}

void Turtle2D::Rotate(float degrees)
{
	angle += degrees;
}