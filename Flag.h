#pragma once

#ifndef _FLAG_H_
#define _FLAG_H_

#include <gl/glew.h>
#include <gl/freeglut.h>
#include "Eigen/Eigen"
#include "Object.h"
#include "Drawable.h"

class Texture;
class Flag : public Object, public Drawable {
	static const int N_WIDTH_POINTS = 4;
	static const int N_LENGTH_POINTS = 7;
	static const int ORDER = 4;
	static const float K;
	static const float M;
	static const float g;
	static const float RESISTANCE_FACTOR;
	static const float WIND_FORCE;
	float key_points[ORDER - 1 + N_WIDTH_POINTS + ORDER - 1][ORDER - 1 + N_LENGTH_POINTS + ORDER - 1][3];
	float tex_key_points[ORDER - 1 + N_WIDTH_POINTS + ORDER - 1][ORDER - 1 + N_LENGTH_POINTS + ORDER - 1][2];
	Eigen::Vector3f masses[N_WIDTH_POINTS][N_LENGTH_POINTS];
	Eigen::Vector3f masses_v[N_WIDTH_POINTS][N_LENGTH_POINTS];
	float WIDTH_MIN_D, WIDTH_MAX_D, WIDTH_ORI_D, LENGTH_MIN_D, LENGTH_MAX_D, LENGTH_ORI_D;
	float sknot[ORDER + ORDER - 1 + N_WIDTH_POINTS + ORDER - 1];
	float tknot[ORDER + ORDER - 1 + N_LENGTH_POINTS + ORDER - 1];
	GLUnurbs *nurbs;
	static void __stdcall nurbsError(GLenum code);
	const float flag_height, flag_length, flag_width;
	void updateKeyPoints();
	void initPhys(float flag_length, float flag_width);
	Texture *texture;
	GLUquadric *quad;
protected:
	void _render();
public:
	Flag(float flag_height, float flag_length, float flag_width, const std::string &tex_path);
	~Flag();
	void update(float deltaTime);
};

#endif