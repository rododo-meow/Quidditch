#include <cmath>

#include "Table.h"
#include "OBJDrawable.h"
#include "gl.h"
#include "GameParam.h"
#include "Texture.h"

using namespace GameParam;

Table::Table(const std::string &model, const std::string &faceTexture, float WIDTH, float LENGTH, size_t N_WIDTH, size_t N_LENGTH, const float *height) : 
		drawable(new OBJDrawable(model)), 
		WIDTH(WIDTH),
		LENGTH(LENGTH),
		N_WIDTH(N_WIDTH), 
		N_LENGTH(N_LENGTH), 
		texture(Texture::load(faceTexture.c_str())), 
		normals(new Eigen::Vector3f[N_WIDTH * N_LENGTH * 2]),
		directions(new Direction[N_WIDTH * N_LENGTH]),
		vertex(new Eigen::Vector3f[(N_WIDTH + 1) * (N_LENGTH + 1)]) {
	drawable->bind(this);

	maxHeight = 0;
	for (size_t i = 0; i <= N_WIDTH; i++)
		for (size_t j = 0; j <= N_LENGTH; j++) {
			*const_cast<Eigen::Vector3f*>(&vertex[i * (N_LENGTH + 1) + j]) = Eigen::Vector3f(TABLE_LENGTH * j / N_LENGTH, height[i* (N_LENGTH + 1) + j], TABLE_WIDTH * i / N_WIDTH);
			maxHeight = std::max(height[i* (N_LENGTH + 1) + j], maxHeight);
		}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, N_WIDTH * N_LENGTH * 2 * 3 * sizeof(PerVertex), NULL, GL_STATIC_DRAW);
	{
		PerVertex *map = (PerVertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		Eigen::Vector3f *tmp = const_cast<Eigen::Vector3f*>(normals);
		for (size_t i = 0; i < N_WIDTH; i++) {
			float h1 = height[i * (N_LENGTH + 1)], h2 = height[(i + 1) * (N_LENGTH + 1)];
			for (size_t j = 0; j < N_LENGTH; j++) {
				float h3 = height[i * (N_LENGTH + 1) + j + 1], h4 = height[(i + 1) * (N_LENGTH + 1) + j + 1];

#define ADD_VERTEX(i, j, normal) { \
	map->pos_x = TABLE_LENGTH * (j) / N_LENGTH - TABLE_LENGTH / 2; \
	map->pos_y = TABLE_FACE_Y + height[(i) * (N_LENGTH + 1) + (j)]; \
	map->pos_z = TABLE_WIDTH * (i) / N_WIDTH - TABLE_WIDTH / 2; \
	map->tex_coord_x = 1.0f * (j) / N_LENGTH; \
	map->tex_coord_y = 1.0f * (i) / N_WIDTH; \
	map->tex_coord_z = 0; \
	map->normal_x = (normal).x(); \
	map->normal_y = (normal).y(); \
	map->normal_z = (normal).z(); \
	map++; }
#define ADD_TRIANGLE(i1, j1, i2, j2, i3, j3) { \
	Eigen::Vector3f normal = Eigen::Vector3f( \
		((int) ((j2) - (j1))) * TABLE_LENGTH / N_LENGTH, \
		height[(i2) * (N_LENGTH + 1) + (j2)] - height[(i1) * (N_LENGTH + 1) + (j1)], \
		((int) ((i2) - (i1))) * TABLE_WIDTH / N_WIDTH).cross(Eigen::Vector3f( \
			((int) ((j3) - (j2))) * TABLE_LENGTH / N_LENGTH, \
			height[(i3) * (N_LENGTH + 1) + (j3)] - height[(i2) * (N_LENGTH + 1) + (j2)], \
			((int) ((i3) - (i2))) * TABLE_WIDTH / N_WIDTH)).normalized(); \
	ADD_VERTEX((i1), (j1), normal); \
	ADD_VERTEX((i2), (j2), normal); \
	ADD_VERTEX((i3), (j3), normal); \
	*tmp++ = normal; }

				if (h1 + h4 > h2 + h3) {
					*const_cast<Direction*>(directions + i * N_LENGTH + j) = YS_ZX;
					ADD_TRIANGLE(i, j, i + 1, j, i + 1, j + 1);
					ADD_TRIANGLE(i, j, i + 1, j + 1, i, j + 1);
				} else {
					*const_cast<Direction*>(directions + i * N_LENGTH + j) = ZS_YX;
					ADD_TRIANGLE(i, j, i + 1, j, i, j + 1);
					ADD_TRIANGLE(i + 1, j, i + 1, j + 1, i, j + 1);
				}
				h1 = h3, h2 = h4;
			}
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(6 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Table::~Table() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	delete drawable;
}

void Table::render() {
	drawable->render();
	glBindVertexArray(vao);
	texture->enable();
	glDrawArrays(GL_TRIANGLES, 0, N_LENGTH * N_WIDTH * 2 * 3);
	texture->disable();
	glBindVertexArray(0);
}

float Table::getHeight(float x, float z) const {
	int i = z * N_WIDTH / WIDTH, j = x * N_LENGTH / LENGTH;
	float off_x = x * N_LENGTH / LENGTH - j, off_z = z * N_WIDTH / WIDTH - i;
	switch (directions[i * N_LENGTH + j]) {
	case ZS_YX:
		if (off_x + off_z >= 1)
			return vertex[(i + 1) * (N_LENGTH + 1) + j + 1].y() +
				(vertex[(i + 1) * (N_LENGTH + 1) + j].y() - vertex[(i + 1) * (N_LENGTH + 1) + j + 1].y()) * (1 - off_x) +
				(vertex[i * (N_LENGTH + 1) + j + 1].y() - vertex[(i + 1) * (N_LENGTH + 1) + j + 1].y()) *(1 - off_z);
		else
			return vertex[i * (N_LENGTH + 1) + j].y() +
				(vertex[i * (N_LENGTH + 1) + j + 1].y() - vertex[i * (N_LENGTH + 1) + j].y()) * (1 - off_x) +
				(vertex[(i + 1) * (N_LENGTH + 1) + j].y() - vertex[i * (N_LENGTH + 1) + j].y()) *(1 - off_z);
	case YS_ZX:
		if (off_x >= off_z)
			return vertex[i * (N_LENGTH + 1) + j + 1].y() +
				(vertex[i * (N_LENGTH + 1) + j].y() - vertex[i * (N_LENGTH + 1) + j + 1].y()) * (1 - off_x) +
				(vertex[(i + 1) * (N_LENGTH + 1) + j + 1].y() - vertex[i * (N_LENGTH + 1) + j + 1].y()) * off_z;
		else
			return vertex[(i + 1) * (N_LENGTH + 1) + j].y() +
				(vertex[(i + 1) * (N_LENGTH + 1) + j + 1].y() - vertex[(i + 1) * (N_LENGTH + 1) + j].y()) * off_x +
				(vertex[i * (N_LENGTH + 1) + j].y() - vertex[(i + 1) * (N_LENGTH + 1) + j].y()) * (1 - off_z);
	default:
		throw "Are you kidding?";
	}
}