#pragma once

#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include "Eigen/Eigen"

class ProgressBar {
	float color_left[4], color_right[4];
	Eigen::Vector2f position;
	float width, height;
	float progress;
	void linearBlend();
public:
	ProgressBar();
	float getProgress() const { return progress; }
	void setProgress(float progress) { if (progress >= 0 && progress <= 1) this->progress = progress; }
	float getWidth() const { return width; }
	void setWidth(float width) { this->width = width; }
	float getHeight() const { return height; }
	void setHeight(float height) { this->height = height; }
	const Eigen::Vector2f& getPosition() const { return position; }
	void setPosition(const Eigen::Vector2f &position) { this->position = position; }
	const float * const getLeftColor() const { return color_left; }
	const float * const getRightColor() const { return color_right; }
	void setLeftColor(const float color[]) {
		setLeftColor(color[0], color[1], color[2], color[3]);
	}
	void setLeftColor(float r, float g, float b, float a) {
		this->color_left[0] = r;
		this->color_left[1] = g;
		this->color_left[2] = b;
		this->color_left[3] = a;
	}
	void setRightColor(const float color[]) {
		setRightColor(color[0], color[1], color[2], color[3]);
	}
	void setRightColor(float r, float g, float b, float a) {
		this->color_right[0] = r;
		this->color_right[1] = g;
		this->color_right[2] = b;
		this->color_right[3] = a;
	}
	void render();
};

#endif