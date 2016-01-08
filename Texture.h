#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <map>

class Texture {
	unsigned int texID;
	Texture(unsigned int format, size_t width, size_t height);
	Texture(const char *path);
	static std::map<std::string, Texture*> loadedTex;
	~Texture();
	int refcnt = 1;
public:
	static Texture* load(const char *path);
	static Texture* getNoiseTexture(size_t width, size_t height);
	void enable();
	void disable();
	void unload();
	inline unsigned int getID() const { return texID; }
};

#endif