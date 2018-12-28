#include "mesh.h"
#include "../core/application.h"

#include <string>
#include <iostream>

void GLQuadProperties::MatchWindowDimensions()
{
	ApplicationSettings settings = GetApplicationSettings();
	positionX = 0;
	positionY = 0;
	width = settings.windowWidth;
	height = settings.windowHeight;

}

GLQuad::GLQuad()
{
	CreateMeshBuffer(MeshBufferProperties{
		-1.0f,
		1.0f,
		1.0f,
		-1.0f
	});
}

GLQuad::GLQuad(GLQuadProperties properties)
{
	ApplicationSettings settings = GetApplicationSettings();
	float windowWidth = float(settings.windowWidth);
	float windowHeight = float(settings.windowHeight);

	// GL coordinate system has the origin in the middle of the screen and
	// ranges between -1.0 to 1.0. UI coordinates must be remapped.
	float relativeWidth  = properties.width     / windowWidth;
	float relativeHeight = properties.height    / windowHeight;
	float relativeX      = properties.positionX / windowWidth;
	float relativeY      = properties.positionY / windowHeight;

	MeshBufferProperties bufferProperties{
		-1.0f + 2.0f*relativeX,						// left edge
		-1.0f + 2.0f*(relativeX + relativeWidth),	// right edge
		 1.0f - 2.0f*relativeY,			     		// top edge
		 1.0f - 2.0f*(relativeY + relativeHeight),  // bottom edge
	};
	CreateMeshBuffer(bufferProperties);
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

void GLQuad::CreateMeshBuffer(MeshBufferProperties properties)
{
	float left = properties.left;
	float right = properties.right;
	float top = properties.top;
	float bottom = properties.bottom;

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
