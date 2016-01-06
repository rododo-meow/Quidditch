#include "Game.h"
#include "Label.h"
#include "OBJBall.h"
#include "Camera.h"
#include "Eigen/Eigen"
#include "Phys.h"
#include "util.h"
#include "Flying.h"
#include "OBJObject.h"
#include <sstream>
#include "ProgressBar.h"
#include "Rect.h"
#include "Flag.h"
#include "Shadow.h"
#include "Table.h"

using namespace std;
using namespace Eigen;
using namespace GameParam;

Game Game::Instance;

Game::~Game() {
	destroy();
}

#define SAFE_DELETE(x) if ((x)) { \
	delete (x); \
	(x) = NULL; \
}

void Game::destroy() {
	SAFE_DELETE(camera);
	SAFE_DELETE(table);
	SAFE_DELETE(ball);
	for (int i = 0; i < N_RUNNERS; i++)
		SAFE_DELETE(runnerBalls[i]);
	for (int i = 0; i < N_LAZY; i++)
		SAFE_DELETE(lazyBalls[i]);
	SAFE_DELETE(score_label);
	SAFE_DELETE(phys);
	SAFE_DELETE(flying);
	SAFE_DELETE(cooldown_bar);
	SAFE_DELETE(mask);
	SAFE_DELETE(flag);
	SAFE_DELETE(renderer);
	SAFE_DELETE(fps_label);
}

void Game::_glDisplay() {
	camera->setMatrix();

	// render game objects
	renderer->stage1();
	table->render();
	ball->render();
	for (int i = 0; i < N_RUNNERS; i++)
		runnerBalls[i]->render();
	for (int i = 0; i < N_LAZY; i++)
		lazyBalls[i]->render();
	flying->render();
	renderer->setFlag(true);
	flag->render();
	renderer->setFlag(false);

	renderer->stage2();
	table->render();
	ball->render();
	for (int i = 0; i < N_RUNNERS; i++)
		runnerBalls[i]->render();
	for (int i = 0; i < N_LAZY; i++)
		lazyBalls[i]->render();
	flying->render();
	renderer->setFlag(true);
	flag->render();
	renderer->setFlag(false);

	renderer->stage3();
	table->render();
	ball->render();
	for (int i = 0; i < N_RUNNERS; i++)
		runnerBalls[i]->render();
	for (int i = 0; i < N_LAZY; i++)
		lazyBalls[i]->render();
	flying->render();
	renderer->setFlag(true);
	flag->render();
	renderer->setFlag(false);

	renderer->end();

	// render UI
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cooldown_bar->render();
	mask->render();
	score_label->render();
	if (isGameOver)
		gameover_label->render();
	fps_label->render();

	glFlush();
	glutSwapBuffers();
}

