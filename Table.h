#pragma once

#ifndef _TABLE_H_
#define _TABLE_H_

#include "Eigen/Eigen"
#include "Object.h"

class OBJDrawable;
class Texture;
class Table : public Object {
	unsigned int vbo, vao;
	struct PerVertex {
		float pos_x, pos_y, pos_z;
		float tex_coord_x, tex_coord_y, tex_coord_z;
		float normal_x, normal_y, normal_z;
	};
	Texture *texture;
public:
	const size_t N_WIDTH, N_LENGTH;
	const Eigen::Vector3f * const normals;
	const enum Direction {
		ZS_YX, YS_ZX
	} * const directions;
	OBJDrawable * const drawable;
	Table(const std::string &model, const std::string &faceTexture, size_t N_WIDTH, size_t N_LENGTH, const float *height);
	~Table();
	void render();
};

#endif