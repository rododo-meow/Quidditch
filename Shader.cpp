#include "gl.h"
#include <stdarg.h>
#include "Shader.h"

Shader::Shader(int type, const char *code) {
	shader = glCreateShader(type);
	CHECK_GL;
	glShaderSource(shader, 1, &code, NULL);
	CHECK_GL;
	glCompileShader(shader);
	CHECK_GL;
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		int len;
		char buf[4096];
		glGetShaderInfoLog(shader, 4096, &len, buf);
		printf("shader compile error:\n%s\n%s", code, buf);
	}
}

Shader::~Shader() {
	glDeleteShader(shader);
}

ShaderProgram::ShaderProgram(int cnt, ...) {
	program = glCreateProgram();
	CHECK_GL;
	va_list args;
	va_start(args, cnt);
	while (cnt--) {
		Shader *shader = va_arg(args, Shader*);
		glAttachShader(program, shader->shader);
		CHECK_GL;
	}
	va_end(args);
	glBindAttribLocation(program, 1, "TexCoord");
	CHECK_GL;
	glBindAttribLocation(program, 2, "Normal");
	CHECK_GL;
	glLinkProgram(program);
	CHECK_GL;
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		int len;
		char buf[4096];
		glGetProgramInfoLog(program, 4096, &len, buf);
		printf("program link error:\n%s", buf);
	}
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

Shader* Shader::loadFromFile(int type, const char *filename) {
	struct stat fs;

	if (stat(filename, &fs))
		throw std::exception("Can't read vertex shader's code");
	char *buf = (char *)malloc(fs.st_size + 1);
	FILE *f = fopen(filename, "r");
	buf[fread(buf, 1, fs.st_size, f)] = 0;
	Shader *shader = new Shader(type, buf);
	fclose(f);
	free(buf);
	return shader;
}