void Game::_glFrame(float deltaTime) {
	// change direction and velocity of main ball
	Eigen::Vector3f ballV = phys->getVelocity(ball);
	ballV(1, 0) = 0;
	if (!isGameOver) {
		Eigen::Vector3f front = -camera->getN();
		front(1, 0) = 0;
		front.normalize();
		Eigen::Vector3f cross = front.cross(ballV.normalized());
		if (cross(1, 0) > 0.1f)
			ballV = vec4To3(matRotate(Vector3f({ 0.f, 1.f, 0.f }), TURNING_FACTOR / ballV.norm()) * vec3To4(ballV));
		else if (cross(1, 0) < -0.1f)
			ballV = vec4To3(matRotate(Vector3f({ 0.f, -1.f, 0.f }), TURNING_FACTOR / ballV.norm()) * vec3To4(ballV));
	}
	if (ballV.norm() < INIT_VELOCITY)
		ballV = (ballV.norm() + deltaTime * ACCELERATION) * ballV.normalized();
	else if (ballV.norm() > INIT_VELOCITY)
		ballV = (ballV.norm() - deltaTime * ACCELERATION) * ballV.normalized();
	phys->setVelocity(ball, ballV);

	// change direction and velocity of runner balls
	for (int i = 0; i < N_RUNNERS; i++) {
		Eigen::Vector3f ballV = phys->getVelocity(runnerBalls[i]);
		Eigen::Vector3f front = ball->getPosition() - runnerBalls[i]->getPosition();
		front.normalize();
		front = vec4To3(matRotateAroundY(rand(-30, 30)) * vec3To4(front));
		Eigen::Vector3f cross = front.cross(ballV.normalized());
		if (cross(1, 0) > 0.1f)
			ballV = vec4To3(matRotate(Vector3f({ 0.f, 1.f, 0.f }), RUNNER_TURNING_FACTOR / ballV.norm()) * vec3To4(ballV));
		else if (cross(1, 0) < -0.1f)
			ballV = vec4To3(matRotate(Vector3f({ 0.f, -1.f, 0.f }), RUNNER_TURNING_FACTOR / ballV.norm()) * vec3To4(ballV));
		if (ballV.norm() < RUNNER_INIT_VELOCITY)
			ballV = (ballV.norm() + deltaTime * RUNNER_ACCELERATION) * ballV.normalized();
		else if (ballV.norm() > RUNNER_INIT_VELOCITY)
			ballV = (ballV.norm() - deltaTime * RUNNER_ACCELERATION) * ballV.normalized();
		phys->setVelocity(runnerBalls[i], ballV);
	}

	// change velocity of lazy balls
	for (int i = 0; i < N_RUNNERS; i++) {
		Eigen::Vector3f ballV = phys->getVelocity(lazyBalls[i]);
		if (ballV.norm() > 0)
			ballV = (ballV.norm() - deltaTime * RUNNER_ACCELERATION) * ballV.normalized();
		phys->setVelocity(lazyBalls[i], ballV);
	}

	// let flying think its life
	flying->think(deltaTime, phys);

	// let flag do phys simulation
	flag->update(deltaTime);

	phys->update(deltaTime);

	// update cooldown bar
	if (lastSpeedUpTime < 0 || time - lastSpeedUpTime >= SPEED_UP_COOLDOWN)
		cooldown_bar->setProgress(1);
	else
		cooldown_bar->setProgress((time - lastSpeedUpTime) / SPEED_UP_COOLDOWN);

	// if game over, update mask
	if (isGameOver && mask->getColor()[3] < FADE_OUT_ALPHA)
		mask->setColor(0, 0, 0, mask->getColor()[3] + FADE_OUT_ALPHA / FADE_OUT_DURATION * deltaTime);

	frames++;
	fps_label->text = ftos(frames / (time - startTime));
	glutPostRedisplay();
}

void Game::_glMouseMove(int x, int y) {
	if (x < WINDOW_WIDTH / 4)
	{
		mouse_x += WINDOW_WIDTH / 4;
		x += WINDOW_WIDTH / 4;
		POINT p;
		GetCursorPos(&p);
		SetCursorPos(p.x + WINDOW_WIDTH / 4, p.y);
	}
	if (x > WINDOW_WIDTH / 4 * 3)
	{
		mouse_x -= WINDOW_WIDTH / 4;
		x -= WINDOW_WIDTH / 4;
		POINT p;
		GetCursorPos(&p);
		SetCursorPos(p.x - WINDOW_WIDTH / 4, p.y);
	}
	if (y < WINDOW_HEIGHT / 4)
	{
		mouse_y += WINDOW_HEIGHT / 4;
		y += WINDOW_HEIGHT / 4;
		POINT p;
		GetCursorPos(&p);
		SetCursorPos(p.x, p.y + WINDOW_HEIGHT / 4);
	}
	if (y > WINDOW_HEIGHT / 4 * 3)
	{
		mouse_y -= WINDOW_HEIGHT / 4;
		y -= WINDOW_HEIGHT / 4;
		POINT p;
		GetCursorPos(&p);
		SetCursorPos(p.x, p.y - WINDOW_HEIGHT / 4);
	}
	Eigen::Matrix4f mat = camera->getObservationMat() *
		matRotate({ 0.f, 1.f, 0.f }, (mouse_x - x) * MOUSE_SENSITIVIE);
	if ((mat(2, 1) > 0.1f || (mouse_y - y) < 0) && (mat(2, 1) < 0.9f || (mouse_y - y) > 0))
		mat = matRotate({ 1.f, 0.f, 0.f }, (mouse_y - y) * MOUSE_SENSITIVIE) * mat;
	float dis = (camera->getPosition() - ball->getPosition()).norm();
	camera->setObservation(mat);
	camera->setPosition(ball->getPosition());
	camera->backward(dis);
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}

