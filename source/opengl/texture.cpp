#include "texture.h"

#include <iostream>
#include <memory>
#include <algorithm>
#include "../thirdparty/lodepng.h"

#define INTERNAL_PIXEL_FORMAT GL_RGBA
#define PIXEL_FORMAT GL_RGBA
#define PIXEL_TYPE GL_UNSIGNED_INT_8_8_8_8_REV

void GLTexture::UpdateParameters()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_PIXEL_FORMAT, width, height, 0, PIXEL_FORMAT, PIXEL_TYPE, (GLvoid*)glData.data());
}

inline void GLTexture::SetPixel(unsigned int pixelIndex, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	glData[pixelIndex + 0] = r;
	glData[pixelIndex + 1] = g;
	glData[pixelIndex + 2] = b;
	glData[pixelIndex + 3] = a;
}

void GLTexture::SetPixel(unsigned int x, unsigned int y, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	SetPixel(PixelArrayIndex(x, y), r, g, b, a);
}

void GLTexture::SetPixel(unsigned int x, unsigned int y, double r, double g, double b, double a)
{
	r = std::max(std::min(1.0, r), 0.0);
	g = std::max(std::min(1.0, g), 0.0);
	b = std::max(std::min(1.0, b), 0.0);
	SetPixel(x, y, GLubyte(r*255.0), GLubyte(g*255.0), GLubyte(b*255.0), GLubyte(a*255.0));
}

unsigned int GLTexture::PixelArrayIndex(unsigned int x, unsigned int y)
{
	return y * width * 4 + x * 4;
}

void GLTexture::UseForDrawing()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void GLTexture::CopyToGPU()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, PIXEL_FORMAT, PIXEL_TYPE, (GLvoid*)glData.data());
}

void GLTexture::FillDebug()
{
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			GLubyte r = (GLubyte)(x / (float)width * 255);
			GLubyte g = (GLubyte)(y / (float)height * 255);
			GLubyte b = 0;
			GLubyte a = 255;
			SetPixel(x, y, r, g, b, a);
		}
	}
}

void GLTexture::SaveAsPNG(std::string filename, bool incrementNewFile)
{
	unsigned error = lodepng::encode(filename, glData, (unsigned int)width, (unsigned int)height);
	if (error)
	{
		std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	}
}

void GLTexture::LoadPNG(std::string filename)
{
	unsigned sourceWidth, sourceHeight;

	glData.clear();
	glData.shrink_to_fit();

	std::vector<unsigned char> png;
	lodepng::State state;
	unsigned error = lodepng::load_file(png, filename);
	if (!error)
	{
		error = lodepng::decode(glData, sourceWidth, sourceHeight, state, png);
	}

	if (error)
	{
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	}
	else
	{
		const LodePNGColorMode& color = state.info_png.color;
		//switch (color.colortype)
		//{
		//case LCT_RGB:
		//	PIXEL_FORMAT = GL_RGB;
		//	break;
		//case LCT_RGBA:
		//default:
		//	PIXEL_FORMAT = GL_RGBA;
		//	break;
		//}

		width = sourceWidth;
		height = sourceHeight;

		size = width * height * lodepng_get_channels(&color);

		UpdateParameters();
	}
}
