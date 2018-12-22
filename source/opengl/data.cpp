#include "data.h"

#include <iostream>
#include <memory>
#include <algorithm>
#include "../thirdparty/lodepng.h"

#define INTERNAL_PIXEL_FORMAT GL_RGBA
#define PIXEL_FORMAT GL_BGRA
#define PIXEL_TYPE GL_UNSIGNED_INT_8_8_8_8_REV

void GLImageBuffer::UpdateParameters()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_PIXEL_FORMAT, width, height, 0, PIXEL_FORMAT, PIXEL_TYPE, (GLvoid*)glData.data());
}

inline void GLImageBuffer::SetPixel(unsigned int pixelIndex, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	// Note that the bytes are stored in BGRA order
	// https://www.khronos.org/opengl/wiki/Pixel_Transfer

	glData[pixelIndex] = b;
	glData[pixelIndex + 1] = g;
	glData[pixelIndex + 2] = r;
	glData[pixelIndex + 3] = a;
}

void GLImageBuffer::SetPixel(unsigned int x, unsigned int y, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	SetPixel(PixelArrayIndex(x, y), r, g, b, a);
}

void GLImageBuffer::SetPixel(unsigned int x, unsigned int y, double r, double g, double b, double a)
{
	r = std::max(std::min(1.0, r), 0.0);
	g = std::max(std::min(1.0, g), 0.0);
	b = std::max(std::min(1.0, b), 0.0);
	SetPixel(x, y, GLubyte(r*255.0), GLubyte(g*255.0), GLubyte(b*255.0), GLubyte(a*255.0));
}

unsigned int GLImageBuffer::PixelArrayIndex(unsigned int x, unsigned int y)
{
	return y * width * 4 + x * 4;
}

void GLImageBuffer::UseForDrawing()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void GLImageBuffer::SendToGPU()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, PIXEL_FORMAT, PIXEL_TYPE, (GLvoid*)glData.data());
}

void GLImageBuffer::FillDebug()
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

void GLImageBuffer::SaveAsPNG(std::string filename, bool incrementNewFile)
{
	unsigned error = lodepng::encode(filename, glData, (unsigned int)width, (unsigned int)height);
	if (error)
	{
		std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	}
}

void GLImageBuffer::LoadPNG(std::string filename)
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

GLQuad::GLQuad()
{
	CreateMeshBuffer();
	CreateShaders();
}

GLQuad::~GLQuad()
{
	glDeleteProgram(glProgram);
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &texCoordBuffer);
}

void GLQuad::Draw()
{
	const GLuint QUAD_NUM_VERTICES = 6; // two triangles
	glUseProgram(glProgram);
	glDrawArrays(GL_TRIANGLES, 0, QUAD_NUM_VERTICES);
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

void GLQuad::CreateShaders()
{
	std::string glsl_vertex = R"glsl(
		#version 330

		layout(location = 0) in vec4 vertexPosition;
		layout(location = 1) in vec4 vertexTCoord;

		out vec4 TCoord;

		void main()
		{
			gl_Position = vertexPosition;
			TCoord = vertexTCoord;
		}
	)glsl";

	std::string glsl_fragment = R"glsl(
		#version 330

		in vec4 TCoord;

		uniform sampler2D textureSampler;
		layout(location = 0) out vec4 color;

		void main() 
		{
			color = texture(textureSampler, TCoord.rg);
			//color = TCoord;
		}
	)glsl";

	auto compileAndPrintStatus = [](GLuint glShaderId) -> GLint {
		GLint compileStatus = 0;
		glCompileShader(glShaderId);
		glGetShaderiv(glShaderId, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == GL_FALSE)
		{
			std::string message("");

			int infoLogLength = 0;
			glGetShaderiv(glShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength == 0)
			{
				message = "Message is empty (GL_INFO_LOG_LENGTH == 0)";
			}
			else
			{
				std::unique_ptr<GLchar[]> infoLog(new GLchar[infoLogLength]);
				int charsWritten = 0;
				glGetShaderInfoLog(glShaderId, infoLogLength, &charsWritten, infoLog.get());
				message = std::string(infoLog.get());
			}

			std::cout << L"GL_INFO_LOG: " << message;
		}

		return compileStatus;
	};

	// Generate program and shaders
	glProgram = glCreateProgram();
	GLint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	GLint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	// Load vertex shader
	GLint sourceLength = (GLint)glsl_vertex.size();
	const char *vertexSourcePtr = glsl_vertex.c_str();
	glShaderSource(vertex_shader_id, 1, &vertexSourcePtr, &sourceLength);

	// Load fragment shader
	sourceLength = (GLint)glsl_fragment.size();
	const char *fragmentSourcePtr = glsl_fragment.c_str();
	glShaderSource(fragment_shader_id, 1, &fragmentSourcePtr, &sourceLength);

	// Compile it
	if (compileAndPrintStatus(vertex_shader_id) == GL_FALSE ||
		compileAndPrintStatus(fragment_shader_id) == GL_FALSE)
	{
		std::cout << L"Failed to compile shaders\n";
	}

	// Link it
	glAttachShader(glProgram, vertex_shader_id);
	glAttachShader(glProgram, fragment_shader_id);

	glBindAttribLocation(glProgram, 0, "vertexPosition");
	glBindAttribLocation(glProgram, 1, "vertexTCoord");

	glLinkProgram(glProgram);
	glUseProgram(glProgram);

	// Remove unlinked shaders as we don't need them any longer
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
}