void Game::_glKeyboard(int key) {
	switch (key) {
	case 'w':
	case GLUT_KEY_UP | 0x100:
		if ((camera->getPosition() - ball->getPosition()).norm() > MIN_VIEW_DISTANCE)
			camera->forward(0.1f);
		break;
	case 's':
	case GLUT_KEY_DOWN | 0x100:
		if ((camera->getPosition() - ball->getPosition()).norm() < MAX_VIEW_DISTANCE)
			camera->backward(0.1f);
		break;
	case 'a':
		camera->left(0.1f);
		break;
	case 'd':
		camera->right(0.1f);
		break;
	case VK_ESCAPE:
		glutLeaveMainLoop();
		break;
	case 'r':
		init();
		break;
	}
	glutPostRedisplay();
}

void Game::_cameraFollow(const Vector3f &oldP, const Vector3f &newP) {
	float dis = (camera->getPosition() - oldP).norm();
	camera->setPosition(newP);
	camera->backward(dis);
}

void Game::initGL() {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set background color and depth
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(20.f);
}

float tmp[3][3] = {
	0, 0, 0,
	0, 0.2f, 0,
	0, 0, 0
};

void Game::initGameObject() {
	phys = new Phys();
	phys->addAfterCollision(collision, this);

	table = new Table(TABLE_OBJ_FILENAME, FACE_TEXTURE_FILENAME, 2, 2, &tmp[0][0]);

	ball = new OBJBall(BALL_OBJ_FILENAME, BALL_RADIUS);
	ball->teleport(Vector3f({ 0.f, TABLE_FACE_Y + BALL_RADIUS, 0.f }));
	ball->data = ball;
	phys->addBall(ball);
	phys->setVelocity(ball, Vector3f({ -INIT_VELOCITY, 0.f, 0.f }));

	for (int i = 0; i < N_RUNNERS; i++) {
		runnerBalls[i] = new OBJBall(BALL_OBJ_FILENAME, BALL_RADIUS);
		runnerBalls[i]->data = runnerBalls;
		runnerBalls[i]->teleport(Vector3f({
			rand(-TABLE_LENGTH / 2 + BALL_RADIUS, TABLE_LENGTH / 2 - BALL_RADIUS),
			TABLE_FACE_Y + BALL_RADIUS,
			rand(-TABLE_WIDTH / 2 + BALL_RADIUS, TABLE_WIDTH / 2 - BALL_RADIUS) }));
		runnerBalls[i]->drawable->setColor(1.f, 0.f, 0.f, 1.f);
		phys->addBall(runnerBalls[i]);
		phys->setVelocity(runnerBalls[i], vec4To3(matRotateAroundY(360.f * rand() / RAND_MAX) * Vector4f({ -RUNNER_INIT_VELOCITY, 0.f, 0.f, 1.f })));
	}

	for (int i = 0; i < N_LAZY; i++) {
		lazyBalls[i] = new OBJBall(BALL_OBJ_FILENAME, BALL_RADIUS);
		lazyBalls[i]->data = lazyBalls;
		lazyBalls[i]->teleport(Vector3f({
			rand(-TABLE_LENGTH / 2 + BALL_RADIUS, TABLE_LENGTH / 2 - BALL_RADIUS),
			TABLE_FACE_Y + BALL_RADIUS,
			rand(-TABLE_WIDTH / 2 + BALL_RADIUS, TABLE_WIDTH / 2 - BALL_RADIUS) }));
		lazyBalls[i]->drawable->setColor(0.f, 0.f, 1.f, 1.f);
		phys->addBall(lazyBalls[i]);
		phys->setVelocity(lazyBalls[i], Vector3f({ 0.f, 0.f, 0.f }));
	}

	score_label = new Label();
	score_label->setColor(1, 1, 1, 1);
	score_label->setPosition(Vector2f({ SCORE_X, SCORE_Y }));
	setScore(getScore());

	flying = new Flying(BALL_OBJ_FILENAME, BALL_RADIUS);
	flying->data = flying;
	phys->addBall(flying);
	flying->setPosition(Vector3f({ 
		rand(FLYING_MIN_X, FLYING_MAX_X),
		rand(TABLE_FACE_Y + BALL_RADIUS + FLYING_MIN_FLY_HEIGHT, TABLE_FACE_Y + BALL_RADIUS + FLYING_MAX_FLY_HEIGHT),
		rand(FLYING_MIN_Z, FLYING_MAX_Z)}));
	flying->drawable->setColor(1, 1, 0.3f, 1);

	cooldown_bar = new ProgressBar();
	cooldown_bar->setPosition(Vector2f({ COOLDOWN_X, COOLDOWN_Y }));
	cooldown_bar->setWidth(COOLDOWN_WIDTH);
	cooldown_bar->setHeight(COOLDOWN_HEIGHT);
	cooldown_bar->setLeftColor(COOLDOWN_LEFT_COLOR);
	cooldown_bar->setRightColor(COOLDOWN_RIGHT_COLOR);

	mask = new Rect();
	mask->setWidth(2);
	mask->setHeight(2);
	mask->setColor(0, 0, 0, 0);

	gameover_label = new Label();
	gameover_label->setColor(1, 1, 1, 1);
	gameover_label->setPosition(Vector2f({ GAMEOVER_LABEL_X, GAMEOVER_LABEL_Y }));

	fps_label = new Label();
	fps_label->setColor(1, 1, 1, 1);
	fps_label->setPosition(Vector2f({ FPS_LABEL_X, FPS_LABEL_Y }));

	flag = new Flag(FLAG_HEIGHT, FLAG_LENGTH, FLAG_WIDTH, "ObjModel/flag.jpg");
	flag->setPosition({ FLAG_POSITION_X, FLAG_POSITION_Y, FLAG_POSITION_Z });
	flag->setAmbient(0.3f);
	flag->setDiffuse(0.5f);
}

