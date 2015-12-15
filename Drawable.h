#pragma once

#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

#include <gl/glew.h>
#include <gl/freeglut.h>
#include "Object.h"
#include "glMatUtil.h"
#include "Transforms.h"

class Drawable {
protected:
	virtual void _render() = 0;
private:
	float ambient[4];
	float diffuse[4];
	float specular[4];
	const float ambient_factor = .5f;
	const float diffuse_factor = .3f;
	const float specular_factor = .00001f;
	const float shininess = 20;
	Object *object;
public:
	inline void bind(Object *object) { this->object = object; }
	Drawable() : object(NULL) { setColor(1.f, 1.f, 1.f, 1.f); }
	void setColor(float r, float g, float b, float a) {
		ambient[0] = r * ambient_factor;
		ambient[1] = g * ambient_factor;
		ambient[2] = b * ambient_factor;
		ambient[3] = a;
		diffuse[0] = r * diffuse_factor;
		diffuse[1] = g * diffuse_factor;
		diffuse[2] = b * diffuse_factor;
		diffuse[3] = a;
		specular[0] = r * specular_factor;
		specular[1] = g * specular_factor;
		specular[2] = b * specular_factor;
		specular[3] = a;
	}
	void setColor(float rgba[]) {
		setColor(rgba[0], rgba[1], rgba[2], rgba[3]);
	}
	inline void render() {
		glMultiMatrix(matTranslate(object->getPosition()));
		glMultiMatrix(object->getRotation());
		glMultiMatrix(matScale(object->getScale()));
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		this->_render();
		glMultiMatrix(matScale(1 / object->getScale().x(), 1 / object->getScale().y(), 1 / object->getScale().z()));
		glMultiMatrix(object->getRotation().inverse());
		glMultiMatrix(matTranslate(-object->getPosition()));
	}
};

#endif