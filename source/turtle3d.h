#pragma once
#include "opengl/mesh.h"
#include "core/math.h"
#include <map>
#include <stack>
#include <functional>

struct TurtleTransform
{
	glm::fvec3 position = glm::fvec3{ 0.0f };
	glm::fvec3 forwardDirection = glm::fvec3{ 0.0f, 1.0f, 0.0f }; // normalized, orthogonal, "up" basis vector. Local Y+ defined in world space coordinates.
	glm::fvec3 sideDirection = glm::fvec3{ 0.0f, 0.0f, 1.0f }; // normalized, orthogonal, "side" basis vector. Local Z+ defined in world space coordinates.

	void Clear()
	{
		position = glm::fvec3{ 0.0f };
		forwardDirection = glm::fvec3{ 0.0f, 1.0f, 0.0f };
		sideDirection = glm::fvec3{ 0.0f, 0.0f, 1.0f };
	}
};

struct Bone
{
	Bone* parent = nullptr;
	Bone* firstChild = nullptr;
	Bone* nextSibling = nullptr;

	TurtleTransform transform;
	float length = 0.0f;

	Bone() = default;
	~Bone()
	{
		Clear();
	}

	glm::fvec3 tipPosition()
	{
		return transform.position + transform.forwardDirection*length;
	}

	void Clear()
	{
		parent = nullptr;
		transform.Clear();
		length = 0.0f;

		if (firstChild)
		{
			delete firstChild;
			firstChild = nullptr;
		}

		if (nextSibling)
		{
			delete nextSibling;
			nextSibling = nullptr;
		}
	}

	Bone* NewChild()
	{
		Bone* newChild = nullptr;
		if (!firstChild)
		{
			firstChild = new Bone();
			newChild = firstChild;
		}
		else
		{
			Bone* sibling = firstChild;
			while (sibling->nextSibling)
			{
				sibling = sibling->nextSibling;
			}

			sibling->nextSibling = new Bone();
			newChild = sibling->nextSibling;
		}

		newChild->transform.position = this->tipPosition();

		return newChild;
	}

	void DebugPrint(int depth = 0)
	{
		for (int i = 0; i < depth; ++i)
		{
			printf("  ");
		}

		printf("x\n");

		if (firstChild)
		{
			firstChild->DebugPrint(depth+1);
		}

		if (nextSibling)
		{
			nextSibling->DebugPrint(depth);
		}
	}

	void ForEach(std::function<void(Bone*)>& callback)
	{
		callback(this);

		if (firstChild)
		{
			firstChild->ForEach(callback);
		}

		if (nextSibling)
		{
			nextSibling->ForEach(callback);
		}
	}
};

class Turtle3D
{
public:
	std::map<char, std::function<void(Turtle3D&, int)>> actions;

	TurtleTransform transform;
	std::stack<Bone*> boneStack;
	Bone* activeBone = nullptr;
	Bone* rootBone = nullptr;

	Turtle3D()
	{
		Clear();
	}

	~Turtle3D() = default;

	void Clear()
	{
		transform.Clear();
		if (rootBone)
		{
			delete rootBone;
			rootBone = nullptr;
			activeBone = nullptr;
		}
		boneStack = std::stack<Bone*>();
	}

	void GenerateSkeleton(std::string& symbols, TurtleTransform startTransform = TurtleTransform{})
	{
		Clear();
		transform = std::move(startTransform);

		size_t size = symbols.size();
		for (int i=0; i<size; i++)
		{
			int repetitionCounter = 1;
			while ((i < int(size - 1)) && symbols[i] == symbols[i + 1])
			{
				repetitionCounter++;
				i++;
			}

			if (actions.count(symbols[i]))
			{
				actions[symbols[i]](*this, repetitionCounter);
			}
		}
	}

	void PushState()
	{
		boneStack.push(activeBone);
	}

	void PopState()
	{
		activeBone = boneStack.top();
		boneStack.pop();

		transform = activeBone->transform;
		transform.position = activeBone->tipPosition();
	}

	// Angles are related to the forward and side basis vectors.
	// Yaw is applied first.
	void Rotate(float yawDegrees, float pitchDegrees)
	{
		glm::mat4 identity{ 1 };

		// Twist the forward direction
		auto yawRot = glm::rotate(identity, glm::radians(yawDegrees), transform.forwardDirection);
		transform.sideDirection = yawRot * glm::fvec4(transform.sideDirection, 0.0f);

		// Change the up/down angle of the forward direction
		glm::fvec3 pitchVector = glm::cross(transform.forwardDirection, transform.sideDirection);
		auto pitchRot = glm::rotate(identity, glm::radians(pitchDegrees), pitchVector);
		transform.forwardDirection = pitchRot * glm::fvec4(transform.forwardDirection, 0.0f);
		transform.sideDirection = pitchRot * glm::fvec4(transform.sideDirection, 0.0f);
	}

	void MoveForward(float distance)
	{
		glm::fvec3 oldPosition = transform.position;
		transform.position += transform.forwardDirection*distance;

		if (!rootBone)
		{
			rootBone = new Bone{};
			activeBone = rootBone;
		}
		else
		{
			activeBone = activeBone->NewChild();
		}
		activeBone->transform = transform;
		activeBone->transform.position = oldPosition;
		activeBone->length = distance;
	}

	void ForEachBone(std::function<void(Bone*)> callback)
	{
		if (rootBone && callback)
		{
			rootBone->ForEach(callback);
		}
	}

	void BonesToGLLines(GLLine& lines, glm::fvec4 boneColor, glm::fvec4 normalColor)
	{
		ForEachBone([&lines, &boneColor, &normalColor](Bone* b) -> void
		{
			lines.AddLine(
				b->transform.position,
				b->tipPosition(),
				boneColor
			);
			lines.AddLine(
				b->transform.position,
				b->transform.position + b->transform.sideDirection*0.2f,
				normalColor
			);
		});
		lines.SendToGPU();
	}
};