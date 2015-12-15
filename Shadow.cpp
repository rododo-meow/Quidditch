#include <gl/glew.h>
#include <cstdio>
#include "Shadow.h"
#include "Shader.h"

ShadowRenderer::ShadowRenderer(int WINDOW_WIDTH, int WINDOW_HEIGHT) : WINDOW_WIDTH(WINDOW_WIDTH), WINDOW_HEIGHT(WINDOW_HEIGHT) {
	struct stat fs;

	if (stat("shader/shadow.vert", &fs))
		throw std::exception("Can't read vertex shader's code");
	char *buf = (char *) malloc(fs.st_size + 1);
	FILE *f = fopen("shader/shadow.vert", "r");
	buf[fread(buf, 1, fs.st_size, f)] = 0;
	sv_vShader = new Shader(GL_VERTEX_SHADER, buf);
	fclose(f);
	free(buf);

	if (stat("shader/shadow.geom", &fs))
		throw std::exception("Can't read geometry shader's code");
	buf = (char *)malloc(fs.st_size + 1);
	f = fopen("shader/shadow.geom", "r");
	buf[fread(buf, 1, fs.st_size, f)] = 0;
	sv_gShader = new Shader(GL_GEOMETRY_SHADER, buf);
	fclose(f);
	free(buf);

	if (stat("shader/shadow.frag", &fs))
	throw std::exception("Can't read geometry shader's code");
	buf = (char *)malloc(fs.st_size + 1);
	f = fopen("shader/shadow.frag", "r");
	buf[fread(buf, 1, fs.st_size, f)] = 0;
	sv_fShader = new Shader(GL_FRAGMENT_SHADER, buf);
	fclose(f);
	free(buf);

	programSV = new ShaderProgram(3, sv_vShader, sv_gShader, sv_fShader);
	
	if (stat("shader/mtr.vert", &fs))
		throw std::exception("Can't read vertex shader's code");
	buf = (char *)malloc(fs.st_size + 1);
	f = fopen("shader/mtr.vert", "r");
	buf[fread(buf, 1, fs.st_size, f)] = 0;
	mtr_vShader = new Shader(GL_VERTEX_SHADER, buf);
	fclose(f);
	free(buf);

	if (stat("shader/mtr.frag", &fs))
		throw std::exception("Can't read geometry shader's code");
	buf = (char *)malloc(fs.st_size + 1);
	f = fopen("shader/mtr.frag", "r");
	buf[fread(buf, 1, fs.st_size, f)] = 0;
	mtr_fShader = new Shader(GL_FRAGMENT_SHADER, buf);
	fclose(f);
	free(buf);

	programMTR = new ShaderProgram(2, mtr_vShader, mtr_fShader);
	loc_texID = glGetUniformLocation(programMTR->getID(), "texID");
	
	glEnable(GL_LIGHT0);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &ambientBuffer);
	glBindTexture(GL_TEXTURE_2D, ambientBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ambientBuffer, 0);

	glGenTextures(1, &fullLightBuffer);
	glBindTexture(GL_TEXTURE_2D, fullLightBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fullLightBuffer, 0);

	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	glGenRenderbuffers(1, &stencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, stencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowRenderer::~ShadowRenderer() {
	glDeleteRenderbuffers(1, &depthBuffer);
	glDeleteRenderbuffers(1, &stencilBuffer);
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &ambientBuffer);
	glDeleteTextures(1, &fullLightBuffer);
	delete programMTR;
	delete programSV;
	delete sv_fShader;
	delete sv_gShader;
	delete sv_vShader;
	delete mtr_vShader;
	delete mtr_fShader;
}

void ShadowRenderer::stage1() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	programMTR->enable();
	glUniform1i(loc_texID, 0);
}

void ShadowRenderer::stage2() {
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CW);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
	glClear(GL_STENCIL_BUFFER_BIT);
	programSV->enable();
}

void ShadowRenderer::stage3() {
	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
}

void ShadowRenderer::end() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	programSV->disable();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fullLightBuffer);
	glStencilFunc(GL_EQUAL, 0, 0xFFFFFFFF);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glEnd();

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_TEXTURE_2D);
}

void ShadowRenderer::setLightPosition(float x, float y, float z, float w) {
	lightpos[0] = x;
	lightpos[1] = y;
	lightpos[2] = z;
	lightpos[3] = w;
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
}

void ShadowRenderer::setLightAmbient(float r, float g, float b, float a) {
	ambient[0] = r;
	ambient[1] = g;
	ambient[2] = b;
	ambient[3] = a;
}

void ShadowRenderer::setLightDiffuse(float r, float g, float b, float a) {
	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
	diffuse[3] = a;
}

void ShadowRenderer::setLightSpecular(float r, float g, float b, float a) {
	specular[0] = r;
	specular[1] = g;
	specular[2] = b;
	specular[3] = a;
}