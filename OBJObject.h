#ifndef _OBJ_OBJECT_H_
#define _OBJ_OBJECT_H_

#include <string>

#include "OBJDrawable.h"
#include "Object.h"

class OBJObject : public Object {
public:
	OBJDrawable * const drawable;
	OBJObject(const std::string &filename) : drawable(new OBJDrawable(filename)) {
		drawable->bind(this);
	}
	inline void render() { drawable->render(); }
	~OBJObject() {
		delete drawable;
	}
};

#endif