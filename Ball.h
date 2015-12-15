#pragma once

#ifndef _BALL_H_
#define _BALL_H_

#include "Drawable.h"

class Ball : public Object {
public:
	const float radius, m;
	const bool collideWithTable;
	Ball(float radius, float m, bool collideWithTable) : radius(radius), m(m), collideWithTable(collideWithTable) {}
};

#endif