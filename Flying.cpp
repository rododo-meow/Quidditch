#include "Flying.h"
#include "util.h"
#include "Game.h"
#include "GameParam.h"
#include "Eigen/Eigen"
#include "Phys.h"

using namespace Eigen;
using namespace GameParam;

Flying::Flying(float radius) : Ball(radius, 10, false), state(S_FLOATING) {
	bind(this);
}

void Flying::_render() {
	glutSolidSphere(radius, 40, 30);
}

void Flying::think(float deltaTime, Phys *phys) {
	switch (state) {
	case S_FLOATING: {
		float p = rand(0, 1);
		if (p < FLYING_RUSH_P) {
			target = Vector3f(
				rand(FLYING_MIN_X, FLYING_MAX_X),
				rand(TABLE_FACE_Y + BALL_RADIUS + FLYING_MIN_FLY_HEIGHT, TABLE_FACE_Y + BALL_RADIUS + FLYING_MAX_FLY_HEIGHT),
				rand(FLYING_MIN_Z, FLYING_MAX_Z));
			state = S_RUSHING;
			break;
		}
		p -= FLYING_RUSH_P;
		if (p < FLYING_RELAX_P) {
			target = Vector3f(
				rand(-TABLE_LENGTH / 2 + radius, TABLE_LENGTH / 2 - radius),
				TABLE_FACE_Y + BALL_RADIUS,
				rand(-TABLE_WIDTH / 2 + radius, TABLE_WIDTH / 2 - radius));
			state = S_DOWNING;
			break;
		}
		p -= FLYING_RELAX_P;
		break;
	}
	case S_RUSHING:
		if ((target - getPosition()).norm() < deltaTime * FLYING_RUSH_VELOCITY) {
			phys->setVelocity(this, vec4To3(matRotateAroundY(rand(0, 360)) * Vector4f({ FLYING_FLOATING_VELOCITY, 0, 0, 1 })));
			state = S_FLOATING;
		} else
			phys->setVelocity(this, (target - getPosition()).normalized() * FLYING_RUSH_VELOCITY);
		break;
	case S_DOWNING:
		if ((target - getPosition()).norm() < deltaTime * FLYING_DOWN_VELOCITY) {
			state = S_DOWN;
			phys->setVelocity(this, Vector3f({ 0, 0, 0 }));
		} else {
			Vector3f v = target - getPosition();
			if (getPosition().x() < -TABLE_LENGTH / 2 + radius || getPosition().x() > TABLE_LENGTH / 2 - radius ||
				getPosition().z() < -TABLE_WIDTH / 2 + radius || getPosition().z() > TABLE_WIDTH / 2 - radius)
				v.y() = 0;
			v.normalize();
			phys->setVelocity(this, v * FLYING_DOWN_VELOCITY);
		}
		break;
	case S_DOWN: {
		float p = rand(0, 1);
		if (p < FLYING_WAKEUP_P) {
			target = Vector3f(
				getPosition().x(),
				rand(TABLE_FACE_Y + BALL_RADIUS + FLYING_MIN_FLY_HEIGHT, TABLE_FACE_Y + BALL_RADIUS + FLYING_MAX_FLY_HEIGHT),
				getPosition().y());
			state = S_UPING;
		}
		break;
	}
	case S_UPING:
		if ((target - getPosition()).norm() < deltaTime * FLYING_DOWN_VELOCITY)
			state = S_FLOATING;
		else
			phys->setVelocity(this, (target - getPosition()).normalized() * FLYING_DOWN_VELOCITY);
		break;
	}
}
	