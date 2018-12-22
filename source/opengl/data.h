#pragma once
#include <vector>
#include "glad/glad.h"

class GLImageBuffer
{
public:
	std::vector<GLubyte> glData; // vector is used to simplify load/save with lodepng
	GLuint textureId = 0;
	
	int size = 0;
	int numPixels = 0;
	int width = 0;
	int height = 0;

public:
	GLImageBuffer(int imageWidth, int imageHeight)
		: width{ imageWidth }, height{ imageHeight }
	{
		numPixels = width * height;
		size = numPixels * 4;
		glData.resize(size);

		for (int i = 0; i < size; ++i)
		{
			glData[i] = 0;
		}

		glGenTextures(1, &textureId);
		UpdateParameters();
	}

	~GLImageBuffer()
	{
		glDeleteTextures(1, &textureId);
	}

	void UpdateParameters();

	inline GLubyte& operator[] (unsigned int i) { return glData[i]; }

	inline void SetPixel(unsigned int pixelIndex, GLubyte r, GLubyte g, GLubyte b, GLubyte a);
	void SetPixel(unsigned int x, unsigned int y, GLubyte r, GLubyte g, GLubyte b, GLubyte a);
	void SetPixel(unsigned int x, unsigned int y, double r, double g, double b, double a);

	unsigned int PixelArrayIndex(unsigned int x, unsigned int y);

	void UseForDrawing();
	void SendToGPU();

	void FillDebug();
	void SaveAsPNG(std::string filename, bool incrementNewFile = false);
	void LoadPNG(std::string filename);
};

class GLQuad
{
protected:
	GLuint positionBuffer = 0;
	GLuint texCoordBuffer = 0;

	GLuint glProgram = 0;

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

	void CreateShaders();
};