#pragma once

#ifndef _GL_MAT_UTIL_H_
#define _GL_MAT_UTIL_H_

#include <gl/glew.h>
#include <gl/freeglut.h>
#include "Eigen/Eigen"
#include <iostream>

inline void glSaveMatrix(GLenum pname, Eigen::Matrix4f &matrix) {
	GLfloat mat[16];
	glGetFloatv(pname, mat);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			matrix(j, i) = mat[i * 4 + j];
}

inline void glDumpMatrix(GLenum pname) {
	Eigen::Matrix4f t;
	glSaveMatrix(pname, t);
	std::cout << t << std::endl;
}

inline void glLoadMatrix(GLenum pname, const Eigen::Matrix4f &matrix) {
	glMatrixMode(pname);
	GLfloat mat[16];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat[i * 4 + j] = matrix(j, i);
	glLoadMatrixf(mat);
	glMatrixMode(GL_MODELVIEW);
}

inline void glMultiMatrix(const Eigen::Matrix4f &matrix) {
	GLfloat mat[16];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat[i * 4 + j] = matrix(j, i);
	glMultMatrixf(mat);
}

#endif