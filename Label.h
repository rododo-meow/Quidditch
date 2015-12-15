#pragma once

#ifndef _LABEL_H_
#define _LABEL_H_

#include <string>
#include "Eigen/Eigen"

class Label {
	float color[4];
	Eigen::Vector2f position;
public:
	const Eigen::Vector2f& getPosition() const { return position; }
	void setPosition(const Eigen::Vector2f &position) { this->position = position; }
	const float * const getColor() const { return color; }
	void setColor(const float color[]) {
		setColor(color[0], color[1], color[2], color[3]);
	}
	void setColor(float r, float g, float b, float a) {
		this->color[0] = r;
		this->color[1] = g;
		this->color[2] = b;
		this->color[3] = a;
	}
	std::string text;
	void render();
};

#endif