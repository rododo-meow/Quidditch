#pragma once

#ifndef _PHYS_H_
#define _PHYS_H_

#include <map>
#include <list>
#include "Eigen/Eigen"

class Ball;

class Phys {
	struct Data {
		Eigen::Vector3f velocity;
		Eigen::Vector3f force;
		Data() : velocity(0.f, 0.f, 0.f), force(0.f, 0.f, 0.f) {}
	};
	std::map<Ball*, Data*> balls;
public:
	typedef void (*AfterCollision)(void *that, Ball *ball1, Ball *ball2);
private:
	std::list<std::pair<AfterCollision, void*> > afterCollisionCallbacks;
public:
	void addAfterCollision(AfterCollision callback, void *that) {
		afterCollisionCallbacks.push_back(std::make_pair(callback, that));
	}
	Phys() {};
	void addBall(Ball *ball) { balls.insert(std::make_pair(ball, new Data())); }
	void update(float deltaTime);
	const Eigen::Vector3f getVelocity(Ball *ball) const { return balls.at(ball)->velocity; }
	void addForce(Ball *ball, const Eigen::Vector3f &f) { balls.at(ball)->force += f; }
	void setVelocity(Ball *ball, const Eigen::Vector3f &v) { balls.at(ball)->velocity = v; }
};

#endif