void Game::initLight() {
	GLfloat noLight[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, noLight);

	renderer->setLightPosition(3.0f, 3.0f, 3.0f, 0.0f);
	renderer->setLightAmbient(1, 1, 1, 1);
	renderer->setLightDiffuse(1, 1, 1, 1);
	renderer->setLightSpecular(.1f, .1f, .1f, .1f);
}

void Game::initCamera() {
	camera = new Camera();
	ball->addPositionChanged(cameraFollow, NULL);
	camera->setPerspective(30.f, ((float)WINDOW_WIDTH) / WINDOW_HEIGHT, .03f, 20.f);
	camera->setUVN(Vector3f({ -5.f, -5.f, 0.f }), Vector3f({ 0.f, 1.f, 0.f }));
	camera->setPosition(ball->getPosition());
	camera->backward(DEFAULT_VIEW_DISTANCE);
}

void Game::setScore(int64_t score) {
	this->score = score;
	ostringstream os;
	os << "Score: " << score;
	score_label->text = os.str();
}

void Game::_collision(Ball *a, Ball *b) {
	if (isGameOver)
		return;
	if (b->data == ball)
		b = a, a = ball;
	if (a->data == ball) {
		if (b->data == runnerBalls)
			setScore(getScore() - PENALTY_SCORE);
		else if (b->data == lazyBalls) {
			Vector3f va = phys->getVelocity(a), vb = phys->getVelocity(b), ccline = (a->getPosition() - b->getPosition()).normalized();
			float v = abs(va.dot(ccline) - vb.dot(ccline));
			setScore(getScore() + (int) (v * v * SCORE_FACTOR + BASE_SCORE));
		} else if (b->data == flying) {
			setScore(getScore() + FLYING_SCORE);
			gameOver();
		}
	}
}

