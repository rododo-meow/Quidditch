#include "Label.h"
#include "gl.h"

using namespace std;
void Label::render() {
	glColor4fv(color);
	glRasterPos2f(position[0], position[1]);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*) text.c_str());
}