#include "ProgressBar.h"
#include "gl.h"

ProgressBar::ProgressBar() : 
	color_left{ 1, 1, 1, 1 }, color_right{ 1, 1, 1, 1 }, position({ 0, 0 }), width(0.1f), height(0.03f), progress(1) {
}

void ProgressBar::linearBlend() {
	float color[4];
	color[0] = color_left[0] * (1 - progress) + color_right[0] * progress;
	color[1] = color_left[1] * (1 - progress) + color_right[1] * progress;
	color[2] = color_left[2] * (1 - progress) + color_right[2] * progress;
	color[3] = color_left[3] * (1 - progress) + color_right[3] * progress;
	glColor4fv(color);
}

void ProgressBar::render() {
	glBegin(GL_QUADS);
	glColor4fv(color_left);
	glVertex2f(position.x(), position.y() + height);
	glVertex2f(position.x(), position.y());
	linearBlend();
	glVertex2f(position.x() + progress * width, position.y());
	glVertex2f(position.x() + progress * width, position.y() + height);
	glEnd();
}