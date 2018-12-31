#pragma once
#include <string>
#include "glad/glad.h"
#include "../core/math.h"

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
	GLuint Id();
};

class GLTexturedProgram : public GLProgram
{
protected:
	GLint mvpId = 0;

public:
	GLTexturedProgram();
	~GLTexturedProgram() = default;

	void UpdateMVP(glm::mat4& mvp);
};
