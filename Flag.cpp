#include "Flag.h"
#include <iostream>
#include "util.h"
#include "Transforms.h"
#include "OBJDrawable.h"
#include "Texture.h"

using namespace Eigen;

const float Flag::K = 5.f;
const float Flag::M = .1f;
const float Flag::g = 5.f;
const float Flag::RESISTANCE_FACTOR = 0.6f;
const float Flag::WIND_FORCE = 1.7f;

Flag::~Flag() {
	gluDeleteNurbsRenderer(nurbs);
	gluDeleteQuadric(quad);
	texture->unload();
}

void __stdcall Flag::nurbsError(GLenum code) {
	std::cout << code << std::endl;
	std::cout << gluErrorString(code) << std::endl;
}

void Flag::initPhys(float flag_length, float flag_width) {
	WIDTH_ORI_D = flag_width / N_WIDTH_POINTS;
	LENGTH_ORI_D = flag_length / N_LENGTH_POINTS;
	WIDTH_MIN_D = WIDTH_ORI_D * 0.95f;
	WIDTH_MAX_D = WIDTH_ORI_D * 1.05f;
	LENGTH_MIN_D = LENGTH_ORI_D * 0.95f;
	LENGTH_MAX_D = LENGTH_ORI_D * 1.05f;
	for (int i = 0; i < N_WIDTH_POINTS; i++)
		for (int j = 0; j < N_LENGTH_POINTS; j++) {
			masses[i][j] = Vector3f(j * flag_length / (N_LENGTH_POINTS - 1), i * flag_width / (N_WIDTH_POINTS - 1), 0);
			masses_v[i][j].setZero();
		}
}

void Flag::updateKeyPoints() {
	for (int i = 0; i < N_WIDTH_POINTS; i++)
		for (int j = 0; j < N_LENGTH_POINTS; j++) {
			key_points[ORDER - 1 + i][ORDER - 1 + j][0] = masses[i][j].x();
			key_points[ORDER - 1 + i][ORDER - 1 + j][1] = masses[i][j].y();
			key_points[ORDER - 1 + i][ORDER - 1 + j][2] = masses[i][j].z();
		}
	for (int i = 0; i < N_WIDTH_POINTS; i++)
		for (int j = 0; j < ORDER - 1; j++) {
			key_points[ORDER - 1 + i][j][0] = key_points[ORDER - 1 + i][ORDER - 1][0];
			key_points[ORDER - 1 + i][j][1] = key_points[ORDER - 1 + i][ORDER - 1][1];
			key_points[ORDER - 1 + i][j][2] = key_points[ORDER - 1 + i][ORDER - 1][2];
			key_points[ORDER - 1 + i][ORDER - 1 + N_LENGTH_POINTS + j][0] = key_points[ORDER - 1 + i][ORDER - 1 + N_LENGTH_POINTS - 1][0];
			key_points[ORDER - 1 + i][ORDER - 1 + N_LENGTH_POINTS + j][1] = key_points[ORDER - 1 + i][ORDER - 1 + N_LENGTH_POINTS - 1][1];
			key_points[ORDER - 1 + i][ORDER - 1 + N_LENGTH_POINTS + j][2] = key_points[ORDER - 1 + i][ORDER - 1 + N_LENGTH_POINTS - 1][2];
		}
	for (int i = 0; i < ORDER - 1; i++) {
		memcpy(&key_points[i][0][0], &key_points[ORDER - 1][0][0], sizeof(key_points[0]));
		memcpy(&key_points[ORDER - 1 + N_WIDTH_POINTS + i][0][0], &key_points[ORDER - 1 + N_WIDTH_POINTS - 1][0][0], sizeof(key_points[0]));
	}
}

