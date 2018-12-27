#pragma once
#include <string>
#include "glad/glad.h"

class GLProgram
{
protected:
	GLuint programId = 0;
	GLint vertex_shader_id = 0;
	GLint fragment_shader_id = 0;

public:
	GLProgram();
	~GLProgram();

	void LoadFragmentShader(std::string shaderText);
	void LoadVertexShader(std::string shaderText);
	void CompileAndLink();
	void Use();
};

class GLTexturedProgram : public GLProgram
{
public:
	GLTexturedProgram();
	~GLTexturedProgram() = default;
};
