#pragma once

#ifndef _FLYING_H_
#define _FLYING_H_

#include "OBJBall.h"

class Phys;
class Table;
class Flying : public OBJBall {
	Eigen::Vector3f target;
	enum {
		S_RUSHING,
		S_DOWNING,
		S_DOWN,
		S_UPING,
		S_FLOATING
	} state;
	Table *table;
public:
	Flying(const char *filename, float radius);
	void think(float deltaTime, Phys *phys);
	inline void setTable(Table *table) { this->table = table; }
};

#endif