Flag::Flag(float flag_height, float flag_length, float flag_width, const std::string &tex_path) : flag_height(flag_height), flag_length(flag_length), flag_width(flag_width) {
	bind(this);
	texture = Texture::load(tex_path.c_str());
	initPhys(flag_length, flag_width);
	updateKeyPoints();
	for (int i = 0; i < N_WIDTH_POINTS; i++)
		for (int j = 0; j < N_LENGTH_POINTS; j++) {
			tex_key_points[ORDER - 1 + i][ORDER - 1 + j][0] = 1.f * j / (N_LENGTH_POINTS - 1);
			tex_key_points[ORDER - 1 + i][ORDER - 1 + j][1] = 1.f * i / (N_WIDTH_POINTS - 1);
		}
	for (int i = 0; i < N_WIDTH_POINTS; i++)
		for (int j = 0; j < ORDER - 1; j++) {
			tex_key_points[ORDER - 1 + i][j][0] = tex_key_points[ORDER - 1 + i][ORDER - 1][0];
			tex_key_points[ORDER - 1 + i][j][1] = tex_key_points[ORDER - 1 + i][ORDER - 1][1];
			tex_key_points[ORDER - 1 + i][ORDER - 1 + N_LENGTH_POINTS + j][0] = tex_key_points[ORDER - 1 + i][ORDER - 1 + N_LENGTH_POINTS - 1][0];
			tex_key_points[ORDER - 1 + i][ORDER - 1 + N_LENGTH_POINTS + j][1] = tex_key_points[ORDER - 1 + i][ORDER - 1 + N_LENGTH_POINTS - 1][1];
		}
	for (int i = 0; i < ORDER - 1; i++)
		for (int j = 0; j < ORDER - 1 + N_LENGTH_POINTS + ORDER - 1; j++) {
			tex_key_points[i][j][0] = tex_key_points[ORDER - 1][j][0];
			tex_key_points[i][j][1] = tex_key_points[ORDER - 1][j][1];
			tex_key_points[ORDER - 1 + N_WIDTH_POINTS + i][j][0] = tex_key_points[ORDER - 1 + N_WIDTH_POINTS - 1][j][0];
			tex_key_points[ORDER - 1 + N_WIDTH_POINTS + i][j][1] = tex_key_points[ORDER - 1 + N_WIDTH_POINTS - 1][j][1];
		}
	for (int i = 0; i < ORDER - 1; i++)
		sknot[i] = tknot[i] = 0;
	for (int i = 0; i < ORDER + N_WIDTH_POINTS; i++)
		sknot[ORDER - 1 + i] = 1.0f * i / (ORDER + N_WIDTH_POINTS - 1);
	for (int i = 0; i < ORDER + N_LENGTH_POINTS; i++)
		tknot[ORDER - 1 + i] = 1.0f * i / (ORDER + N_LENGTH_POINTS - 1);
	for (int i = 0; i < ORDER - 1; i++)
		sknot[ORDER - 1 + ORDER + N_WIDTH_POINTS + i] = tknot[ORDER - 1 + ORDER + N_LENGTH_POINTS + i] = 1;
	nurbs = gluNewNurbsRenderer();
	gluNurbsProperty(nurbs, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
	gluNurbsProperty(nurbs, GLU_U_STEP, 30);
	gluNurbsProperty(nurbs, GLU_V_STEP, 30);
	gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, GLU_FILL);
	gluNurbsCallback(nurbs, GLU_ERROR, (void(__stdcall *)()) nurbsError);
	quad = gluNewQuadric();
}

