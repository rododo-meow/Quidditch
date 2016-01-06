#include "Texture.h"
#include "gl.h"
#include "turbojpeg/turbojpeg.h"

std::map<std::string, Texture*> Texture::loadedTex;

Texture::Texture(int format, int width, int height) {
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

Texture::Texture(const char *filename) {
	tjhandle decompressor = tjInitDecompress();
	struct stat filestat;
	if (stat(filename, &filestat))
		throw std::exception("Can't open texture file");
	unsigned char *jpegData = (unsigned char *)malloc(filestat.st_size);
	FILE *jpegFile = fopen(filename, "rb");
	fread(jpegData, 1, filestat.st_size, jpegFile);
	fclose(jpegFile);

	int width, height, subsample, colorspace;
	tjDecompressHeader3(decompressor, jpegData, filestat.st_size, &width, &height, &subsample, &colorspace);
	char * r = tjGetErrorStr();
	unsigned char *rgbData = (unsigned char *)malloc(width * height * tjPixelSize[TJPF_RGB]);
	tjDecompress2(decompressor, jpegData, filestat.st_size, rgbData, width, width * tjPixelSize[TJPF_RGB], height, TJPF_RGB, TJFLAG_BOTTOMUP | TJFLAG_NOREALLOC);
	tjDestroy(decompressor);
	free(jpegData);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//此为纹理过滤参数设置
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	free(rgbData);
}

Texture::~Texture() {
	glDeleteTextures(1, &texID);
	for (std::map<std::string, Texture*>::iterator it = loadedTex.begin(); it != loadedTex.end(); it++)
		if (it->second == this) {
			loadedTex.erase(it);
			break;
		}
}

Texture* Texture::load(int format, int width, int height) {
	return new Texture(format, width, height);
}

Texture* Texture::load(const char *path) {
	if (!loadedTex.count(path))
		loadedTex[path] = new Texture(path);
	loadedTex[path]->refcnt++;
	return loadedTex[path];
}

void Texture::unload() {
	refcnt--;
	if (refcnt == 0)
		delete this;
}

void Texture::enable() {
	glBindTexture(GL_TEXTURE_2D, texID);
}

void Texture::disable() {
	glBindTexture(GL_TEXTURE_2D, 0);
}