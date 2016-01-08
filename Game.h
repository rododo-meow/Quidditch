#pragma once

#ifndef _GAME_H_
#define _GAME_H_

#include "gl.h"
#include <sys/timeb.h>
#include "Eigen/Eigen"
#include "GameParam.h"

class Label;
class OBJBall;
class OBJObject;
class Camera;
class Phys;
class Object;
class Flying;
class Ball;
class ProgressBar;
class Rect;
class Flag;
class ShadowRenderer;
class Table;
class Game {
public:
	static Game Instance;
private:
	Game() {};
	~Game();

private:
	int winID;
	int64_t score;
	float startTime = -1;
	float time = -1;
	float lastSpeedUpTime = -1;
	float lastCollisionTime = -1;
	uint64_t frames = 0;
	bool isGameOver = false;
	int WINDOW_WIDTH, WINDOW_HEIGHT;
	int mouse_x, mouse_y;

	// Callbacks
	static void glDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) { Instance._glDebug(source, type, id, severity, length, message); }
	void _glDebug(int source, int type, unsigned int id, int severity, unsigned int len, const char *message);
	static void glDisplay() { Instance._glDisplay(); }
	void _glDisplay();
	static void glMouseMove(int x, int y) { Instance._glMouseMove(x, y); }
	void _glMouseMove(int x, int y);
	static void glMouseButton(int button, int state, int x, int y) { 
		if (state == GLUT_DOWN)
			Instance._glMouseClick(button);
	}
	void _glMouseClick(int button);
	static void glKeyboard(unsigned char key, int x, int y) { Instance._glKeyboard(key); }
	static void glSpecial(int key, int x, int y) { Instance._glKeyboard(key | 0x100); }
	void _glKeyboard(int key);
	static void glFrame(int) {
		static unsigned long long first_time = 0;
		static unsigned long long last_time = 0;
		timeb this_time;
		ftime(&this_time);
		if (last_time == 0)
			last_time = 1000LL * this_time.time + this_time.millitm;
		if (first_time == 0)
			first_time = 1000LL * this_time.time + this_time.millitm;
		Instance.time = (1000LL * this_time.time + this_time.millitm - first_time) / 1000.f;
		if (Instance.startTime < 0)
			Instance.startTime = Instance.time;
		glutTimerFunc(1000 / GameParam::FPS, glFrame, 0);
		Instance._glFrame((1000LL * this_time.time + this_time.millitm - last_time) / 1000.f);
		last_time = 1000LL * this_time.time + this_time.millitm;
	}
	void _glFrame(float deltaTime);
	static void cameraFollow(void *that, Object *obj, const Eigen::Vector3f &oldP, const Eigen::Vector3f &newP) {
		Instance._cameraFollow(oldP, newP);
	};
	void _cameraFollow(const Eigen::Vector3f &oldP, const Eigen::Vector3f &newP);
	static void collision(void *that, Ball *a, Ball *b) {
		((Game *) that)->_collision(a, b);
	}
	void _collision(Ball *a, Ball *b);
	static void glClose() { Instance.destroy(); }

	// Renderer
	ShadowRenderer *renderer = NULL;

	// Camera
	Camera *camera = NULL;

	// Game objects
	Table *table = NULL;
	OBJBall *ball = NULL;
	OBJBall *runnerBalls[GameParam::N_RUNNERS] = {};
	OBJBall *lazyBalls[GameParam::N_LAZY] = {};
	Label *score_label = NULL;
	Phys *phys = NULL;
	Flying *flying = NULL;
	ProgressBar *cooldown_bar = NULL;
	Rect *mask = NULL;
	Label *gameover_label = NULL;
	Flag *flag = NULL;
	Label *fps_label = NULL;

	// Inits
	void initGameObject();
	void initCamera();
	void initGL();
	void initLight();

	void setScore(int64_t score);
	inline int64_t getScore() const { return score; }
	void speedUp();
	void gameOver();
public:
	void init();
	void destroy();
	float getTime() const { return time; }
};

#endif