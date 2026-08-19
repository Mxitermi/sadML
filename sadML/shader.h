#pragma once
#include "defines.h"
#include "GL/glew.h"
#include <string>

struct Shader {
	Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename);
	virtual ~Shader();
	GLuint getShaderId();
	void bind();
	void unBind();
private:
	GLuint compile(std::string shaderSourceCode, GLenum type);
	std::string parse(const char* fileName);
	GLuint createShader(const char*vertexShaderFilename, const char* fragmentShaderFilename);

	GLuint shaderId;
};