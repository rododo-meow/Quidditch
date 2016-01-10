#include "Texture.h"
#include "gl.h"
#include "turbojpeg/turbojpeg.h"
#include "Perlin.h"
#include "util.h"
#include "png.h"

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
	int format = -1;
	{
		size_t len = strlen(filename);
		if (filename[len - 4] == '.' && filename[len - 3] == 'j' && filename[len - 2] == 'p' && filename[len - 1] == 'g')
			format = 0;
		else if (filename[len - 4] == '.' && filename[len - 3] == 'p' && filename[len - 2] == 'n' && filename[len - 1] == 'g')
			format = 1;
	}

	switch (format) {
	case 0: {
		// jpg
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
		break; }
	case 1: {
		// png
		FILE *fp;
		png_structp png_ptr;
		png_infop info_ptr;
		png_bytep* row_pointers;
		int w, h, y, color_type, temp;
		char buf[4];
		struct stat filestat;
		if (stat(filename, &filestat))
			throw std::exception("Can't open texture file");
		fp = fopen(filename, "rb");
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
		info_ptr = png_create_info_struct(png_ptr);
		setjmp(png_jmpbuf(png_ptr));
		/* 读取PNG_BYTES_TO_CHECK个字节的数据 */
		temp = fread(buf, 1, 4, fp);
		/* 若读到的数据并没有PNG_BYTES_TO_CHECK个字节 */
		if (temp < 4) {
			fclose(fp);
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			throw "Not a png file";
		}
		/* 检测数据是否为PNG的签名 */
		temp = png_sig_cmp((png_bytep)buf, (png_size_t)0, 4);
		/* 如果不是PNG的签名，则说明该文件不是PNG文件 */
		if (temp != 0) {
			fclose(fp);
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			throw "Not a png file";
		}

		/* 复位文件指针 */
		rewind(fp);
		/* 开始读文件 */
		png_init_io(png_ptr, fp);
		/* 读取PNG图片信息 */
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0);
		/* 获取图像的色彩类型 */
		color_type = png_get_color_type(png_ptr, info_ptr);
		/* 获取图像的宽高 */
		w = png_get_image_width(png_ptr, info_ptr);
		h = png_get_image_height(png_ptr, info_ptr);
		/* 获取图像的所有行像素数据，row_pointers里边就是rgba数据 */
		row_pointers = png_get_rows(png_ptr, info_ptr);
		/* 根据不同的色彩类型进行相应处理 */
		switch (color_type) {
		case PNG_COLOR_TYPE_RGB_ALPHA: {
			unsigned char *rgbaData = (unsigned char *) malloc(w * h * 4), *tmp = rgbaData;
			for (y = 0; y < h; ++y) {
				memcpy(tmp, row_pointers[y], w * 4);
				tmp += w * 4;
			}
			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//此为纹理过滤参数设置
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			free(rgbaData);
			break; }
		case PNG_COLOR_TYPE_RGB: {
			unsigned char *rgbData = (unsigned char *) malloc(w * h * 3), *tmp = rgbData;
			for (y = 0; y < h; ++y) {
				memcpy(tmp, row_pointers[y], w * 3);
				tmp += w * 3;
			}
			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbData);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//此为纹理过滤参数设置
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			free(rgbData);
			break; }
		}
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		fclose(fp);
		break; }
	}
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