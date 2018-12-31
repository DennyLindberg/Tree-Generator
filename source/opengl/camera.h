#pragma once
#include "../core/math.h"
#include "../core/application.h"

class Camera
{
public:
	glm::vec3 position = glm::vec3{ 0.0f };
	glm::vec3 focusPoint = glm::vec3{ 0.0f };
	glm::vec3 upVector = {0.0f, 1.0f, 0.0f};
	float fieldOfView = 90.0f;
	float nearClipPlane = 0.1f;
	float farClipPlane = 100.0f;

	Camera() = default;
	~Camera() = default;

	inline glm::mat4 ViewMatrix()
	{
		return glm::lookAt(position, focusPoint, upVector);
	}

	inline glm::mat4 ProjectionMatrix()
	{
		return glm::perspective(
			glm::radians(fieldOfView),
			GetApplicationSettings().windowRatio,
			nearClipPlane, farClipPlane
		);
	}

	inline glm::mat4 ViewProjectionMatrix()
	{
		return ProjectionMatrix() * ViewMatrix();
	}
};