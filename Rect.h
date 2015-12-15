#pragma once

#ifndef _RECT_H_
#define _RECT_H_

#include "Eigen/Eigen"

class Rect {
	float color[4];
	Eigen::Vector2f position;
	float width, height;
public:
	Rect();
	float getWidth() const { return width; }
	void setWidth(float width) { this->width = width; }
	float getHeight() const { return height; }
	void setHeight(float height) { this->height = height; }
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
	void render();
};

#endif