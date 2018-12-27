#pragma once
#include <vector>
#include "glad/glad.h"

class GLQuad
{
protected:
	GLuint positionBuffer = 0;
	GLuint texCoordBuffer = 0;

public:
	GLQuad();
	~GLQuad();
	void Draw();

protected:
	void CreateMeshBuffer();

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