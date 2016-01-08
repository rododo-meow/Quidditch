#include "OBJDrawable.h"
#include "Transforms.h"
#include "Texture.h"

#include <map>

std::map<std::string, OBJDrawable::Model*> OBJDrawable::Model::loadedModel;

OBJDrawable::OBJDrawable(const std::string &filename) : model(Model::load(filename)) {
}

OBJDrawable::Model::Model(const std::string &filename) {
	_load(filename);
}

#define REUSE(line) do { \
	if (!reuseMap.count(line)) { \
		sscanf((line), "%d/%d/%d", &a, &b, &c); \
		vbodata.push_back(vertex[a * 3]); \
		vbodata.push_back(vertex[a * 3 + 1]); \
		vbodata.push_back(vertex[a * 3 + 2]); \
		vbodata.push_back(texture_vertex[b * 3]); \
		vbodata.push_back(texture_vertex[b * 3 + 1]); \
		vbodata.push_back(texture_vertex[b * 3 + 2]); \
		vbodata.push_back(norm[c * 3]); \
		vbodata.push_back(norm[c * 3 + 1]); \
		vbodata.push_back(norm[c * 3 + 2]); \
		reuseMap[(line)] = reuseMap.size(); } } while(0)

int OBJDrawable::Model::_load(const std::string &filename) {	
	std::vector<float> vertex(3);
	std::vector<float> texture_vertex(3);
	std::vector<float> norm(3);
	std::vector<size_t> vindex;
	std::vector<float> vbodata;
	std::map<std::string, Texture*> texture;
	std::map<std::string, size_t> reuseMap;

	clear();

	FILE *objFile = fopen(filename.c_str(), "r");
	if (!objFile)
		goto fail;

	FILE *mtlFile = NULL;
	
	char line[4096], subline1[64], subline2[64], subline3[64];
	size_t line_len;
	float x, y, z;
	int a, b, c;
	while (fgets(line, 4096, objFile)) {
		line_len = strlen(line);
		if (line[line_len - 1] != '\n')
			goto fail;
		switch (line[0]) {
		case '#':
			break;
		case 'v':
			switch (line[1]) {
			case ' ':
				sscanf(line + 2, "%f%f%f", &x, &y, &z);
				vertex.push_back(x);
				vertex.push_back(y);
				vertex.push_back(z);
				break;
			case 't':
				sscanf(line + 3, "%f%f%f", &x, &y, &z);
				texture_vertex.push_back(x);
				texture_vertex.push_back(y);
				texture_vertex.push_back(z);
				break;
			case 'n':
				sscanf(line + 2, "%f%f%f", &x, &y, &z);
				norm.push_back(x);
				norm.push_back(y);
				norm.push_back(z);
				break;
			}
			break;
		case 'm':
			if (strncmp(line, "mtllib", 6) == 0) {
				char path[256], *p = path;
				strcpy(path, filename.c_str());\
				while (*p++);
				while (p >= path && *p != '/') p--;
				p++;
				while (*p) *p++ = 0;
				while (!*--p);
				sscanf(line + 7, "%s", p + 1);

				mtlFile = fopen(path, "r");
				if (mtlFile == NULL)
					goto fail;

				std::string texName;
				while (fgets(line, 4096, mtlFile)) {
					if (strncmp(line, "newmtl", 6) == 0) {
						texName = line + 7;
					} else if (strncmp(line, "\tmap_Ka", 7) == 0) {
						sscanf(line + 8, "%s", p + 1);
						if (strcmp(p + 1, "noise") == 0)
							texture[texName] = Texture::getNoiseTexture(256, 256);
						else if ((texture[texName] = Texture::load(path)) == NULL)
							goto fail;
					}
				}
			}
			break;
		case 'f': {
			if (sscanf(line + 2, "%s %s %s", subline1, subline2, subline3) != 3)
				goto fail;
			REUSE(subline1);
			REUSE(subline2);
			REUSE(subline3);
			vindex.push_back(reuseMap[subline1]);
			vindex.push_back(reuseMap[subline2]);
			vindex.push_back(reuseMap[subline3]);
			break;
		}
		case 'u':
			if (textures.empty())
				textures.push_back(std::make_pair(std::make_pair(0, 0), texture[line + 7]));
			else {
				textures.back().first.second = vindex.size() - textures.back().first.first;
				textures.push_back(std::make_pair(std::make_pair(vindex.size(), 0), texture[line + 7]));
			}
			break;
		}
	}

	if (!textures.empty())
		textures.back().first.second = vindex.size() - textures.back().first.first;

	// setup up opengl
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vbodata.size() * sizeof(float), &vbodata[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (6 * sizeof(float)));
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vindex.size() * sizeof(unsigned int), &vindex[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return 0;
fail:
	if (objFile)
		fclose(objFile);
	if (mtlFile)
		fclose(mtlFile);
	for (std::map<std::string, Texture*>::iterator it = texture.begin(); it != texture.end(); it++)
		if (it->second != NULL)
			it->second->unload();
	texture.clear();
	vbo = vao = -1;
	return -1;
}

void OBJDrawable::Model::clear() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	for (std::vector<std::pair<std::pair<size_t, size_t>, Texture*> >::const_iterator it = textures.begin(); it != textures.end(); it++)
		it->second->unload();
	textures.clear();
}

void OBJDrawable::Model::render() {
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	for (std::vector<std::pair<std::pair<size_t, size_t>, Texture*> >::const_iterator it = textures.begin(); it != textures.end(); it++) {
		it->second->enable();
		glDrawElementsBaseVertex(GL_TRIANGLES, it->first.second, GL_UNSIGNED_INT, (void *) (it->first.first * sizeof(unsigned int)), 0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

OBJDrawable::Model::~Model() {
	clear();
}

OBJDrawable::Model* OBJDrawable::Model::load(const std::string &path) {
	if (!loadedModel.count(path))
		loadedModel[path] = new Model(path);
	loadedModel[path]->refcnt++;
	return loadedModel[path];
}

void OBJDrawable::Model::unload() {
	refcnt--;
	if (refcnt == 0) {
		for (std::map<std::string, Model*>::iterator it = loadedModel.begin(); it != loadedModel.end(); it++)
			if (it->second == this) {
				loadedModel.erase(it);
				break;
			}
		delete this;
	}
}

OBJDrawable::~OBJDrawable() {
	model->unload();
}