void Flag::update(float deltaTime) {
	// apply gravity
	for (int i = 0; i < N_WIDTH_POINTS; i++)
		for (int j = 1; j < N_LENGTH_POINTS; j++)
			masses_v[i][j] += Vector3f(0, -g * deltaTime, 0);

	// apply wind
	Vector3f WIND = vec4To3(matRotateAroundY(rand(-30.f, 30.f)) * Vector4f(rand(WIND_FORCE * 0.7f, WIND_FORCE) / M * deltaTime, 0, 0, 1));
	for (int i = 0; i < N_WIDTH_POINTS; i++)
		for (int j = 1; j < N_LENGTH_POINTS; j++)
			masses_v[i][j] += WIND;

	// apply elasticity
	for (int i = 1; i < N_WIDTH_POINTS - 1; i++)
		for (int j = 1; j < N_LENGTH_POINTS - 1; j++) {
			masses_v[i][j] += (masses[i - 1][j] - masses[i][j]).normalized() * (((masses[i - 1][j] - masses[i][j]).norm() - WIDTH_ORI_D) * K / M * deltaTime);
			masses_v[i][j] += (masses[i + 1][j] - masses[i][j]).normalized() * (((masses[i + 1][j] - masses[i][j]).norm() - WIDTH_ORI_D) * K / M * deltaTime);
			masses_v[i][j] += (masses[i][j - 1] - masses[i][j]).normalized() * (((masses[i][j - 1] - masses[i][j]).norm() - LENGTH_ORI_D) * K / M * deltaTime);
			masses_v[i][j] += (masses[i][j + 1] - masses[i][j]).normalized() * (((masses[i][j + 1] - masses[i][j]).norm() - LENGTH_ORI_D) * K / M * deltaTime);
		}
	for (int i = 1; i < N_WIDTH_POINTS - 1; i++) {
		masses_v[i][N_LENGTH_POINTS - 1] += (masses[i - 1][N_LENGTH_POINTS - 1] - masses[i][N_LENGTH_POINTS - 1]).normalized() * (((masses[i - 1][N_LENGTH_POINTS - 1] - masses[i][N_LENGTH_POINTS - 1]).norm() - WIDTH_ORI_D) * K / M * deltaTime);
		masses_v[i][N_LENGTH_POINTS - 1] += (masses[i + 1][N_LENGTH_POINTS - 1] - masses[i][N_LENGTH_POINTS - 1]).normalized() * (((masses[i + 1][N_LENGTH_POINTS - 1] - masses[i][N_LENGTH_POINTS - 1]).norm() - WIDTH_ORI_D) * K / M * deltaTime);
		masses_v[i][N_LENGTH_POINTS - 1] += (masses[i][N_LENGTH_POINTS - 2] - masses[i][N_LENGTH_POINTS - 1]).normalized() * (((masses[i][N_LENGTH_POINTS - 2] - masses[i][N_LENGTH_POINTS - 1]).norm() - LENGTH_ORI_D) * K / M * deltaTime);
	}
	for (int j = 1; j < N_LENGTH_POINTS - 1; j++) {
		masses_v[0][j] += (masses[1][j] - masses[0][j]).normalized() * (((masses[1][j] - masses[0][j]).norm() - WIDTH_ORI_D) * K / M * deltaTime);
		masses_v[0][j] += (masses[0][j - 1] - masses[0][j]).normalized() * (((masses[0][j - 1] - masses[0][j]).norm() - LENGTH_ORI_D) * K / M * deltaTime);
		masses_v[0][j] += (masses[0][j + 1] - masses[0][j]).normalized() * (((masses[0][j + 1] - masses[0][j]).norm() - LENGTH_ORI_D) * K / M * deltaTime);
		masses_v[N_WIDTH_POINTS - 1][j] += (masses[N_WIDTH_POINTS - 2][j] - masses[N_WIDTH_POINTS - 1][j]).normalized() * (((masses[N_WIDTH_POINTS - 2][j] - masses[N_WIDTH_POINTS - 1][j]).norm() - WIDTH_ORI_D) * K / M * deltaTime);
		masses_v[N_WIDTH_POINTS - 1][j] += (masses[N_WIDTH_POINTS - 1][j - 1] - masses[N_WIDTH_POINTS - 1][j]).normalized() * (((masses[N_WIDTH_POINTS - 1][j - 1] - masses[N_WIDTH_POINTS - 1][j]).norm() - LENGTH_ORI_D) * K / M * deltaTime);
		masses_v[N_WIDTH_POINTS - 1][j] += (masses[N_WIDTH_POINTS - 1][j + 1] - masses[N_WIDTH_POINTS - 1][j]).normalized() * (((masses[N_WIDTH_POINTS - 1][j + 1] - masses[N_WIDTH_POINTS - 1][j]).norm() - LENGTH_ORI_D) * K / M * deltaTime);
	}
	masses_v[0][N_LENGTH_POINTS - 1] += (masses[0][N_LENGTH_POINTS - 2] - masses[0][N_LENGTH_POINTS - 1]).normalized() * (((masses[0][N_LENGTH_POINTS - 2] - masses[0][N_LENGTH_POINTS - 1]).norm() - LENGTH_ORI_D) * K / M * deltaTime);
	masses_v[0][N_LENGTH_POINTS - 1] += (masses[1][N_LENGTH_POINTS - 1] - masses[0][N_LENGTH_POINTS - 1]).normalized() * (((masses[1][N_LENGTH_POINTS - 1] - masses[0][N_LENGTH_POINTS - 1]).norm() - WIDTH_ORI_D) * K / M * deltaTime);
	masses_v[N_WIDTH_POINTS - 1][N_LENGTH_POINTS - 1] += (masses[N_WIDTH_POINTS - 1][N_LENGTH_POINTS - 2] - masses[N_WIDTH_POINTS - 1][N_LENGTH_POINTS - 1]).normalized() * (((masses[N_WIDTH_POINTS - 1][N_LENGTH_POINTS - 2] - masses[N_WIDTH_POINTS - 1][N_LENGTH_POINTS - 1]).norm() - LENGTH_ORI_D) * K / M * deltaTime);
	masses_v[N_WIDTH_POINTS - 1][N_LENGTH_POINTS - 1] += (masses[N_WIDTH_POINTS - 2][N_LENGTH_POINTS - 1] - masses[N_WIDTH_POINTS - 1][N_LENGTH_POINTS - 1]).normalized() * (((masses[N_WIDTH_POINTS - 2][N_LENGTH_POINTS - 1] - masses[N_WIDTH_POINTS - 1][N_LENGTH_POINTS - 1]).norm() - WIDTH_ORI_D) * K / M * deltaTime);

	// apply resistance
	for (int i = 0; i < N_WIDTH_POINTS; i++)
		for (int j = 1; j < N_LENGTH_POINTS; j++)
			masses_v[i][j] += -masses_v[i][j] * RESISTANCE_FACTOR * masses_v[i][j].norm() / M * deltaTime;

	for (int i = 0; i < N_WIDTH_POINTS; i++)
		for (int j = 0; j < N_LENGTH_POINTS; j++)
			masses[i][j] += masses_v[i][j] * deltaTime;

	for (int j = 0; j < N_LENGTH_POINTS - 1; j++)
		if ((masses[N_WIDTH_POINTS - 1][j + 1] - masses[N_WIDTH_POINTS - 1][j]).norm() > LENGTH_MAX_D) {
			masses[N_WIDTH_POINTS - 1][j + 1] = masses[N_WIDTH_POINTS - 1][j] + (masses[N_WIDTH_POINTS - 1][j + 1] - masses[N_WIDTH_POINTS - 1][j]).normalized() * LENGTH_MAX_D;
			if ((masses_v[N_WIDTH_POINTS - 1][j + 1] - masses_v[N_WIDTH_POINTS - 1][j]).dot(masses[N_WIDTH_POINTS - 1][j] - masses[N_WIDTH_POINTS - 1][j + 1]) < 0)
				masses_v[N_WIDTH_POINTS - 1][j + 1] -= (masses[N_WIDTH_POINTS - 1][j + 1] - masses[N_WIDTH_POINTS - 1][j]).normalized() * (masses_v[N_WIDTH_POINTS - 1][j + 1] - masses_v[N_WIDTH_POINTS - 1][j]).dot((masses[N_WIDTH_POINTS - 1][j + 1] - masses[N_WIDTH_POINTS - 1][j]).normalized());
		}

	// fix elasticity when D is too long
	for (int i = 0; i < N_WIDTH_POINTS - 1; i++)
		for (int j = 0; j < N_LENGTH_POINTS - 1; j++) {
			if ((masses[i + 1][j] - masses[i][j]).norm() > WIDTH_MAX_D) {
				masses[i + 1][j] = masses[i][j] + (masses[i + 1][j] - masses[i][j]).normalized() * WIDTH_MAX_D;
				if ((masses_v[i + 1][j] - masses_v[i][j]).dot(masses[i + 1][j] - masses[i][j]) > 0)
					masses_v[i + 1][j] -= (masses[i + 1][j] - masses[i][j]).normalized() * (masses_v[i + 1][j] - masses_v[i][j]).dot((masses[i + 1][j] - masses[i][j]).normalized());
			}
			if ((masses[i][j + 1] - masses[i][j]).norm() > LENGTH_MAX_D) {
				masses[i][j + 1] = masses[i][j] + (masses[i][j + 1] - masses[i][j]).normalized() * LENGTH_MAX_D;
				if ((masses_v[i][j + 1] - masses_v[i][j]).dot(masses[i][j] - masses[i][j + 1]) < 0)
					masses_v[i][j + 1] -= (masses[i][j + 1] - masses[i][j]).normalized() * (masses_v[i][j + 1] - masses_v[i][j]).dot((masses[i][j + 1] - masses[i][j]).normalized());
			}
		}

	// fix elasticity when D is too short
	for (int i = 0; i < N_WIDTH_POINTS - 1; i++)
		for (int j = 0; j < N_LENGTH_POINTS - 1; j++) {
			if ((masses[i + 1][j] - masses[i][j]).norm() < WIDTH_MIN_D) {
				masses[i + 1][j] = masses[i][j] + (masses[i + 1][j] - masses[i][j]).normalized() * WIDTH_MIN_D;
				if ((masses_v[i + 1][j] - masses_v[i][j]).dot(masses[i + 1][j] - masses[i][j]) < 0)
					masses_v[i + 1][j] -= (masses[i + 1][j] - masses[i][j]).normalized() * (masses_v[i + 1][j] - masses_v[i][j]).dot((masses[i + 1][j] - masses[i][j]).normalized());
			}
			if ((masses[i][j + 1] - masses[i][j]).norm() < LENGTH_MIN_D) {
				masses[i][j + 1] = masses[i][j] + (masses[i][j + 1] - masses[i][j]).normalized() * LENGTH_MIN_D;
				if ((masses_v[i][j + 1] - masses_v[i][j]).dot(masses[i][j] - masses[i][j + 1]) < 0)
					masses_v[i][j + 1] -= (masses[i][j + 1] - masses[i][j]).normalized() * (masses_v[i][j + 1] - masses_v[i][j]).dot((masses[i][j + 1] - masses[i][j]).normalized());
			}
		}
			
	updateKeyPoints();
}

