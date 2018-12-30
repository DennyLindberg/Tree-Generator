#pragma once
#include <vector>
#include "glad/glad.h"

struct GLQuadProperties
{
	int positionX;
	int positionY;
	int width;
	int height;

	void MatchWindowDimensions();
};

class GLQuad
{
protected:
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
	GLCube()
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

	~GLCube()
	{
		glBindVertexArray(0);
		glDeleteBuffers(1, &positionBuffer);
		glDeleteBuffers(1, &texCoordBuffer);
		glDeleteVertexArrays(1, &vao);
	}

	void Draw()
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 12*3);
	}

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