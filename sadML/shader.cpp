#include "defines.h"
#include "GL/glew.h"
#include <iostream>
#include "shader.h"
#include <fstream>


	Shader::Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename) {
		shaderId = createShader(vertexShaderFilename, fragmentShaderFilename);
	}
	Shader:: ~Shader() {
		glDeleteShader(shaderId);
	}

	void Shader::bind() {
		glUseProgram(shaderId);


	}
	void Shader::unBind() {
		glUseProgram(0);
	}

	GLuint Shader::compile(std::string shaderSourceCode, GLenum type) {
		GLuint id = glCreateShader(type);
		const char* src = shaderSourceCode.c_str();
		glShaderSource(id, 1, &src, 0);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE) {
			int length = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = new char[length];
			glGetShaderInfoLog(id, length, &length, message);
			std::cout << "Error in Compiling the Shader: " << message << std::endl;
			delete[] message;
			return 0;
		}
		return id;
	}
	std::string Shader::parse(const char* fileName) {
		FILE* file = nullptr;
		fopen_s(&file, fileName, "rb");
		if (file == nullptr) {
			std::cout << "File " << fileName << " not found" << std::endl;
			return "";
		}

		std::string content;
		fseek(file, 0, SEEK_END);
		size_t filesize = ftell(file);
		rewind(file);
		content.resize(filesize);

		fread(&content[0], 1, filesize, file);
		fclose(file);

		return content;
	}
	GLuint Shader::createShader(const char* vertexShaderFilename, const char* fragmentShaderFilename) {
		std::string vsS = parse(vertexShaderFilename);
		std::string fsS = parse(fragmentShaderFilename);

		GLuint program = glCreateProgram();
		GLuint vs = compile(vsS, GL_VERTEX_SHADER);
		GLuint fs = compile(fsS, GL_FRAGMENT_SHADER);

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);

		return program;
	}
