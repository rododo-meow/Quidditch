#pragma once

#ifndef _OBJ_DRAWABLE_H_
#define _OBJ_DRAWABLE_H_

#include <map>
#include <string>
#include <vector>
#include "Drawable.h"

class Texture;
class OBJDrawable : public Drawable {
	class Model {
		static std::map<std::string, Model*> loadedModel;
		struct Float3 {
			float data[3];
			Float3() : data{ 0, 0, 0 } {}
			Float3(float x, float y, float z) : data{ x, y, z } {}
			Float3(const float a[3]) : data{ a[0], a[1], a[2] } {}
		};
		unsigned int vbo; // 9 floats per vertex, 0-2: pos, 3-5: texCorrd, 6-8: normal
		unsigned int vao, ibo;
		std::vector<std::pair<std::pair<size_t, size_t>, Texture*> > textures;
		int refcnt = 0;
		int _load(const std::string &filename);
		Model(const std::string &filename);
		~Model();
	public:
		static Model* load(const std::string &filename);
		void clear();
		void render();
		void unload();
	};
	Model *model;
	void _render() override { model->render(); }
public:
	OBJDrawable(const std::string &filename);
	~OBJDrawable();
};

#endif