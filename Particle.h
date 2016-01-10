#pragma once

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <list>
#include "Eigen/Eigen"

class Shader;
class ShaderProgram;
class Texture;
class ParticleSystem {
	struct ParticleGroup {
		size_t cnt;
		unsigned int vbo, vao;
		Texture *texture;
		float time, maxTime;
		~ParticleGroup();
	};
	struct ParticleGroupVBO {
		struct {
			float position[3];
			float size[4];
			float beginColor[4], endColor[4];
			float v[2];
			float life[2];
		} particles[0];
	};
	std::list<ParticleGroup*> particles;
	Shader *vShader, *gShader, *fShader;
	ShaderProgram *program;
	unsigned int loc_time;
public:
	struct ParticleParameter {
		size_t minCnt, maxCnt;
		float minV, maxV;
		struct {
			float r, g, b, a;
		} beginColor, endColor;
		float direction;
		float spread;
		float a;
		Texture *texture;
		float minLifetime, maxLifetime;
		struct {
			float x, y;
		} beginSize, endSize;
	};
	ParticleSystem();
	~ParticleSystem();
	void render();
	void update(float deltaTime);
	void spawn(const ParticleParameter &parameter, const Eigen::Vector3f &position);
};

#endif