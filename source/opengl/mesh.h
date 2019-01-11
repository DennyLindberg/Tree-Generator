#pragma once
#include <vector>
#include "glad/glad.h"
#include "../core/math.h"

struct GLQuadProperties
{
	float positionX;
	float positionY;
	float width;
	float height;

	void MatchWindowDimensions();
};

class GLMesh
{
public:
	GLMesh() = default;
	~GLMesh() = default;
};

struct GLLineSegment
{
	glm::fvec3 start;
	glm::fvec3 end;
};

class GLLine
{
protected:
	GLuint vao = 0;
	GLuint positionBuffer = 0;
	GLuint colorBuffer = 0;

	std::vector<GLLineSegment> lineSegments;
	std::vector<glm::fvec4> colors;

public:
	GLLine()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		const GLuint positionAttribId = 0;
		const GLuint colorAttribId = 1;

		// Generate buffers
		glGenBuffers(1, &positionBuffer);
		glGenBuffers(1, &colorBuffer);

		// Load positions
		int valuesPerPosition = 3; // glm::fvec3 has 3 floats
		glEnableVertexAttribArray(positionAttribId);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glVertexAttribPointer(positionAttribId, valuesPerPosition, GL_FLOAT, false, 0, 0);

		// Load colors
		valuesPerPosition = 4; // glm::fvec4 has 4 floats
		glEnableVertexAttribArray(colorAttribId);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(colorAttribId, valuesPerPosition, GL_FLOAT, false, 0, 0);

		SendToGPU();
	}

	~GLLine()
	{
		glBindVertexArray(0);
		glDeleteBuffers(1, &positionBuffer);
		glDeleteBuffers(1, &colorBuffer);
		glDeleteVertexArrays(1, &vao);
	}

	void AddLine(glm::fvec3 start, glm::fvec3 end, glm::fvec4 color)
	{
		lineSegments.push_back(GLLineSegment{std::move(start), std::move(end)});
		colors.push_back(color);
		colors.push_back(std::move(color));
	}

	void Clear()
	{
		lineSegments.clear();
		lineSegments.shrink_to_fit();
		colors.clear();
		colors.shrink_to_fit();
		SendToGPU();
	}

	void SendToGPU()
	{
		glBindVertexArray(vao);

		// Positions
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		size_t size = lineSegments.size();
		bool hasData = (size > 0);
		float* frontPtr = (float*)(hasData ? &lineSegments.front() : NULL);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLLineSegment), frontPtr, GL_STATIC_DRAW);

		// Colors
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		size = colors.size();
		hasData = (size > 0);
		frontPtr = (float*)(hasData ? &colors.front() : NULL);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::fvec4), frontPtr, GL_STATIC_DRAW);
	}

	void Draw()
	{
		if (lineSegments.size() > 0)
		{
			glBindVertexArray(vao);
			glDrawArrays(GL_LINES, 0, GLsizei(lineSegments.size()) * 2 * 3);
		}
	}
};

class GLQuad
{
protected:
	GLuint vao = 0;
	GLuint positionBuffer = 0;
	GLuint texCoordBuffer = 0;

public:
	GLQuad();
	GLQuad(GLQuadProperties properties);
	~GLQuad();
	void Draw();

protected:
	struct MeshBufferProperties
	{
		float left;
		float right;
		float top;
		float bottom;
	};

	void CreateMeshBuffer(MeshBufferProperties properties);

	template <class T>
	void BufferVector(GLenum glBufferType, const std::vector<T>& vector, GLenum usage)
	{
		if (vector.size() > 0)
		{
			glBufferData(glBufferType, vector.size() * sizeof(T), &vector.front(), usage);
		}
		else
		{
			glBufferData(glBufferType, 0, NULL, usage);
		}
	}
};

class GLCube
{
protected:
	GLuint vao = 0;
	GLuint positionBuffer = 0;
	GLuint texCoordBuffer = 0;

public:
	GLCube();
	~GLCube();

	void Draw();

protected:
	template <class T>
	void BufferVector(GLenum glBufferType, const std::vector<T>& vector, GLenum usage)
	{
		if (vector.size() > 0)
		{
			glBufferData(glBufferType, vector.size() * sizeof(T), &vector.front(), usage);
		}
		else
		{
			glBufferData(glBufferType, 0, NULL, usage);
		}
	}
};