#pragma once

#ifndef _SHARER_H_
#define _SHADER_H_

#include <string>

class ShaderProgram;
class Shader {
	int shader;
	friend class ShaderProgram;
public:
	Shader(int type, const char *code);
	~Shader();
};

class ShaderProgram {
	int program;
public:
	ShaderProgram(int cnt, ...);
	~ShaderProgram();
	void enable();
	void disable();
	const int inline getID() const { return program; }
};

#endif