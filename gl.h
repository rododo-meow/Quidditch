#pragma once

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <cstdio>

inline static void checkGLError(const char *func, size_t line) {
	GLenum err = glGetError();
	while (err != GL_NO_ERROR) {
		printf("%s:%d %x\n", func, line, err);
		err = glGetError();
	}
}

#ifdef NDEBUG
#define CHECK_GL \
	checkGLError(__FILE__, __LINE__);
#else
#define CHECK_GL
#endif
