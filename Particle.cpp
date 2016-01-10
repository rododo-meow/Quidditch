#include "Particle.h"
#include "Shader.h"
#include "Texture.h"
#include "gl.h"
#include "util.h"
#include "Game.h"
#include <iostream>

ParticleSystem::ParticleSystem() {
	vShader = Shader::loadFromFile(GL_VERTEX_SHADER, "Shader/particle.vert");
	gShader = Shader::loadFromFile(GL_GEOMETRY_SHADER, "Shader/particle.geom");
	fShader = Shader::loadFromFile(GL_FRAGMENT_SHADER, "Shader/particle.frag");
	program = new ShaderProgram(3, vShader, gShader, fShader);
	loc_time = 0;
}

ParticleSystem::~ParticleSystem() {
	SAFE_DELETE(vShader);
	SAFE_DELETE(gShader);
	SAFE_DELETE(fShader);
	SAFE_DELETE(program);
}

ParticleSystem::ParticleGroup::~ParticleGroup() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	texture->unload();
}

void ParticleSystem::render() {
	glLoadIdentity();
	program->enable();
	for (std::list<ParticleGroup*>::const_iterator it = particles.begin(); it != particles.end(); it++) {
		(*it)->texture->enable();
		glBindVertexArray((*it)->vao);
		glUniform1f(loc_time, (*it)->time);
		glDrawArrays(GL_POINTS, 0, (*it)->cnt);
	}
	glBindVertexArray(0);
	program->disable();
}

void ParticleSystem::update(float deltaTime) {
	for (std::list<ParticleGroup*>::const_iterator it = particles.begin(); it != particles.end();) {
		(*it)->time += deltaTime;
		if ((*it)->time > (*it)->maxTime) {
			delete *it;
			it = particles.erase(it);
		} else
			it++;
	}
}

void ParticleSystem::spawn(const ParticleParameter &parameter, const Eigen::Vector3f &position) {
	ParticleGroup *particle = new ParticleGroup();
	particle->texture = parameter.texture->clone();
	particle->cnt = rand(parameter.minCnt, parameter.maxCnt);
	glGenBuffers(1, &particle->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, particle->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleGroupVBO) + particle->cnt * sizeof(ParticleGroupVBO::particles[0]), NULL, GL_STATIC_DRAW);
	ParticleGroupVBO *map = (ParticleGroupVBO *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (size_t i = 0; i < particle->cnt; i++) {
		map->particles[i].position[0] = position.x();
		map->particles[i].position[1] = position.y();
		map->particles[i].position[2] = position.z();
		map->particles[i].size[0] = parameter.beginSize.x / Game::Instance.WINDOW_WIDTH * Game::Instance.WINDOW_HEIGHT;
		map->particles[i].size[1] = parameter.beginSize.y;
		map->particles[i].size[2] = parameter.endSize.x / Game::Instance.WINDOW_WIDTH * Game::Instance.WINDOW_HEIGHT;
		map->particles[i].size[3] = parameter.endSize.y;
		memcpy(map->particles[i].beginColor, &parameter.beginColor, sizeof(parameter.beginColor));
		memcpy(map->particles[i].endColor, &parameter.endColor, sizeof(parameter.endColor));
		float v = rand(parameter.minV, parameter.maxV), dir = (parameter.direction + rand(0, parameter.spread)) * ((float) M_PI / 180.f);
		map->particles[i].v[0] = v * cos(dir) / Game::Instance.WINDOW_WIDTH * Game::Instance.WINDOW_HEIGHT;
		map->particles[i].v[1] = v * sin(dir);
		map->particles[i].life[0] = rand(0.f, parameter.minLifetime * 0.1f);
		map->particles[i].life[1] = map->particles[i].life[0] + rand(parameter.minLifetime, parameter.maxLifetime);
		if (map->particles[i].life[1] > particle->maxTime)
			particle->maxTime = map->particles[i].life[1];
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glGenVertexArrays(1, &particle->vao);
	glBindVertexArray(particle->vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleGroupVBO::particles[0]), &((ParticleGroupVBO*)0)->particles[0].position);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleGroupVBO::particles[0]), &((ParticleGroupVBO*)0)->particles[0].size);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleGroupVBO::particles[0]), &((ParticleGroupVBO*)0)->particles[0].beginColor);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleGroupVBO::particles[0]), &((ParticleGroupVBO*)0)->particles[0].endColor);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleGroupVBO::particles[0]), &((ParticleGroupVBO*)0)->particles[0].v);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleGroupVBO::particles[0]), &((ParticleGroupVBO*)0)->particles[0].life);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	particles.push_back(particle);
}