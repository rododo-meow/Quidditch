#include <gl/glew.h>
#include <stdarg.h>
#include "Shader.h"

Shader::Shader(int type, const char *code) {
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);
}

Shader::~Shader() {
	glDeleteShader(shader);
}

ShaderProgram::ShaderProgram(int cnt, ...) {
	program = glCreateProgram();
	va_list args;
	va_start(args, cnt);
	while (cnt--) {
		Shader *shader = va_arg(args, Shader*);
		glAttachShader(program, shader->shader);
	}
	va_end(args);
	glLinkProgram(program);
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(program);
}

void ShaderProgram::enable() {
	glUseProgram(program);
}

void ShaderProgram::disable() {
	glUseProgram(0);
}