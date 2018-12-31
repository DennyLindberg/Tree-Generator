#pragma once
#include <vector>
#include "glad/glad.h"

struct GLQuadProperties
{
	float positionX;
	float positionY;
	float width;
	float height;

	void MatchWindowDimensions();
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