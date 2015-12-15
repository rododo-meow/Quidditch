#pragma once

#ifndef _FLYING_H_
#define _FLYING_H_

#include "Ball.h"

class Phys;
class Flying : public Ball, public Drawable {
	void _render() override;
	Eigen::Vector3f target;
	enum {
		S_RUSHING,
		S_DOWNING,
		S_DOWN,
		S_UPING,
		S_FLOATING
	} state;
public:
	Flying(float radius);
	void think(float deltaTime, Phys *phys);
};

#endif