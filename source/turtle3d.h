#pragma once
#include "opengl/mesh.h"
#include "core/math.h"
#include <map>
#include <stack>
#include <functional>

template<class ContextType>
struct TurtleState3D
{
	glm::fvec3 position         = glm::fvec3{ 0.0f };
	glm::fvec3 forwardDirection = glm::fvec3{ 0.0f, 1.0f, 0.0f }; // normalized, orthogonal, "up" basis vector. Local Y+ defined in world space coordinates.
	glm::fvec3 sideDirection    = glm::fvec3{ 0.0f, 0.0f, 1.0f }; // normalized, orthogonal, "side" basis vector. Local Z+ defined in world space coordinates.
	ContextType context         = ContextType{};                  // templated type to define context-specific data
};

template<class ContextType>
class Turtle3D
{
public:
	using TurtleState = TurtleState3D<ContextType>;
	using SkeletonJoint = TurtleState;

	TurtleState previousState;
	TurtleState state;
	std::stack<TurtleState> turtleStack;
	std::map<char, std::function<void(Turtle3D<ContextType>&)>> actions;

	std::vector<std::pair<TurtleState, TurtleState>> bones;

	Turtle3D()
	{
		Clear();
	}

	~Turtle3D() = default;

	void Clear()
	{
		turtleStack = std::stack<TurtleState>();
		bones.clear();
		bones.shrink_to_fit();
		state = TurtleState{};
	}

	void GenerateSkeleton(std::string& symbols, TurtleState startState = TurtleState{})
	{
		Clear();
		state = startState;
		previousState = state;
		PushNewBone(); // root

		for (char& c : symbols)
		{
			if (actions.count(c))
			{
				actions[c](*this);
			}
		}
	}

	void PushState()
	{
		previousState = state;
		turtleStack.push(state);
	}

	void PopState()
	{
		state = turtleStack.top();
		previousState = turtleStack.empty()? state : turtleStack.top();

		turtleStack.pop();
	}

	// Angles are related to the forward and side basis vectors
	// Yaw is applied first.
	void Rotate(float yawDegrees, float pitchDegrees)
	{
		glm::mat4 identity{ 1 };

		// Twist the forward direction
		auto yawRot = glm::rotate(identity, glm::radians(yawDegrees), state.forwardDirection);
		state.sideDirection = yawRot * glm::fvec4(state.sideDirection, 0.0f);

		// Change the up/down angle of the forward direction
		glm::fvec3 pitchVector = glm::cross(state.forwardDirection, state.sideDirection);
		auto pitchRot = glm::rotate(identity, glm::radians(pitchDegrees), pitchVector);
		state.forwardDirection = pitchRot * glm::fvec4(state.forwardDirection, 0.0f);
		state.sideDirection = pitchRot * glm::fvec4(state.sideDirection, 0.0f);
	}

	void MoveForward(float distance)
	{
		state.position += state.forwardDirection*distance;

		PushNewBone();

		previousState = state;
	}

	void PushNewBone()
	{
		bones.push_back(std::make_pair(previousState, state));
	}

	void ForEachBone(std::function<void(std::pair<TurtleState, TurtleState>&)> callback)
	{
		if (!callback) return;

		for (auto& b : bones)
		{
			callback(b);
		}
	}
};