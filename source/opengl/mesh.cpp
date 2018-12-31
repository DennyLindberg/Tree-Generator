#include "mesh.h"
#include "../core/application.h"

#include <string>
#include <iostream>

void GLQuadProperties::MatchWindowDimensions()
{
	ApplicationSettings settings = GetApplicationSettings();
	positionX = 0.0f;
	positionY = 0.0f;
	width = float(settings.windowWidth);
	height = float(settings.windowHeight);

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
	glBindVertexArray(vao);
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

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

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


GLCube::GLCube()
{
	std::vector<float> positions = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

	std::vector<float> uvs = {
		0.000059f, 1.0f - 0.000004f, 1.0f, 1.0f,
		0.000103f, 1.0f - 0.336048f, 1.0f, 1.0f,
		0.335973f, 1.0f - 0.335903f, 1.0f, 1.0f,
		1.000023f, 1.0f - 0.000013f, 1.0f, 1.0f,
		0.667979f, 1.0f - 0.335851f, 1.0f, 1.0f,
		0.999958f, 1.0f - 0.336064f, 1.0f, 1.0f,
		0.667979f, 1.0f - 0.335851f, 1.0f, 1.0f,
		0.336024f, 1.0f - 0.671877f, 1.0f, 1.0f,
		0.667969f, 1.0f - 0.671889f, 1.0f, 1.0f,
		1.000023f, 1.0f - 0.000013f, 1.0f, 1.0f,
		0.668104f, 1.0f - 0.000013f, 1.0f, 1.0f,
		0.667979f, 1.0f - 0.335851f, 1.0f, 1.0f,
		0.000059f, 1.0f - 0.000004f, 1.0f, 1.0f,
		0.335973f, 1.0f - 0.335903f, 1.0f, 1.0f,
		0.336098f, 1.0f - 0.000071f, 1.0f, 1.0f,
		0.667979f, 1.0f - 0.335851f, 1.0f, 1.0f,
		0.335973f, 1.0f - 0.335903f, 1.0f, 1.0f,
		0.336024f, 1.0f - 0.671877f, 1.0f, 1.0f,
		1.000004f, 1.0f - 0.671847f, 1.0f, 1.0f,
		0.999958f, 1.0f - 0.336064f, 1.0f, 1.0f,
		0.667979f, 1.0f - 0.335851f, 1.0f, 1.0f,
		0.668104f, 1.0f - 0.000013f, 1.0f, 1.0f,
		0.335973f, 1.0f - 0.335903f, 1.0f, 1.0f,
		0.667979f, 1.0f - 0.335851f, 1.0f, 1.0f,
		0.335973f, 1.0f - 0.335903f, 1.0f, 1.0f,
		0.668104f, 1.0f - 0.000013f, 1.0f, 1.0f,
		0.336098f, 1.0f - 0.000071f, 1.0f, 1.0f,
		0.000103f, 1.0f - 0.336048f, 1.0f, 1.0f,
		0.000004f, 1.0f - 0.671870f, 1.0f, 1.0f,
		0.336024f, 1.0f - 0.671877f, 1.0f, 1.0f,
		0.000103f, 1.0f - 0.336048f, 1.0f, 1.0f,
		0.336024f, 1.0f - 0.671877f, 1.0f, 1.0f,
		0.335973f, 1.0f - 0.335903f, 1.0f, 1.0f,
		0.667969f, 1.0f - 0.671889f, 1.0f, 1.0f,
		1.000004f, 1.0f - 0.671847f, 1.0f, 1.0f,
		0.667979f, 1.0f - 0.335851f, 1.0f, 1.0f
	};

	std::vector<float> colors = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const GLuint positionAttribId = 0;
	const GLuint texCoordAttribId = 1;

	// Generate buffers
	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &texCoordBuffer);

	// Load positions
	glEnableVertexAttribArray(positionAttribId);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glVertexAttribPointer(positionAttribId, 3, GL_FLOAT, false, 0, 0);
	BufferVector(GL_ARRAY_BUFFER, positions, GL_STATIC_DRAW);

	// Load UVs
	glEnableVertexAttribArray(texCoordAttribId);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glVertexAttribPointer(texCoordAttribId, 4, GL_FLOAT, false, 0, 0);
	BufferVector(GL_ARRAY_BUFFER, uvs, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLCube::~GLCube()
{
	glBindVertexArray(0);
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &texCoordBuffer);
	glDeleteVertexArrays(1, &vao);
}

void GLCube::Draw()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}