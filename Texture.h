#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <map>

class Texture {
	unsigned int texID;
	Texture(int format, int width, int height);
	Texture(const char *path);
	static std::map<std::string, Texture*> loadedTex;
	~Texture();
	int refcnt = 0;
public:
	static Texture* load(int format, int width, int height);
	static Texture* load(const char *path);
	void enable();
	void disable();
	void unload();
	inline unsigned int getID() const { return texID; }
};

#endif