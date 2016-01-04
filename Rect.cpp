#include "Rect.h"
#include "gl.h"

Rect::Rect() :
	color{ 1, 1, 1, 1 }, position({ -1, -1 }), width(1), height(1) {
}

void Rect::render() {
	glBegin(GL_QUADS);
	glColor4fv(color);
	glVertex2f(position.x(), position.y() + height);
	glVertex2f(position.x(), position.y());
	glVertex2f(position.x() + width, position.y());
	glVertex2f(position.x() + width, position.y() + height);
	glEnd();
}