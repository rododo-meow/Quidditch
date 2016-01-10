#include "Texture.h"
#include "gl.h"
#include "turbojpeg/turbojpeg.h"
#include "Perlin.h"
#include "util.h"

std::map<std::string, Texture*> Texture::loadedTex;

Texture::Texture(unsigned int format, size_t width, size_t height) {
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

Texture* Texture::load(const char *path) {
	if (!loadedTex.count(path))
		loadedTex[path] = new Texture(path);
	else
		loadedTex[path]->refcnt++;
	return loadedTex[path];
}

Texture * Texture::getNoiseTexture(size_t width, size_t height) {
	Texture *texture = new Texture(GL_RGB, width, height);
	glBindTexture(GL_TEXTURE_2D, texture->texID);
	float *data = new float[width * height * 3], *tmp = data;
	Perlin2D<LinearInterpolater2D<Generator2D> > perlin(rand<size_t>());
	for (size_t i = 0; i < height; i++)
		for (size_t j = 0; j < width; j++) {
			float t = perlin(4.f, 3, 1.f, 2.5f, 1.f * j / (width - 1), 1.f * i / (height - 1));
			*tmp++ = t;
			*tmp++ = t;
			*tmp++ = t;
		}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, data);
	delete data;
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
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