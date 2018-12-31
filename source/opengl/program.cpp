#include "program.h"

#include <string>
#include <iostream>

GLProgram::GLProgram()
{
	programId = glCreateProgram();
	fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
}

GLProgram::~GLProgram()
{
	glDeleteProgram(programId);
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
}

void GLProgram::LoadFragmentShader(std::string shaderText)
{
	GLint sourceLength = (GLint)shaderText.size();
	const char *fragmentSourcePtr = shaderText.c_str();
	glShaderSource(fragment_shader_id, 1, &fragmentSourcePtr, &sourceLength);
}

void GLProgram::LoadVertexShader(std::string shaderText)
{
	GLint sourceLength = (GLint)shaderText.size();
	const char *vertexSourcePtr = shaderText.c_str();
	glShaderSource(vertex_shader_id, 1, &vertexSourcePtr, &sourceLength);
}

GLint CompileAndPrintStatus(GLuint glShaderId)
{
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
}

void GLProgram::CompileAndLink()
{
	if (CompileAndPrintStatus(vertex_shader_id) == GL_FALSE ||
		CompileAndPrintStatus(fragment_shader_id) == GL_FALSE)
	{
		std::cout << L"Failed to compile shaders\n";
	}
	else
	{
		glAttachShader(programId, vertex_shader_id);
		glAttachShader(programId, fragment_shader_id);
		glLinkProgram(programId);
	}
}

void GLProgram::Use()
{
	glUseProgram(programId);
}

GLuint GLProgram::Id()
{
	return programId;
}

GLTexturedProgram::GLTexturedProgram()
{
	LoadFragmentShader(R"glsl(
		#version 330

		in vec4 TCoord;

		uniform sampler2D textureSampler;
		layout(location = 0) out vec4 color;

		void main() 
		{
			color = texture(textureSampler, TCoord.rg);
		}
	)glsl");

	LoadVertexShader(R"glsl(
		#version 330

		layout(location = 0) in vec3 vertexPosition;
		layout(location = 1) in vec4 vertexTCoord;
		uniform mat4 mvp;

		out vec4 TCoord;

		void main()
		{
			gl_Position = mvp * vec4(vertexPosition, 1.0f);
			TCoord = vertexTCoord;
		}
	)glsl");

	CompileAndLink();

	glBindAttribLocation(programId, 0, "vertexPosition");
	glBindAttribLocation(programId, 1, "vertexTCoord");
	mvpId = glGetUniformLocation(programId, "mvp");
}

void GLTexturedProgram::UpdateMVP(glm::mat4& mvp)
{
	Use();
	glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);
}