void Flag::_render() {
	return;
	glEnable(GL_AUTO_NORMAL);
	glDisable(GL_CULL_FACE);
	glTranslatef(0, flag_height - flag_width, 0);
	texture->enable();
	gluBeginSurface(nurbs);
	gluNurbsSurface(
		nurbs,
		ORDER + ORDER - 1 + N_WIDTH_POINTS + ORDER - 1,
		sknot,
		ORDER + ORDER - 1 + N_LENGTH_POINTS + ORDER - 1,
		tknot,
		(ORDER - 1 + N_LENGTH_POINTS + ORDER - 1) * 2,
		2,
		(float *)tex_key_points,
		ORDER,
		ORDER,
		GL_MAP2_TEXTURE_COORD_2);
	gluNurbsSurface(
		nurbs,
		ORDER + ORDER - 1 + N_WIDTH_POINTS + ORDER - 1,
		sknot,
		ORDER + ORDER - 1 + N_LENGTH_POINTS + ORDER - 1,
		tknot,
		(ORDER - 1 + N_LENGTH_POINTS + ORDER - 1) * 3,
		3,
		(float *) key_points,
		ORDER,
		ORDER,
		GL_MAP2_VERTEX_3);
	gluEndSurface(nurbs);
	glTranslatef(0, flag_width - flag_height, 0);
	glEnable(GL_CULL_FACE);
	glDisable(GL_AUTO_NORMAL);
}