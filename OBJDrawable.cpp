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

int OBJDrawable::Model::_load(const std::string &filename) {	
	std::vector<Float3> vertex;
	vertex.push_back({ 0, 0, 0 });
	std::vector<Float3> texture_vertex;
	texture_vertex.push_back({ 0, 0, 0 });
	std::vector<Float3> norm;
	norm.push_back({ 0, 0, 0 });
	std::map<std::string, Texture*> texture;

	clear();

	FILE *objFile = fopen(filename.c_str(), "r");
	if (!objFile)
		goto fail;

	FILE *mtlFile = NULL;

	char line[4096];
	size_t line_len;
	float x, y, z;
	int a, b, c, d, e, f, g, h, i;
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
				vertex.push_back({ x, y, z });
				break;
			case 't':
				sscanf(line + 3, "%f%f%f", &x, &y, &z);
				texture_vertex.push_back({ x, y, z });
				break;
			case 'n':
				sscanf(line + 2, "%f%f%f", &x, &y, &z);
				norm.push_back({ x, y, z });
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
						if ((texture[texName] = Texture::load(path)) == NULL)
							goto fail;
					}
				}
			}
			break;
		case 'f': {
			if (renderList == -1) {
				renderList = glGenLists(1);
				glNewList(renderList, GL_COMPILE);
			}
			if (sscanf(line + 2, "%d/%d/%d %d/%d/%d %d/%d/%d", &a, &b, &c, &d, &e, &f, &g, &h, &i) != 9)
				goto fail;
			glNormal3fv(norm[c].data);
			glTexCoord2fv(texture_vertex[b].data);
			glVertex3fv(vertex[a].data);
			glNormal3fv(norm[f].data);
			glTexCoord2fv(texture_vertex[e].data);
			glVertex3fv(vertex[d].data);
			glNormal3fv(norm[i].data);
			glTexCoord2fv(texture_vertex[h].data);
			glVertex3fv(vertex[g].data);
			break;
		}
		case 'u':
			if (strncmp(line, "usemtl", 6) == 0) {
				if (renderList == -1) {
					renderList = glGenLists(1);
					glNewList(renderList, GL_COMPILE);
					texture[line + 7]->enable();
					glBegin(GL_TRIANGLES);
				} else {
					glEnd();
					texture[line + 7]->enable();
					glBegin(GL_TRIANGLES);
				}
				
			}
			break;
		}
	}

	glEnd();
	glEndList();
	for (std::map<std::string, Texture*>::iterator it = texture.begin(); it != texture.end(); it++)
		textures.push_back(it->second);

	return 0;
fail:
	glEnd();
	glEndList();
	if (objFile)
		fclose(objFile);
	if (mtlFile)
		fclose(mtlFile);
	glDeleteLists(renderList, 1);
	for (std::map<std::string, Texture*>::iterator it = texture.begin(); it != texture.end(); it++)
		if (it->second != NULL)
			it->second->unload();
	renderList = -1;
	return -1;
}

void OBJDrawable::Model::clear() {
	if (renderList != -1) {
		glDeleteLists(renderList, 1);
		renderList = -1;
	}
	for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end(); it++)
		(*it)->unload();
	textures.clear();
}

void OBJDrawable::Model::render() {
	glCallList(renderList);
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
	if (refcnt == 0)
		delete this;
}

OBJDrawable::~OBJDrawable() {
	model->unload();
}