void Game::gameOver() {
	ostringstream os;
	os << "Game over, your score is: " << getScore() << endl << "Press Esc to exit, R to restart";
	gameover_label->text = os.str();
	isGameOver = true;
}

void Game::speedUp() {
	if (lastSpeedUpTime < 0 || getTime() - lastSpeedUpTime > SPEED_UP_COOLDOWN) {
		lastSpeedUpTime = getTime();
		phys->setVelocity(ball, phys->getVelocity(ball).normalized() * SPEED_UP_VELOCITY);
	}
}

void Game::_glMouseClick(int button) {
	if (isGameOver)
		return;
	if (button == 0)
		speedUp();
}

void Game::_glDebug(int source, int type, unsigned int id, int severity, unsigned int len, const char *message) {
	printf("GL debug: %s\n", message);
}

void Game::init() {
	if (!winID) {
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
		if (FULLSCREEN) {
			glutGameModeString(":32");
			if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE) == 0) {
				std::cout << "Can't enter game mode\n";
				exit(0);
			}
			winID = glutEnterGameMode();
			WINDOW_WIDTH = glutGameModeGet(GLUT_GAME_MODE_WIDTH);
			WINDOW_HEIGHT = glutGameModeGet(GLUT_GAME_MODE_HEIGHT);
			glutSetWindowTitle("Quidditch");

			// init cursor
			mouse_x = WINDOW_WIDTH / 2;
			mouse_y = WINDOW_HEIGHT / 2;
			SetCursorPos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
			glutSetCursor(GLUT_CURSOR_NONE);
		} else {
			glutInitWindowPosition(0, 0);
			glutInitWindowSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
			winID = glutCreateWindow("Quidditch");
			WINDOW_WIDTH = DEFAULT_WINDOW_WIDTH;
			WINDOW_HEIGHT = DEFAULT_WINDOW_HEIGHT;

			// init cursor
			mouse_x = WINDOW_WIDTH / 2;
			mouse_y = WINDOW_HEIGHT / 2;
			SetCursorPos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
			glutSetCursor(GLUT_CURSOR_NONE);
		}
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			exit(0);
		}
		printf("OpenGL version %s\n", glGetString(GL_VERSION));
		printf("OpenGL vendor %s\n", glGetString(GL_VENDOR));
	}
	CHECK_GL;
	destroy();
	CHECK_GL;

	glDebugMessageCallback((GLDEBUGPROC) glDebug, NULL);
	//glEnable(GL_DEBUG_OUTPUT);
	glutDisplayFunc(glDisplay);
	glDepthFunc(GL_LESS);
	glutMotionFunc(glMouseMove);
	glutMouseFunc(glMouseButton);
	glutPassiveMotionFunc(glMouseMove);
	glutSpecialFunc(glSpecial);
	glutKeyboardFunc(glKeyboard);
	glutCloseFunc(glClose);

	initGL();
	CHECK_GL
	renderer = new ShadowRenderer(WINDOW_WIDTH, WINDOW_HEIGHT);
	CHECK_GL
	initGameObject();
	CHECK_GL
	initCamera();
	CHECK_GL
	initLight();
	CHECK_GL

	setScore(0);
	isGameOver = false;

	static bool hasTimer = false;
	if (!hasTimer) {
		glutTimerFunc(1000 / FPS, glFrame, 0);
		hasTimer = true;
	}

	lastSpeedUpTime = -1;
	time = -1;
}