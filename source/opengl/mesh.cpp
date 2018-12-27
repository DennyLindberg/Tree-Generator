#include "mesh.h"

#include <string>
#include <iostream>

GLQuad::GLQuad()
{
	CreateMeshBuffer();
}

GLQuad::~GLQuad()
{
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &texCoordBuffer);
}

void GLQuad::Draw()
{
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GLQuad::CreateMeshBuffer()
{
	// GL coordinate system is not like most UIs where the origin is upper left corner.
	// Y is up, X is right 
	float scale = 1.0f;
	float right = 1.0f * scale;
	float left = -1.0f * scale;
	float top = 1.0f * scale;
	float bottom = -1.0f * scale;

	const GLuint valuesPerPosition = 3;
	std::vector<float> positions = {
		// Triangle 1
		left, top, 0.0f,
		left, bottom, 0.0f,
		right, bottom, 0.0f,

		// Triangle 2
		right, bottom, 0.0f,
		right, top, 0.0f,
		left, top, 0.0f,
	};

	// UVs work top to bottom, V is reversed to get image in correct orientation
	const GLuint valuesPerCoord = 4;
	std::vector<float> tcoords = {
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};

	const GLuint positionAttribId = 0;
	const GLuint texCoordAttribId = 1;

	// Generate buffers
	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &texCoordBuffer);

	// Load positions
	glEnableVertexAttribArray(positionAttribId);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glVertexAttribPointer(positionAttribId, valuesPerPosition, GL_FLOAT, false, 0, 0);
	BufferVector(GL_ARRAY_BUFFER, positions, GL_STATIC_DRAW);

	// Load UVs
	glEnableVertexAttribArray(texCoordAttribId);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glVertexAttribPointer(texCoordAttribId, valuesPerCoord, GL_FLOAT, false, 0, 0);
	BufferVector(GL_ARRAY_BUFFER, tcoords, GL_STATIC_DRAW);
}
