#pragma once

#include "OBJObject.h"
#include <string>
#include <iostream>

#include "Ball.h"

class OBJBall : public Ball {
private:
	bool duringTeleport;
	static void _positionChanged(void *that, Object *obj, const Eigen::Vector3f &oldP, const Eigen::Vector3f &newP) {
		((OBJBall *)that)->positionChanged(oldP, newP);
	}
	static bool _beforeChangeRotation(void *that, Object *obj, const Eigen::Matrix4f &oldP, const Eigen::Matrix4f &newP) {
		return false;
	}
	void positionChanged(const Eigen::Vector3f &oldP, const Eigen::Vector3f &newP) {
		if (duringTeleport)
			return;
		if ((newP - oldP).cross(Eigen::Vector3f({ 0.f, 1.f, 0.f })).norm() > 0.005 * radius)
			rotation = matRotate((newP - oldP).cross(Eigen::Vector3f({ 0.f, 1.f, 0.f })), ((newP - oldP).norm() / radius) * (180.f / F_PI)) * rotation;
	}
public:
	OBJDrawable * const drawable;
	OBJBall(const std::string &filename, float radius, float m = 0.1f, bool collideWithTable = true) : Ball(radius, m, collideWithTable), drawable(new OBJDrawable(filename)) {
		drawable->bind(this);
		setScale(Eigen::Vector3f(radius / 0.05f, radius / 0.05f, radius / 0.05f));
		addPositionChanged(_positionChanged, this);
		addBeforeChangeRotation(_beforeChangeRotation, NULL);
	}
	void teleport(const Eigen::Vector3f &position) {
		duringTeleport = true;
		setPosition(position);
		duringTeleport = false;
	}
	inline void render() { drawable->render(); }
	~OBJBall() {
		delete drawable;
	}
};