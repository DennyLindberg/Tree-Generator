#pragma once
#include <vector>
#include "glad/glad.h"

class GLTexture
{
public:
	std::vector<GLubyte> glData; // vector is used to simplify load/save with lodepng
	GLuint textureId = 0;
	
	int size = 0;
	int numPixels = 0;
	int width = 0;
	int height = 0;

public:
	GLTexture(int textureWidth, int textureHeight)
		: width{ textureWidth }, height{ textureHeight }
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

	~GLTexture()
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
	void CopyToGPU();

	void FillDebug();
	void SaveAsPNG(std::string filename, bool incrementNewFile = false);
	void LoadPNG(std::string filename);
};
